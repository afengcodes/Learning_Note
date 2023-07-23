from typing import Any, Dict, List
import attr
from msim_bag_checker.bag_loader.bag_loader import BagLoader
from msim_defs import DataBase
from msim_bag_checker.tasks.base import ResultType, SingleCheckerBase
from msim_bag_checker.third_party.frenet.refline import RawRefLine , RefLine
import math
import matplotlib.pyplot as plt

class WrongBrakeCPIChecker(SingleCheckerBase):
    # check方法
    def process(self, bag_data: BagLoader) -> None:
        professor_traj = bag_data.data.origin_egopose 
        egopose = bag_data.data.egopose 
        accelerations = bag_data.data.accelerations
        origin_accelerations = bag_data.data.origin_accelerations
        lon_cipv = bag_data.data.lon_cipv
        lon_obs = bag_data.data.lon_obs
        obj_info_mem = bag_data.data.obj_info_mem
        # print("accelerations[0].stamp = " + str(accelerations[0].stamp))
        # print("origin_accelerations[0].stamp = " + str(origin_accelerations[0].stamp))

        # thd:
        brake_thd_ = -0.5

        data = {}
        data['-1'] = 0
        has_wrong_brake = False

        # 获取planning egopose中的时间戳和加速度信息
        egopose_acc = []          # 保存planning轨迹中相对时刻及减速度
        egopose_acc_brake = []    # 保存planning轨迹中刹车时的相对时刻及减速度
        plan_stamp_start = None
        false_brake = False
        false_brake_info = []
        obj_info_reload = []
        obj_stamp = None
        
        for p in accelerations:
            if plan_stamp_start == None:
                plan_stamp_start = p.stamp
            egopose_acc.append([p.stamp - plan_stamp_start, p.value])
            # 保存planning轨迹中刹车时的相对时刻及减速度:
            if p.value < brake_thd_:
                egopose_acc_brake.append([p.stamp - plan_stamp_start, p.value])
        
        obs_cipv = []
        cipv_stamp0 = None
        for p in lon_cipv:
            if cipv_stamp0 == None:
                cipv_stamp0 = p.stamp
            obs_cipv.append([p.stamp - cipv_stamp0, p.value])
            # print("p.stamp - cipv_stamp0 = ", p.stamp - cipv_stamp0, "  cipv = ", p.value)

        for info in obj_info_mem:
            if obj_stamp == None:
                obj_stamp = info.stamp
            obj_info_reload.append([info.stamp - obj_stamp, info.obj_infos])
        for brake in egopose_acc_brake:
            CIPV_id = 0
            conf_cnt = 0
            # find brake target for each brake
            for cipv in obs_cipv:
                if cipv[1] > 0 and abs(cipv[0] + 0.4 - brake[0]) < 0.2:
                    CIPV_id = cipv[1]
                    break
            # if CIPV_id == 0:
            #     break
            # search in the later 2s timerange to check false brake
            for info in obj_info_reload:
                if info[0] >= brake[0] - 2 and info[0] <= brake[0] + 2:
                    for obj in info[1]:
                        if obj.track_id == CIPV_id:
                            if obj.collision != 0 and abs(obj.collision_time) < 0.05:
                                conf_cnt += 1
                            else :
                                conf_cnt = 0
                            break
                    else:
                        conf_cnt = 0
                # true brake
                if conf_cnt >= 3:
                    break

                if info[0] > brake[0] + 2:
                    break
            # if (brake[0] > 1 and brake[0] < 5) :
            #     print('CIPV_ID:',CIPV_id)
            #     print('time:',brake[0])
            #     print("conf_cnt:",conf_cnt)
            if conf_cnt < 3:
                false_brake = True
                false_brake_info.append([brake[0], brake[1], CIPV_id])

        if false_brake:
            has_wrong_brake = True

        
        no_need_wrong_brake_time_log = {}
        plan_point_brake_log = {}
        strong_brake_time_log = {}
        pre_brake_time_log = {}
        for ii in range(len(false_brake_info) - 1):
            if str(round(false_brake_info[ii][0])) not in no_need_wrong_brake_time_log.keys():
                no_need_wrong_brake_time_log[str(round(false_brake_info[ii][0]))] = \
                    [round(false_brake_info[ii][0], 2),round(false_brake_info[ii][1], 3)]
        
        self.single_checker_result.data["truck_num"] = 1
        for ii in no_need_wrong_brake_time_log.keys():
            self.single_checker_result.data["no_need_wrong_brake_time_" + str(ii)] =  str(no_need_wrong_brake_time_log[ii][0]) + \
                                                                                      "s:" + \
                                                                                      str(no_need_wrong_brake_time_log[ii][1]) + \
                                                                                      "m/s^2"
            print("false_brake: ",self.single_checker_result.data["no_need_wrong_brake_time_" + str(ii)])

        # cal truck num:
        obs_truck = []
        for p in lon_obs:
            if len(p.truck_ids):
                for truck_id in p.truck_ids:
                    if truck_id not in obs_truck:
                        obs_truck.append(truck_id)
            else:
                continue
        bag_truck_num = len(obs_truck)
        print("obs_truck = ", obs_truck)

        truck_dicts = []
        for obj in false_brake_info:
            if obj[2] in obs_truck and obj[2] not in truck_dicts:
                truck_dicts.append(obj[2])

        print("truck_dicts = ", truck_dicts)
        # print("false_brake_info: ", false_brake_info)
        if len(truck_dicts) == 0:
            has_wrong_brake = False

        # log { wrong_brake_timerange : wrong_brake_truck_id }, eg [start_time ~ end_time, id]
        last_wrong_brake_time_start, last_wrong_brake_time_end, last_wrong_brake_id = -1, -1, -1
        wrong_brake_timerange_id_info = []
        egopose_acc_brake_interval = 0.2      # avoid   {1.0 ~ 2.0, 1} {2.5 ~ 3.0, 1}  -> {1.0 ~ 3.0, 1}
        for obj in false_brake_info:
            if obj[2] in obs_truck and obj[2] == last_wrong_brake_id \
                and (obj[0] - last_wrong_brake_time_end) < egopose_acc_brake_interval:
                last_wrong_brake_time_end = obj[0]
            elif obj[2] in obs_truck and obj[2] == last_wrong_brake_id:
                wrong_brake_timerange_id_info.append([last_wrong_brake_time_start, last_wrong_brake_time_end, last_wrong_brake_id])
                last_wrong_brake_time_start = obj[0]
                last_wrong_brake_time_end = obj[0]
            elif obj[2] in obs_truck:
                if last_wrong_brake_id != -1 :
                    wrong_brake_timerange_id_info.append([last_wrong_brake_time_start, last_wrong_brake_time_end, last_wrong_brake_id])
                last_wrong_brake_id = obj[2]
                last_wrong_brake_time_start = obj[0]
                last_wrong_brake_time_end = obj[0]
        if last_wrong_brake_id != -1: 
            wrong_brake_timerange_id_info.append([last_wrong_brake_time_start, last_wrong_brake_time_end, last_wrong_brake_id])
        #print("wrong_brake_timerange_id_info = ", wrong_brake_timerange_id_info)

        self.single_checker_result.data["truck_num"] = bag_truck_num
        self.single_checker_result.data["truck_id"] = str(obs_truck)
        self.single_checker_result.data["wrong_brake_num"] = len(truck_dicts)
        self.single_checker_result.data["wrong_brake_truck_id"] = str(truck_dicts)
        self.single_checker_result.data["false_brake"] = false_brake
        self.single_checker_result.data["has_wrong_brake"] = has_wrong_brake

        for timerange_id in wrong_brake_timerange_id_info:
            tag = "wrong_brake_timerange_id_" + str(round(timerange_id[0], 2)) + "s ~ " + str(round(timerange_id[1], 2)) + "s"
            self.single_checker_result.data[tag] = timerange_id[2]
            print("timerange_id: %s : %d" % (tag, self.single_checker_result.data[tag]))   

        self.single_checker_result.name = "WrongBrakeCPIChecker"
        self.single_checker_result.result_code = ResultType.SUCCESS if self.single_checker_result.data["has_wrong_brake"] == False else ResultType.ALGORITHM_ERROR

        # 用于定义checker是否成功和错误类型
        # self.single_checker_result.result_code = ResultType.ALGORITHM_ERROR

    # checker需要用到的数据，用于判断BagLoader需要解析的数据和数据是否存在的检查
    def get_required_data(self) -> List[str]:
        return ["egopose", "origin_egopose", "origin_accelerations", "accelerations", "lon_cipv", "lon_obs", "obj_info_mem"]


