import sys
import argparse
import pprint
import os
import re
import json
import subprocess
from typing import Dict, List, Optional, Set
import signal
from pathlib import Path

from msim_logger import logger
from s3_utils import Helper as S3helper

from msim_bag_checker.base_check import BaseChecker, build_exception_checker_result
from msim_bag_checker.bag_loader.bag_loader import BagLoader, GetTopicFromBagError, ParserTopicError
from msim_bag_checker.checker_list import (
    critical_checkers_map,
    non_critical_checkers_map,
)
from msim_defs import CheckerResult, ErrorReport, ResultType, ErrorCode, SingleCheckerResult
from msim_bag_checker.old_interface import transform_excessive_interface
from msim_bag_checker.tasks.base import SingleCheckerBase
from msim_bag_checker.utils.uploader import (
    Uploader,
    UploaderLocalImpl,
    UploaderS3Impl,
)
from msim_bag_checker.utils import resources_monitor

RESULT_FILE_NAME = "result.json"
LOCAL_RESULT_PATH = Path("~/.checker_result").expanduser()
LOCAL_RESULT_PATH.mkdir(parents=True, exist_ok=True)
LOCAL_RESULT_FILE = LOCAL_RESULT_PATH.joinpath(RESULT_FILE_NAME)


def write_result_file(result_dict: Dict):
    with open(LOCAL_RESULT_FILE, 'w+') as outfile:
        json.dump(result_dict, outfile, default=lambda o: o.__dict__, indent=4, separators=(", ", ": "), sort_keys=True)
    print(f"Generate final result file {LOCAL_RESULT_FILE}")


class CaseChecker:

    def __init__(self) -> None:
        self.checker_result = CheckerResult()
        self.critical_checkers: List[SingleCheckerBase] = []
        self.non_critical_checkers: List[SingleCheckerBase] = []
        self.bag_loader: BagLoader
        self.additional_info_uploader: Uploader
        self.final_result_uploader: Uploader
        self.base_check: BaseChecker
        self._start_time = None
        self._end_time = 0
        self.monitor_process = None

    def start_monitor(self, output_dir: str):
        current_pid = os.getpid()
        resource_monitor_cmd = f"python {resources_monitor.__file__} {current_pid} {output_dir}"
        print(f"[Monitor Start] cmd {resource_monitor_cmd}")
        self.monitor_process = subprocess.Popen(resource_monitor_cmd, shell=True, preexec_fn=os.setsid)

    def stop_monitor(self):
        if self.monitor_process:
            os.killpg(os.getpgid(self.monitor_process.pid), signal.SIGINT)

    def check_need_monitor(self, config, output_dir: str):
        key_word = "-resource_monitor"
        if key_word in config:
            self.start_monitor(output_dir)
            config = config.replace(key_word, '')
        return config

    def run(self) -> None:
        # print(f"[Input Arguements] {sys.argv}")
        # parser = argparse.ArgumentParser(description="Case Checker")

        # # TODO: 临时适配旧接口
        # if os.getenv("CHECKER_VERSION") == "OLD":
        #     parser.add_argument("bag", type=str, help="the path of the bag.")
        #     parser.add_argument("output",
        #                         type=str,
        #                         help="the path of the output report.")
        #     parser.add_argument("config", type=str, help="the tag and params")
        # else:
        #     parser.add_argument("--bag",
        #                         type=str,
        #                         help="the path of the bag.",
        #                         required=True)
        #     parser.add_argument("--output",
        #                         type=str,
        #                         help="the path of the output report.",
        #                         required=True)
        #     parser.add_argument("--config", type=str, help="the tag and params")
        # args = parser.parse_args()
        # task_config = args.config
        # task_config = self.check_need_monitor(task_config, args.output)
        # run_cmd = f"poetry run case_checker --config '{task_config}' --bag {args.bag} --output {args.output}"
        # logger.info(run_cmd)
        # self._result_add_info("RunCmd", run_cmd)
        # self._result_add_info("Environment Variables", dict(os.environ))
        try:
            # self._init(args.bag, args.output)
            # bag_path = "/home/ros/Downloads/checker/16-PLAAG1715_event_manual_recording_20220322-233155_0.bag"
            # bag_path = "/home/ros/Downloads/prob/ALAVGoodevents/PLAG92593_recording_recording_rviz_20220127-143615_20220127-143700_0.bag"
            bag_path = "/home/ros/Downloads/prob0519/cpicpd/PLAA95761_event_dbw_disabled_takeover_20220418-154915_0.bag"
            self._init(bag_path, ".")
        except Exception:
            self._exit_with_checker_error(ErrorCode().create("s002000"), "CheckerError-S3Helper")
            write_result_file(self.checker_result)
            self.final_result_uploader.upload_file_obj(self.checker_result)
            return

        # self.base_check = BaseChecker(args.bag, self.checker_result)
        self.base_check = BaseChecker(bag_path, self.checker_result)
        is_success, result = self.base_check.run()
        if not is_success:
            write_result_file(result)
            self.final_result_uploader.upload_file_obj(result)
            return
        try:
            # config = self._evaluation_time_get(task_config)
            # need_data_set, add_parser_maps = self._register_checkers(config)
            # self._load_data(args.bag, need_data_set, add_parser_maps)
            config = self._evaluation_time_get("-cldcpd")
            need_data_set, add_parser_maps = self._register_checkers(config)
            # self._load_data(args.bag, need_data_set, add_parser_maps)
            self._load_data(bag_path, need_data_set, add_parser_maps)
            self._check()
            self._aggregate_data()
        except GetTopicFromBagError:
            self._exit_with_checker_error(ErrorCode().create("s002000"), "CheckerError-TopicRead")
            logger.error("stop check for download or parser sim-after bag file error")
        except ParserTopicError:
            self._exit_with_checker_error(ErrorCode().create("c002000"), "CheckerError-TopicParser")
            logger.error("stop check for download or parser sim-after bag file error")
        except Exception:
            self._exit_with_checker_error(ErrorCode().create("c001000"), "CheckerError-others")

        write_result_file(self.checker_result)
        self._check_result_file_size()
        self.final_result_uploader.upload_file_obj(self.checker_result)

    def _result_add_info(self, info_type, info, checker_name="Info"):
        if checker_name in self.checker_result.checkers_result:
            self.checker_result.checkers_result[checker_name].data.setdefault(info_type, info)
            return
        mock_checker_result = SingleCheckerResult()
        mock_checker_result.name = checker_name
        mock_checker_result.msg = "Information for debug"
        mock_checker_result.data.setdefault(info_type, info)
        self.checker_result.checkers_result.setdefault(checker_name, mock_checker_result)

    def _exit_with_checker_error(self, error_code=ErrorCode(), error_name: str=""):
        self.checker_result.error_code = error_code
        build_exception_checker_result(self.checker_result, error_name)
        logger.error(self.checker_result.error_report.exc_tb)

    def _init(self, bag_path: str, output_path: str) -> None:
        path, name = os.path.split(bag_path)
        name, ext = os.path.splitext(name)
        assert name
        self.checker_result.bag_path = path
        self.checker_result.scenario_name = name

        if output_path.startswith("s3://"):
            upload_helper = S3helper(env_name="UPLOAD_")
            bucket, object_key = upload_helper.get_bucket_and_key(output_path)

            self.additional_info_uploader = UploaderS3Impl(
                helper=upload_helper,
                bucket=bucket,
                key_prefix=f"{object_key}.uploads/",
            )
            self.final_result_uploader = UploaderS3Impl(helper=upload_helper,
                                                        bucket=bucket,
                                                        key_prefix=object_key)
        else:
            scenario_name = self.checker_result.scenario_name
            result_dir = os.path.join(output_path, scenario_name)
            uploads_dir = os.path.join(result_dir, "result.json.uploads")
            self.additional_info_uploader = UploaderLocalImpl(
                uploads_dir=uploads_dir)
            self.final_result_uploader = UploaderLocalImpl(
                uploads_dir=result_dir)

    def _evaluation_time_get(self, config):
        key_word = "-split"
        match_pattern = r'.*(%s\s*)(\d+):(\d+)' % key_word
        match_result = re.match(match_pattern, config)
        if match_result:
            self._start_time = int(match_result.group(2))
            self._end_time = int(match_result.group(3))
            logger.info("filter rosbag with start time %s and end time %s" % (self._start_time, self._end_time))
            config = config.replace("%s%s:%s" % (match_result.group(1), self._start_time, self._end_time), '')
        elif key_word in config:
            config = config.replace(key_word, '')
        return config

    def _register_checkers(self, tags_str: str) -> List:

        def _init_checker(checker: SingleCheckerBase, params: list, uploader: Uploader, meta: Dict, new_parser_maps: Dict) -> None:
            checker.init_uploader(uploader)
            checker.parse_params(params)
            checker.set_meta(meta)
            checker.set_bag_info(self.base_check.bag_topics_info)
            checker.add_new_parser_map(new_parser_maps)

        critical_tag: Dict[str, List] = {}
        non_critical_tag: Dict[str, List] = {}
        current_param: Optional[List] = None
        meta_json_list = []
        for tag in tags_str.split():
            if tag == '--meta':
                current_param = meta_json_list
                continue
            # TODO: 旧接口适配
            if tag.startswith("-"):
                if tag.startswith("--"):
                    logger.warning(f"Old tags are not supported: {tag}")
                    continue
                if tag in critical_checkers_map:
                    critical_tag[tag] = []
                    current_param = critical_tag[tag]
                elif tag in non_critical_checkers_map:
                    non_critical_tag[tag] = []
                    current_param = non_critical_tag[tag]
                else:
                    logger.error("use unknown tag: %s" % tag)
            elif current_param is None:
                logger.error(f"Error in first parameter， does not start with ‘-’, {tag}")
            else:
                current_param.append(tag)
        meta_dict = {}
        try:
            if len(meta_json_list):
                json_str = meta_json_list[0]
                meta_dict = eval(json_str)
                print("get meta information <%s> %s" % (type(meta_dict), meta_dict))
        except:
            logger.warning("import meta is not json format. %s" % meta_json_list)
        need_data_set = set()

        logger.info(f"checker get meta {meta_dict}")
        topics_name = list(self.base_check.bag_topics_info.topics.keys())
        logger.info(f"checker get origin bag info: {len(topics_name)} topcis: {topics_name}")

        new_parser_maps = {}
        for tag, params in critical_tag.items():
            checker = critical_checkers_map[tag]()
            need_data_set.update(checker.get_required_data())
            _init_checker(checker, params, self.additional_info_uploader, meta_dict, new_parser_maps)
            need_data_set.update(set(new_parser_maps.keys()))
            self.critical_checkers.append(checker)

        for tag, params in non_critical_tag.items():
            checker = non_critical_checkers_map[tag]()
            need_data_set.update(checker.get_required_data())
            _init_checker(checker, params, self.additional_info_uploader, meta_dict, new_parser_maps)
            need_data_set.update(set(new_parser_maps.keys()))
            self.non_critical_checkers.append(checker)
        logger.info(
            f"critical_checkers: {[v.get_name() for v in self.critical_checkers]}"
        )
        logger.info(
            f"non_critical_checkers: {[v.get_name() for v in self.non_critical_checkers]}"
        )
        return list(need_data_set), new_parser_maps

    def _load_data(self, bag_path: str, need_data_list: List[str], add_parser_maps: Dict) -> None:
        self.bag_loader = BagLoader(bag_path)
        self.bag_loader.add_parser_func_map(add_parser_maps)
        self.bag_loader.parse_bag_data(need_data_list, self._start_time, self._end_time)

    def _check(self) -> None:
        for checker in self.critical_checkers:
            checker.check(self.bag_loader)
            single_result = checker.get_result()
            self.checker_result.checkers_result[
                single_result.name] = single_result
            if single_result.result_code is not ResultType.SUCCESS:
                return
        for checker in self.non_critical_checkers:
            checker.check(self.bag_loader)
            single_result = checker.get_result()
            self.checker_result.checkers_result[
                single_result.name] = single_result

    def _aggregate_data(self) -> None:
        checker_error_code_map = {
            ResultType.SUCCESS: "p000000",
            ResultType.RAW_DATA_ERROR: "r003000",
            ResultType.SIMULATION_ERROR: "s005000",
            ResultType.ALGORITHM_ERROR: "a001000",
            ResultType.CHECKER_ERROR: "c002001",
            ResultType.ALGORITHM_USABILITY_ERROR: "u001000"
        }
        self.checker_result.get_result_code_from_checkers()
        result_code = self.checker_result.result_code
        if result_code not in checker_error_code_map.keys():
            raise EnvironmentError(f"Result Type {result_code} beyond define!")
        self.checker_result.error_code = ErrorCode.create(checker_error_code_map[result_code])
        if result_code == ResultType.ALGORITHM_ERROR:
            checker_error_codes = set([result.error_code.first_code for result in
                self.checker_result.checkers_result.values() if result.result_code == ResultType.ALGORITHM_ERROR])
            self.checker_result.error_code = ErrorCode.create('a'+ min(checker_error_codes) + "000")

    def _check_result_file_size(self, size_threthold: int=60000):
        file_size = LOCAL_RESULT_FILE.stat().st_size
        if file_size > size_threthold:
            error_log = f"please check the log to find which checker result with large size content, which make result size {file_size} > {size_threthold}"
            logger.error(error_log)
            self.checker_result.result_code = ResultType.CHECKER_ERROR
            self.checker_result.error_code = ErrorCode().create("c002001")
            for checker_name, result in self.checker_result.checkers_result.items():
                self.checker_result.checkers_result[checker_name].data = {}
            self.checker_result.checkers_result["ResultSizeTooLarge"] = SingleCheckerResult(
                name="ResultSizeTooLarge",
                result_code=ResultType.CHECKER_ERROR,
                error_code=ErrorCode().create("c002001"),
                msg=error_log)


def main() -> None:
    logger.info(f"system environment variables:\n {os.environ}")
    case_checker = CaseChecker()
    case_checker.run()
    case_checker.stop_monitor()
    result_dict = case_checker.checker_result.__dict__
    pprint.pprint(result_dict)
    print(f"FINAL RESULT: {case_checker.checker_result.result_code.name}")


if __name__ == '__main__':
    main()
