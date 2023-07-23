import math
from typing import List, Set, Tuple, Dict

from msim_bag_checker.bag_loader.bag_loader import BagLoader
from msim_bag_checker.bag_loader.defs import Vec3, VehicleAttributes
from msim_defs import ResultType, ErrorCode
from msim_bag_checker.params import (
    HIT_LOWER_SPEED,
    HIT_NAVI_ACTION_ACTIVE_ANGLE,
    HIT_NAVI_ACTION_DURATION,
    IGNORE_DISTANCE,
    IGNORE_POINT_LIST,
)
from msim_bag_checker.tasks.base import SingleCheckerBase
from msim_bag_checker.tasks.common_funcs import (  # get_point_label,
    StampIterator,
    cross_product,
    get_distance,
    get_speed,
    trans_position_with_radians,
    transform_bag_data,
    transform_origin_bag_data,
    transform_sim_bag_data,
)
from msim_bag_checker.tasks.cost_task_utils.cost_calculate import getCrashIds

import bisect

# 检测碰撞且过滤他车后方碰撞自车情况
# print("HIT_LOWER_SPEED = " + str(HIT_LOWER_SPEED))
# print("HIT_NAVI_ACTION_ACTIVE_ANGLE = " + str(HIT_NAVI_ACTION_ACTIVE_ANGLE))
# print("HIT_NAVI_ACTION_DURATION = " + str(HIT_NAVI_ACTION_DURATION))

def get_crash_ids(bag_loader: BagLoader) -> Tuple[bool, Set[int]]:
    data = bag_loader.data
    if (
        len(data.plan_path) == 0
        or len(data.fusions_obs) == 0
        or len(data.steer_reports) == 0
        or len(data.egopose) == 0
    ):
        return False, set()
    bag_data = transform_bag_data(bag_loader)
    crash_ids = getCrashIds(*bag_data[:9])
    return True, crash_ids

# 每一帧中判断是否有碰撞
def get_hit_in_frame(
    navi_frame: VehicleAttributes, fusion_frame: VehicleAttributes, navi_is_left: bool
) -> bool:
    navi_points = navi_frame.corners       # 自车polygon
    fusion_points = fusion_frame.corners   # 他车polygon
    hit = any([is_point_in_square(point, navi_points) for point in fusion_points])
    hit = hit or any([is_point_in_square(point, fusion_points) for point in navi_points])
    is_rear, is_left, _ = get_direction(navi_frame.front, fusion_frame.front, navi_frame.theta)
    # print("hit = " + str(hit) + "   is_rear = " + str(is_rear))
    # return hit and ((not is_rear) or (is_rear and is_left == navi_is_left)), is_rear
    return hit, is_rear



def get_vector(point_a: Vec3, point_b: Vec3) -> List[float]:
    return [point_b[0] - point_a[0], point_b[1] - point_a[1]]


# 判断一个点是否在一个矩形框内，通过此方式判断两个polygon是否有碰撞
def is_point_in_square(point: Vec3, corners: List[Vec3]) -> bool:
    sign = []
    for i in range(4):
        sign.append(
            cross_product(
                get_vector(corners[i], point),
                get_vector(corners[i], corners[(i + 1) % 4]),
            )
        )
    return sign[0] * sign[2] > 0 and sign[1] * sign[3] > 0


def get_direction(
    point_navi: Vec3, point_fusion: Vec3, navi_theta: float
) -> Tuple[bool, bool, float]:
    vec = get_vector(point_navi, point_fusion)
    x, y = trans_position_with_radians(vec[0], vec[1], -navi_theta)
    theta_degrees = math.degrees(math.atan2(y, x)) if get_distance([0, 0], [x, y]) > 0.1 else 0.0
    return x < 0, y < 0, theta_degrees


def delta_stamp(obj_a: VehicleAttributes, obj_b: VehicleAttributes) -> float:
    return math.fabs(obj_a.stamp - obj_b.stamp)


class CollideCPD(SingleCheckerBase):
    @staticmethod
    def get_hit_between_tracks(
        navi_track: List[VehicleAttributes], fusion_track: List[VehicleAttributes]
    ) -> Tuple[bool, float, float]:
        def get_navi_action(current_frame: VehicleAttributes, start_index: int) -> Tuple[bool, int]:
            while current_frame.stamp - navi_track[start_index].stamp > HIT_NAVI_ACTION_DURATION:
                start_index += 1
            start_point = navi_track[start_index].front
            theta = navi_track[start_index].theta
            current_point = current_frame.front
            _, is_left, direction = get_direction(start_point, current_point, theta)
            if abs(direction) < HIT_NAVI_ACTION_ACTIVE_ANGLE:
                is_left = None  # type: ignore
            return is_left, start_index

        first_hit_is_rear = False
        last_hit = False
        hit_start = fusion_track[0].stamp - navi_track[0].stamp
        hit_end = fusion_track[-1].stamp - navi_track[0].stamp
        last_navi_start_index = 0
        for pairs in iter(StampIterator(fusion_track, navi_track)):
            fusion_frame = fusion_track[pairs[0]]
            navi_frame = navi_track[pairs[1]]
            if get_speed(navi_frame.velocity.x, navi_frame.velocity.y) < HIT_LOWER_SPEED:
                continue
            if delta_stamp(navi_frame, fusion_frame) < 0.05:
                is_left, last_navi_start_index = get_navi_action(navi_frame, last_navi_start_index)
                hit , is_rear = get_hit_in_frame(navi_frame, fusion_frame, is_left)
                # print("last_hit = " + str(first_hit_is_rear) + "  hit = " + str(hit) + "  is_reat = " + str(is_rear))
                if not last_hit and hit:
                    hit_start = navi_frame.stamp - navi_track[0].stamp
                    # print("first_hit_is_rear = " + str(first_hit_is_rear) + " hit = " + str(hit) + " is_rear = " + str(is_rear))
                    if first_hit_is_rear == False and hit and is_rear:
                        first_hit_is_rear = True
                elif last_hit and not hit:
                    hit_end = navi_frame.stamp - navi_track[0].stamp
                    return True, hit_start, hit_end, is_rear, first_hit_is_rear
                last_hit = hit
        if last_hit:
            return True, hit_start, navi_track[-1].stamp - navi_track[0].stamp, is_rear, first_hit_is_rear
        return False, hit_start, hit_end, is_rear, first_hit_is_rear
    
    @staticmethod
    def check_equivalent_deceleration(obs_frame : VehicleAttributes, ego_frame : VehicleAttributes
        ) -> Tuple[bool, float]:
        eq_decel_thd = -5
        ego_speed = ego_frame.speed
        ego_pos = ego_frame.center
        obs_pos = obs_frame.center

        car_length = max(obs_frame.size.as_list())
        car_length = max(car_length, max(ego_frame.size.as_list()))

        # suppose obs's speed keep constant
        dist = get_distance(ego_pos, obs_pos)
        # suppose driver's reaction time = 0.5 s
        reaction_time = 0.5
        eq_decel = ego_speed ** 2 / (2 * (dist - ego_speed * reaction_time - car_length))
        
        flag = False
        if eq_decel < 0 or eq_decel > abs(eq_decel_thd):    # eq_decel < 0 : collision risk is very high,
            flag = True
        if eq_decel < 0:
            eq_decel = -100
        else:
            eq_decel *= -1
        return flag, eq_decel
    
    @staticmethod
    def check_time_headway(obs_frame : VehicleAttributes, ego_frame : VehicleAttributes
        ) -> Tuple[bool, float]:
        thw_thd = 0.5
        ego_speed = ego_frame.speed
        ego_pos = ego_frame.center
        obs_pos = obs_frame.center

        if ego_speed < HIT_LOWER_SPEED:
            return False, 100    #
        thw = get_distance(ego_pos, obs_pos) / ego_speed

        flag = False
        if thw < thw_thd:
            flag = True
        return flag, thw

    def process(self, bag_data: BagLoader) -> None:
        # navi_fusion = bag_data.data.egopose                     # ego 
        navi_fusion = bag_data.data.inflation_egopose             # ego inflation polygon
        fusion_tracks = bag_data.data.perception_fusion_object    # objects

        # checker : to find out 3 scenarios

        # 1. collision detection, excluding obstacle crashing into ego
        filtered_ids = []
        rear_hit_cars = []
        hit_num = 0
        for track_id in fusion_tracks:
            first_hit = True
            # print("track_id = " + str(track_id) + "  fusion_frame_num = "+ str(len(fusion_tracks[track_id])))
            if len(fusion_tracks[track_id]) < 20:
                continue
            hit, hit_start, hit_end , is_rear , first_hit_is_rear = self.get_hit_between_tracks(
                navi_fusion, fusion_tracks[track_id]
            )
            # if hit: 
            #     print("hit_start)
            if first_hit_is_rear:
                rear_hit_cars.append(track_id)
            # print("track_id = " + str(track_id) + "  hit = " + str(hit))
            if hit:
                hit_num = hit_num + 1
                self.single_checker_result.msg += "id:{} [{}s,{}s]  ".format(
                    track_id, round(hit_start, 3), round(hit_end, 3)
                )
            # if hit and not (succ and track_id not in crash_ids):
            # if hit:
            if hit and track_id not in rear_hit_cars:
                print("track_id = " + str(track_id))
                filtered_ids.append(track_id)
        # self.single_checker_result.msg += "id:{} ".format(000)
        print("rear_hit_cars = " + str(rear_hit_cars))


        # 2. equivalent deceleration for avoiding collision exceed pnc capacity (eg. a_req < -5 m/s^2)
            # abbr : eq_decel
        # 3. time headway < 0.5 s, eg 72 km/h -> 10 m
            # abbr : thw
        egopose = bag_data.data.egopose
        lon_cipv = bag_data.data.lon_cipv
        lon_obs = bag_data.data.lon_obs
        obj_info_mem = bag_data.data.obj_info_mem

        
        obs_cipv = []
        cipv_stamp = None
        for p in lon_cipv:
            if cipv_stamp == None:
                cipv_stamp = p.stamp
            obs_cipv.append([p.stamp - cipv_stamp, p.value])

        eq_decel_cnt, thw_cnt = 0, 0               # successive frames stastifing high collision risk
        eq_decel_frame_thd, thw_frame_thd = 3, 3  # 
        eq_decel_info = []
        thw_info = []
        index_obs_frame = 0
        for pairs in iter(StampIterator(obj_info_mem, egopose)):  # notice 
            fusion_frame = obj_info_mem[pairs[0]]
            ego_frame = egopose[pairs[1]]
            if get_speed(ego_frame.velocity.x, ego_frame.velocity.y) < HIT_LOWER_SPEED:
                continue
            if delta_stamp(ego_frame, fusion_frame) < 0.05:  
                flag_eq_del, flag_thw = False, False
                eq_del_val, thw_val = 0, 100
                eq_del_val_min, thw_val_min = 0, 100
                eq_del_id = []
                thw_id = []
                for obs in fusion_frame.obj_infos:
                    if obs.collision != 0 :
                        if obs.track_id not in fusion_tracks.keys():
                            print("error")
                            break
                        obs_stamp = fusion_frame.stamp
                        index_obs_frame = 0
                        obs_id = obs.track_id
                        if obs_stamp <= fusion_tracks[obs_id][0].stamp:
                            index_obs_frame = 0
                        elif obs_stamp >= fusion_tracks[obs_id][-1].stamp:
                            index_obs_frame = -1
                        else:                       #
                            while index_obs_frame < len(fusion_tracks[obs_id]) and \
                                obs_stamp > fusion_tracks[obs_id][index_obs_frame].stamp:
                                index_obs_frame +=1
                        obs_frame = fusion_tracks[obs_id][index_obs_frame]
                        
                        flag_eq_del_temp, eq_del_val_temp = self.check_equivalent_deceleration(obs_frame, ego_frame)
                        flag_eq_del |= flag_eq_del_temp
                        eq_del_val_min = min(eq_del_val_min, eq_del_val_temp)
                        if flag_eq_del_temp: 
                            eq_del_id.append([obs_id, eq_del_val_temp])

                        flag_thw_temp, thw_val_temp = self.check_time_headway(obs_frame, ego_frame)
                        flag_thw |= flag_thw_temp
                        thw_val_min = min(thw_val_min, thw_val_temp)
                        if flag_thw_temp:
                            thw_id.append([obs_id, thw_val_temp])
                
                time = ego_frame.stamp - egopose[0].stamp
                if flag_eq_del:
                    eq_decel_cnt += 1
                else:
                    eq_decel_cnt = 0
                if eq_decel_cnt >= eq_decel_frame_thd:
                    eq_decel_info.append([time, eq_del_val_min, eq_del_id])

                
                if flag_thw:
                    thw_cnt += 1
                else:
                    thw_cnt = 0
                if thw_cnt >= thw_frame_thd:
                    thw_info.append([time, thw_val_min, thw_id])
                
                if eq_decel_cnt or thw_cnt:
                    print("time %f eq_cnt %d  thw_cnt %d   eq_min %d   thw_min %d " % \
                        (time, eq_decel_cnt, thw_cnt, eq_del_val_min, thw_val_min))
        # TODO: merge timerange
        print(eq_decel_info)
        print(thw_info)



        





        # TODO: other indiction, eg. TTC



        if self.single_checker_result.msg:
            self.single_checker_result.msg += "ids after filter: {}".format(str(filtered_ids))
            if filtered_ids:
                self.single_checker_result.result_code = ResultType.ALGORITHM_ERROR
            self.single_checker_result.data["invalid_hit"] = (
                hit_num > 0 and self.single_checker_result.result_code is ResultType.SUCCESS
            )
        
        self.single_checker_result.data["event_num"] = 1
        self.single_checker_result.data["filtered_ids_num"] = len(filtered_ids)
        self.single_checker_result.data["disengagement_num"] = 1 if (len(filtered_ids) > 0) else 0
        self.single_checker_result.name = "CollideCPD"
        self.single_checker_result.result_code = ResultType.SUCCESS if self.single_checker_result.data["disengagement_num"] == 0 else ResultType.ALGORITHM_ERROR


    def get_required_data(self) -> List[str]:
        return [
            "egopose",
            "origin_egopose",
            "inflation_egopose",
            "steer_reports",
            "fusions_obs",
            "plan_path",
            "perception_fusion_object",
            "origin_accelerations",
            "accelerations",
            "json_lon_obs",
            "lon_cipv",
            "lon_obs",
            "obj_info_mem"
        ]
