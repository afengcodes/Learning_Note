
算法框架与流程
# 01 整体框架介绍
功能定义
目前PNC在整个自动驾驶系统中主要负责基于上游感知、地图和预测的输入实时规划出一条安全且符合导航任务的运动轨迹交由下游控制模块执行。
基本概念和框架
PNC内部按照功能和层级可以分为行为规划和运动规划，简单来说，行为规划决定自车是保持当前车道还是进行变道或者借道动作以及确定合适的障碍物横纵向决策，运动规划则基于具体的行为和决策产生合理的规划轨迹。
本质上PNC对不同驾驶场景采取不同的策略，而场景定义的要素主要包括路口（通行方向）/非路口、车道保持/变道/借道等不同维度，并组合成状态机中的不同子状态，通过不同状态的跳转和对应状态下细分的障碍物避让超车决策和轨迹规划，最终完成PNC模块的整体职能。
单纯的自顶向下的状态机很难维护，且很难规避上下游的不一致性。当前PNC框架中引入了仲裁的机制，通过针对当前状态可跳转的目标状态集合中的每个状态单独进行计算，并基于不同状态下的决策和规划结果进行最终的评价和状态跳转。这种方式实现了上游决策和下游规划的反馈机制，兼顾效率和完备性。
 
 
 
 
 
 
 
 

状态机介绍
状态机的状态由多级维度共同定义，最上层是路口和非路口的区分，路口内又同时区分直行、左转、右转和调头等通行方向。在此之下，进一步区分
车道保持、变道和借道，变道和借道又同时划分了多个阶段形成了最终的状态。
 
 
 
 
仲裁机制介绍
仲裁机制的主要目的是对一组状态集合每一状态对应场景计算的结果进行评价，并输出最终的跳转状态。根据地图任务的内容（必须执行的变道和非必须执行的变道），
以及当前所处的具体状态，可能会采用不同的仲裁逻辑。比如完全基于轨迹评价和打分择优的仲裁逻辑，比如基于优先级排序和可行性评估的仲裁逻辑，或者是基于
时序依赖关系依次执行和可行性评估的仲裁逻辑。

 

# 02 规划Pipeline概述
 
 
 
 


说明：
• 变道决策包括: 变道请求产生决策 (绕障场景识别)、变道选空决策(目标空及前后车id信息)、变道执行决策(变道等待、执行、返回仲裁), 详见04 横向决策说明文档
• 状态机及决策仲裁框架参考01 整体框架介绍
• 纵向决策流程及子模块功能说明详见05 纵向决策说明文档
• 纵向规划问题定义详见06 纵向规划说明文档
• 横向规划问题定义详见07 横向规划说明文档



# 03 框架使用规范
目前的算法框架是围绕Task，Scenario和Arbitrator等概念构建的，同时依靠PlanningContext和ScenarioContext来真正实现行为和数据的切割。
ScenarioContext的声明周期和实例化的场景一致，都是单帧；PlanningContext则作为单例其声明周期是和PNC线程一致，维护每一帧最终输出的完整决策规划结果。
• Task：确定场景下某一个具有明确功能边界和输入输出定义的模块，比如横/纵向决策/规划，ST构图等等
• Scenario: 目前是和状态机状态绑定的一个确定性场景，比如车道保持和变道执行等，场景内包含顺次执行的Task list
• Arbitrator：针对不同Scenario进行评价和仲裁，选择最优的Scenario输出，目前主要使用基于优先级和基于cost的仲裁逻辑
Arbitrator支持多层树状仲裁结构，但是最底层一定是单一确定细分场景的执行流程调用，为了实现接口的统一，SingleScenarioArbitrator作为单一场景仲裁类承载仲裁树的叶子节点的角色。
 
 
 
 
配置文件
Scenario和Task分属不同的级别，但都有一个相对明确的功能边界，在实际使用中也有参数配置的需求。目前的配置文件的书写方式也依照这种层级结构：

Plain%20Text
{
    "scenario_type"： "scenario_name",
    "task_types": [task1, task2, ...],
    "task_list": [task1, task2, ...],
    "task1_config": {...},
    "task2_config": {...},
    ...
}
并且在运行期由ConfigContext类统一读取。
如何新增Task
实际算法开发中如果需要开发新的算法功能模块，必须明确这个模块属于哪一个层级，以及是否受到上一层级的影响。目前
以POMDP planner算法为例
1. 首先需要在使用到该算法的场景配置文件中加入对应的task配置和更新相应的task_types（执行任务的类别集合）和task_lists（执行任务的pipeline，同一类任务可能执行多次）"lane_change_pomdp_planner"，并在ConfigContext中增加对应json字段读取
2. 在TaskConfig中增加对应的配置信息结构体定义LaneChangePOMDPPlannerConfig，并增加optional 私有成员，更新TaskTypeNames
3. 在PlanningContext中增加对应算法模块的输出结构定义POMDPPlannerOutput，并在PlanningContext和ScenarioFacadeContext同时维护对应成员和接口函数
4. 在算法内部继承Task抽象接口，定义算法类LCMDPPlanner，实现接口函数init 和 execute等接口函数 （也可以继承decider，实现init process等接口函数），输入和输出通过ScenarioFacadeContext实现上下游数据传递，并在TaskFactory中进行注册
如何新增Scenario
Scenario目前主要基于已经实现的ScenarioFacade类进行运行期task list的顺次执行。当然也可以支持LoopScenariofacade中的支持单次分叉的执行流。所以Scenario新增的工作主要集中在task list的定义。
以lane_follow场景为例
1. 定义好对应的lane_follow_scenario.json，并在ConfigContext中增加对应json文件读取
2. 在ScenarioFacadeConfig中更新ScenarioFacadeType和维护相应的optional成员
3. 在场景或者单一场景仲裁实例化处（目前一般都在状态的callback中）利用ScenarioFactory的ProduceScenarioWithSpecifiedType接口或者单一场景仲裁类的接口init_from_type输入type信息
注意：如果Scenario需要使用不同于ScenarioFacade中定义的顺次执行流程，比如需要使用LoopScenariofacade中的执行流，则需要在ScenarioFactory相应匹配注册时输入的实例化函数
注意事项
在Task的具体算法设计实现过程中，可以从ScenarioFacadeContext中获取当前场景获取之前已执行Task的结果，相应的将自身的输出更新到其中。同时也能够从PlanningContext中获取上一帧的决策规划结果做一些一致性或者计数逻辑。因此在需要使用这些不同意义不同来源的外部输入时需要明确自身的需求：
• 是否需要使用的是上一帧的计算结果，还是这一帧之前已经执行Task的计算结果，还是当前Task的计算结果，或者是整个Scenario的初始外部输入StateMachineOutput
• 如果是需要使用上一帧的计算结果，是否需要考虑状态的变化对使用的影响





# 04 横向决策说明文档
横向决策功能范围
• 控制状态机中各子状态间的跳转
• 状态机中的变道/借道请求和变道/借道选空
• 状态所对应场景计算中的障碍物横向避让决策
状态机跳转流程
 

 
 
 

变道/借道请求的产生【lateral_behavior_request_manager.cpp】
地图变道请求
地图变道请求生成模块主要负责结合地图模块根据全局路径规划输出的变道任务，综合考虑地图相关信息（车道虚实线、道路可行使方向、目标行驶方向、道路POI、车道属性等）、当前&目标车道车流信息、变道舒适性、任务必须性，最终决定地图变道请求是否产生。
主动变道/借道避让请求
主动变道/借道避让请求生成模块主要负责识别前方可能阻碍自车正常通行的低速/静态/逆行障碍物【lateral_behavior_object_selector.cpp】或识别前方特殊场景（事故、施工、占道、紧急cutin等）【lateral_behavior_state_machine.cpp】，综合考虑地图相关信息（地图任务、车道虚实线、道路可行使方向、目标行驶方向、道路POI、车道属性、路口距离&长度、限速、车道&道路宽度等）、障碍物位置&速度&预测&历史信息、红绿灯信息、自车当前位置&速度&规划&状态&可见范围信息、当前&目标车道车流信息（如：热力图）、变道舒适性、场景倾向性（如：highway倾向主动避让Y形点/merge点），最终决定主动变道/借道避让请求的产生/取消。
主动变道/借道避让障碍物初筛模块 【lateral_behavior_object_selector.cpp】 
算法功能
根据实时车流情况、地图、红绿灯、前方障碍物等信息决定是否向某一侧车道进行主动变道/借道避让。
主要思路
实时比较本车道可通行速度与邻车道可通行速度，所邻车道可通行速度显著大于本车道且持续超过一段时间，再综合考虑地图、红绿灯等信息，最终决定是否产生向该方向的主动变道/借道避让请求。
具体算法
• 左车道车流速度v_l=min(v_lf1,v_lf2 ) ,可通行速度v_lp=min(v_l,v_Cruise )
• 向左变道激活条件 
v_lp>v_(fi )+与前车速度负相关的值
• 计算从左侧超越当前车道前方障碍物需行驶距离d_lb_car_l=d_rel+obj_length+d_safety
• 计算从左侧达到d_lb_car_l所需时间t_surpass_l
• 判定经过t_surpass_l后leadone和leadtwo的间距diff_dn_car能否达到期望的间距desired_gap，同时需判定虚线剩余距离是否足够自车变到最终的车道。（间距的考虑主要针对目标车道存在反向地图任务且与目标方向非同向的情况，同时结合虚线剩余距离反应产生主动变道/借道避让的倾向，虚线剩余越短越鼓励借道。若目标车道为同向，则虚线剩余距离的考量仅为是否足够舒适变道。）
• 向左变道失效条件 
v_lp<v_f1+与前车速度负相关的值 or v_f1>与（目标车速 - v_f1）正相关的值
or 障碍物驶离当前车道 or diff_dn_car不满足需求
• 变道激活条件满足一定计数后发起主动变道请求，变道失效条件满足一定计数后取消主动变道请求，计数值与相对前车的相对速度有关。
 

细分场景识别
拥堵排队、两侧车道车流
变道选空
变道选空模块负责自车在开始进行变道动作时决定插入或者挤进目标车道哪两辆车之间的空，是一个相对独立的功能模块，输入包含
变道虚实线和地图任务相关信息，以及周围障碍物的感知预测信息，输出是空前后车的id以及其他辅助信息（比如是否到达空附近，选空异常或者保护状态，以及是否需要压实线等）
变道选空模块是变道执行决策的一个必须环节，只有当选取了合理的空且自车到达空附近（与空的前车后车都有一定的安全距离）时才可能做出变道执行决策。在未到达空附近之前，
选空信息会影响自车在变道准备阶段的速度规划，指引自车运动到空附近。
选空模块算法的主体流程是：地图信息和障碍物信息的获取和预处理（变道截止距离、实线区是否存在故障车等等）；目标车道障碍物的筛选；逐个分析空的可行性和最优性并选择最优空输出。
评价空的最优性主要包含以下几个方面的因素：
• 空本身的空间属性（主要和空的长度以及前后车的速度相关）
• 空本身的通行效率（主要和空前车的位置和速度相关）
• 到达空附近所需的时间（主要基于自车位置和空前车或者后车的位置和速度相关）
 
 
 
 
选空信息的作用：
• 在变道准备阶段纵向规划会参考选空信息调整期望速度，保证自车尽可能运动到空附近
• 在变道执行决策中，只有当自车到空附近，且和空的前后车留有一定的安全距离才能切换到变道执行状态。安全距离主要是基于跟车TTC计算得来，但是会根据空的大小做一定的修正
更新需车道内避让车信息 【update_avoid_cars（）】 
 

 
 
 
主要思路
在避让一侧车辆时，要同时考虑另一侧车辆对自车可避让上限值的限制
避让车信息更新逻辑
1. 若前视检测到两个需避让车，且两车满足下图状态，则根据前视检测结 果更新两车信息。（黄车为自车）
 

 
 
 
 

 
 
 
 

 
 
 
 

 
 
 
2.
• 若前视检测到一个需避让车，则通过比较当前需避让车id与上一时刻需 避让车id的关系和判断有无需避让的leadone，更新需避让车数量和信息。 （单一需避让车。）
 

 若前视检测到一个需避让车，则通过比较当前需避让车id与上一时刻需 避让车id的关系和判断有无需避让的leadone，更新需避让车数量和信息。 （需避让leadone的出现，对需避让车数量和信息的影响。） 
 

• 若前视检测到一个需避让车，则通过比较当前需避让车id与上一时刻需 避让车id的关系和判断有无需避让的leadone，更新需避让车数量和信息。 （当自车处于避让第一辆车的状态时，第二辆需避让车的id发生变化）
 

 若前视检测到一个需避让车，则通过比较当前需避让车id与上一时刻需 避让车id的关系和判断有无需避让的leadone，更新需避让车数量和信息。 （当自车避让的两车都处于环视范围内，不能急于释放第二辆需避让车。）
 

 若前视检测到一个需避让车，则通过比较当前需避让车id与上一时刻需 避让车id的关系和判断有无需避让的leadone，更新需避让车数量和信息。 （当自车处于避让第一辆车的状态时，第二辆需避让车的id未发生变化）
 

 若前视检测到一个需避让车，则通过比较当前需避让车id与上一时刻需 避让车id的关系和判断有无需避让的leadone，更新需避让车数量和信息。 （当自车处于避让第一辆车的状态时，突然出现第二辆需避让车。）
 

 若前视检测到一个需避让车，则通过比较当前需避让车id与上一时刻需避让车 id的关系和判断有无需避让的leadone，更新需避让车数量和信息。（对leadone的 释放）
 

 3. 若前视未检测到需避让车，且自车需避让车数量不为0，则通过判断当前 需避让的是否为leadone，更新需避让车的信息。
 
 

 4.
 
 
• 若出现前视检测到两辆需避让车，但为下图情况时(第一第二辆需避让车从黄橙变成蓝黄)：
 

 若出现前视检测到一辆需避让车，但为下图情况时：
 

 若出现前视检测到零辆需避让车，但为下图情况时：
 

 

 若满足上述条件，但避让时间未达到t_avdCar,且自车未完全超过该需避让 车时： 1.若该车上一时刻处于环视避让状态，则用该侧鱼眼不断更新该车的 信息 2.否则，则根据前视失去对其跟踪的最后一刻的相对速度更新（预测） 纵向相对距离 3.如果该避让车处于前视可检测范围，但连续2.5s未达到需避让条件/ 连续1s未达到需避让条件且纵向相对速度大于1.5m/s时，释放该已经离开的 需避让车。
 
 
 若满足上述条件，且避让时间达到t_avdCar,或自车完全超过该需避让车时： 自车会开始使用前视和侧视观察正在避让车一侧是否还有需避让车， 若有，则更新其信息，若无，则更新需避让状态。 当自车正在避让两辆环视范围内的车，若第一辆需避让车消失在前视 和侧视有效需避让范围，需继续check另一侧鱼眼和侧视信息。
• 若上述条件均不满足，当出现前视检测到两辆需避让车，但为下图情况时： （若出现两个全新的避让车id，包括从无避让车到突然出现两辆需避让车）
 

 若上述条件均不满足，当出现前视检测到一辆需避让车，但为如下情况时： 1.需避让车从0到1. 2.第二辆需避让车id变化 3.第一辆需避让车在前视范围却持续不符合需避让条件。用当前检测结果更新。
• 若以上条件均不满足，当出现前视检测到零辆需避让车，但为下图情况时： （对满足避让条件的leadone进行计数，以决定其是否最终能判定为需避让车。）
 

 
 
 
参考线期望偏移值初步计算模块【update_path_planner（）】
主要思路 （update_avoidance_path（））
在不对他车产生影响的前提下，计算出避让目标障碍物的参考线偏移值
单车避让轨迹
一、避让距离计算
1. 基于横向相对距离和纵向相对速度、距离计算一个目标避让距离。（黄车代表自车）
_ = 0.5 ∗ ((ℎ − 0.5 ∗ ℎ) − ℎ) + ∗ _(_，d_) (:补偿系数，lat_compen:与相对速度和相对距离相关的横向偏移补偿值，width对应图中LW(lane_width)，carwidth对应图中CW)
 
 

 
 
 
二、计算允许横向避让的最大限值
2. 在避让过程中，自车会时刻检测避让方向侧前方和侧方最近车辆的横向距离，根据该距离计算最大 横向避让的一个限值，同时限制避让不得超过自车道；若距离路口较近时出现道路分流/合流，限制 对应方向横向允许避让限值；若距离左/右车道线截距小于1.4m,限制参考线与左/右车道线截距为 1.4m；若自车处于最右侧车道且不需借非机动车道避让，会限制避让值为计算值的0.8倍；
 

 
 
 
三、规划平顺、舒适的避让轨迹
3. 自车从避让状态逐渐恢复到车道保持状态的过程中，若发现了另一辆需避让车，将会规划一条平滑 的、变化幅度最小的避让轨迹，以平滑衔接连续避让动作。
 

 
 
 
四、避让完成判断
4. 计算以当前的纵向相对速度和相对距离，自车处于避让状态行驶到该需避让车前方一个安全距离处 的时间t_avdCar。
5. 当行驶时间达到t_avdCar且纵向相对距离小于某一阈值时，判定为避让完成。
 

 
 
 
多车避让轨迹（两车在异侧）
1. 计算t_avdCar。
2. 计算经过t_avdCar后，两需避让车的纵向相对距离diff_dnCar。
_ = (_ + _ℎ + _) / _
_ = _ + _ ∗ _ (dist_bTwo:两障碍物的纵向相对距离，vRel_bTwo:两障碍物的纵向相对速度，Car_Length对应图中CL)
 

 
 
 
• 若diff_dnCar小于一个理想的相对距离，且两车的横向相对距离大于可避让阈值，则基础 避让轨迹采用两车的中线，根据计算得到的两车各自的避让补偿值，计算最终的避让轨迹。
 

 
 
 
• 若diff_dnCar大于理想的相对距离，则按单车避让逻辑动作。
 

 若以上条件均不满足，则采取跟车模式。
 
多车避让轨迹（两车在同侧）
1. 计算t_avdCar, 计算经过t_avdCar后，两需避让车的纵向相对距离diff_dnCar。
2. 若diff_dnCar小于一个理想的相对距离，则最大避让值根据横向距离最近车的信息计算； 否则按照单车避让逻辑动作。
 

 
 
 

障碍物横向避让决策 【lateral_behavior_avd_info.cpp】 
障碍物横向避让决策模块主要负责输出自车周围一定范围内障碍物的横向决策信息，包括：避让方向，避让优先级，可横向避让时间区间，纵向规划是否可忽略。
主要考虑信息：自车周围障碍物位置&速度&预测&历史信息，自车当前位置&速度&状态信息，地图信息（车道虚实线、车道&道路宽度、路口非路口信息、车道类型等），上游请求生成&初筛模块输出的请求及障碍物信息。

障碍物横纵向规划分配【update_obstacle_time_interval 】
功能：因为先进行纵向规划，所以横向可绕行的障碍物不能给到纵向避让。该模块通过提取可通行区间，判断障碍物是否block前行道路。若block，将avd_info_中ignore置为false，给到纵向考虑。反之，就可以纵向忽略。
执行流程：
1、根据地图信息，判断是否在出匝道
 2 、判断借道方向与借道宽度
 3、实例化ExtractPathBound，并调用相应的process函数
1）传入参数及含义
lane_borrow_direction，借道方向
 lane_borrow_width，借道路宽度
 avd_obstacles，须避让障碍物中优先级normal的障碍物
 avd_obstacle_prior_，须避让障碍物中优先级较高的障碍物
 block_type，block信息
2）update_delta_s
更新adc_s_list_和s_max_list_
 adc_s_list_的含义是自车从当前s位置出发匀速行驶6s，每隔0.2s构成的s序列
 s_max_list_的含义是，自车从当前s位置出发，以最大的加速度来行驶所可能到达的s位置序列
3）更新baseline对应的refline_info，主要包括曲率，到左右车道线距离，到左右道路边界的距离等
 4）GetSLPolygonSeq，调用上游接口来构建障碍物sl_polygon
 5）update_obstacle_priority，更新障碍物优先级，主要根据障碍物相对自车的位置关系来确定优先级
 6）GenerateRegularPathBound，提取时间区间的主函数
 7）InitPathBoundary，初始化道路边界，初始道路左右边界各10m
 8）GetBoundaryFromLanesAndADC，考虑地图边界来计算当前道路可同行边界
 9）check_motion_collide，从自车当前状态触发，横向以匀jerk来推演，如果超出path_bound约束，则时间区间设置为0
 10）GetBoundaryFromStaticObstacles，根据静态障碍物更新path_bound，如果block，则将对应障碍物时间区间设置为0，否则设置为8
 11）GetBoundaryListFromReverseObstacles，根据逆行车，提取时间区间
 12）GetBoundaryListFromObstacles，提取其他障碍物的时间区间
 13）GetSpeedLimit，根据障碍物和自车的位置关系与提取的空间大小，对纵向位置进行限速
 14）GetLatOffset，根据静态障碍物和对向车提取出的边界，提取横向大致的横向偏移位置
15）CheckStaticIgnore，判断是否危险，是否需要将前方置ignore为true的障碍物改为false
场景决策算法【order_disable_enable()】
根据lead_one的速度信息将自车当前车道前方区域缩小为区域cur_first，表示lead_one及之后与其速度接近的障碍物所占连续POI区域，该区域长度由diff_point表示，并用diff_exist变量表示当前车道在感知范围内是否存在速度可分的区间，设置diff_point+5m之外的区域为cur_avoid，设置lead_one所决定的perception_range至感知范围的POI为cur_pr，设置自车后方一定范围内POI为cur_back；左右两侧车道POI区间由left_total、right_total表示，设置虚线对应的POI范围由left_dash、right_dash表示，设置绕障需要考虑的POI范围（纵向距离diff_point+10m之内）由left_avoid、right_avoid表示；计算各区间内的车辆个数及平均车速，并通过区间长度计算出区间内车辆的平均间距。
 

 
 
 
非路口DISABLE绕障逻辑
对于左/右侧车道，若该车道为同向车道：
（diff_exist为false或cur_first区间内车辆个数大于等于3）且cur_first区间均速很小、平均间距很小（此时lead_one前方存在拥堵），
或cur_pr有车（大于等于3）且均速很小、平均间距很小（感知范围中存在拥堵），
则置cur_slow为true，表示前方存在拥堵：
 

 
 
 
同时，若left/right_total有车（数量大于1）且平均车速很小（表示该侧车道也处于拥堵状态）或left/right_dash有车（数量大于1）且平均车速很小（表示虚线范围内处于拥堵状态），则置disable_l/r为true：
 

 
 
 
若该侧车道存在拥堵（纵向距离前方三十米范围left_close内的车辆个数大于1辆且平均车速较低），且另一侧车道为非同向车道，则推断出当前方向（当前车道及同向车道）存在拥堵，此时disable向非同向车道的变道/借道：
 

 
 
 
若该车道为非同向车道：
（diff_exist为false或cur_first区间内车辆个数大于等于3）且cur_first区间均速很小、平均间距很小（此时lead_one前方存在拥堵），
或cur_pr有车（大于等于3）且均速很小、平均间距很小（感知范围中存在拥堵），
即cur_slow为true，则置disable_l/r为true。
再次结合历史帧信息判断是否触发disable。首先考虑前车历史车速，若前车为由高速刹停，则认为前方存在拥堵的可能性较高，将cur_slow与cur_slow置为true。
 

 
 
 
除此以外，在触发disable信号时记录下前车的id为disable_id_，
 

 
 
 
后续判断过程中只要front_tracks中有disable_id_，则继续保持disable信号。
 

 
 
 
为避免进入死循环，对由于历史帧信息造成的disable信号触发进行衰减，若利用单帧信息判断就可触发disable，则将duration设置为100，否则将逐次衰减置零并不再触发disable，保证disable信号受历史帧影响的时间不超过10s。
 
 

 
 
 

非路口ENABLE绕障逻辑
对于左/右侧车道，若该车道为同向车道：
 

 
 
 
首先需要判断前方低速行驶lead_one是否为障碍物，判断条件为：
perception_range足够小，且cur_first车辆数目小于等于2，且cur_avoid无车或高速；或lead_one为锥桶：
 

 
 
 
此时仅需保证该方向disable信号未触发且该侧车道目标区域left_avoid、right_avoid无车或车流速度不低，则触发enable：
 

 
 
 
对于左/右侧车道，若该车道为非同向车道或非机动车道：
 

 
 
 
首先需要判断前方低速行驶lead_one是否为障碍物，判断条件比同向车道更为严格：
perception_range足够小，且cur_first车辆数目小于等于1，且cur_avoid无车或高速；或lead_one为锥桶：
 

 
 
 
还需要保证该方向disable信号未触发且该侧车道目标区域left_avoid、right_avoid无车或车流速度不低，并确保故障车在路口停止线（或虚线结束点）一定范围之外，以保证借道避让后可以（在虚线范围内）返回当前车道；
 

 
 
 

对于借对向车道绕障，首先判断车道线属性，并确保对向车道纵向一定范围内（保证可以完成借道并返回当前车道）无对向来车，并需要考虑路口停止线限制。
若依据上述规则未能触发任意一侧的enable信号，此时需结合自车车道后侧POI车流信息（去除后侧保障车的影响）再次判断，若后侧车流存在朝某一方向的变道，则再次考虑向该侧车道的变道，此时将该侧的enable触发条件放宽并再次判断。
路口DISABLE绕障逻辑
disable逻辑与非路口基本一致，除left_dash、right_dash由纵向范围30m之内的left_close、right_close代替，且车辆数目的阈值调整为2。

路口ENABLE绕障逻辑
若当前路口不存在跨多车道障碍物、不需要触发跨多车道避让：
绕障逻辑与非路口类似，且无需考虑路口停止线距离限制。针对左转情况，若前方静止障碍物已出路口，则不考虑向左避让障碍物。
若当前路口存在跨多车道障碍物，则选取前方障碍物中最靠左/右侧的障碍物并记录id，
 

 
 
 
选取最左/右侧障碍物的左/右侧为POI区域left_avoid_zone/right_avoid_zone，若该区域存在快速车流，则enable该方向的跨多车道避让，并根据自车位置与目标通行区域位置的关系确定路口静止障碍物的避让方向。
 

 若依据上述方法确定的POI区域不存在快速车流通过，则判断该区域对应车道是否为同向车道且空闲，若满足条件，则触发对应方向的跨多车道避让。
 
 
 

 
 
 
允许变道条件判定算法&变道返回条件判定算法【check_lc_valid() & check_lc_back()】
算法功能
当有一侧变道请求时，根据障碍物信息判断是否可以开始执行变道
主要思路
基于规则和经验公式，判断现在的变道空间是否合适
具体算法
• 侧前方空间可变道条件 
假设前车匀速，自车舒适匀加/减速变道完成后仍能预留安全距离
d_f>(max⁡(d_safe - 〖v_rf〗^2 ∙ γ, α ∙ d_safe))
 v_rf：右前方障碍物相对速度，γ：与加速度相关的系数，α：与相对速度相关的系数
• 侧后方空间可变道条件 
 假设后车匀减速至与自车规划速度同速后仍能留出安全距离
 

 
 
 
 t_r：后车司机反应时间， v_rr：右后方障碍物相对速度，β：与减速度相关的系数
• 变道返回条件与变道条件形式类似但是会在参数上拉开差距，从而避免决策频繁在变道与返回之间切换
 

 
 
 
对地图信息的整合【map_info_manager.h】 

其他类别的横向决策
打灯、贴线等


# 05 纵向决策说明文档
y1. 功能概述：
• 纵向干涉目标初筛（st_graph_generator）
• 结合地图、横向决策、预测信息更新限速、限加速度信息（speed_boundary_decider）
• 确定每一时刻纵向yield/overtake障碍物决策及对应的纵向约束、期望、权重等参数（longitudinal_behavior_planner）

2. 输入：
• 地图信息：参考线、车道宽、虚实线、路口距离等
• 障碍物感知融合信息：障碍物type、size、初始pos、vel等
• 红绿灯决策信息：stop/pass
• 障碍物预测信息：未来5s/8s预测轨迹

3. 输出：
• 场景决策：{no leader, faster leader, slow leader, slow potential leader, close fast leader, close slow leader, close slow potential leader}→ 纵向规划cost：权重系数{W_s, W_v, W_a, W_jerk}
• 期望速度：vr[i] → 纵向规划cost：|| v -vr||²
• yield目标纵向位置：s_l[i]→ 纵向规划cost & constraint：ds_flag · || s -s_l+ ( v + v - v_l )· Δt + d_stop || ²，s + ( v - v_l ) · TR + d_safe≤s_l
• yield目标纵向速度：v_l[i]→纵向规划cost & constraint：ds_flag ·|| s -s_l+ ( v + v -v_l)· Δt + d_stop || ²，s + ( v -v_l) · TR + d_safe≤s_l
• yield目标静态避让距离：d_safe[i]→ 纵向规划constraint：s + ( v - v_l ) · TR +d_safe≤s_l
• overtake目标纵向位置：s_r[i]→ 纵向规划constraint：s_r+ ( v_r - v ) · TR + d_rear≤s
• overtake目标纵向速度：v_r[i]→ 纵向规划constraint：s_r + (v_r- v ) · TR + d_rear≤s
• 速度上限：v_max[i]→ 纵向规划constraint：v ≤v_max
• 加速度上限：a_max[i]→ 纵向规划constraint：a≤a_max
• jerk上限：jerk_max[i]→ 纵向规划constraint：jerk ≤jerk_max
• jerk下限：jerk_min[i]→ 纵向规划constraint：jerk ≥jerk_min
• 时延补偿：TR[i]→ 纵向规划constraint：s + ( v - v_l ) ·TR+ d_safe≤s_l
• 是否存在跟随目标：ds_flag[i]→ 纵向规划cost：ds_flag· || s -s_l+ ( v + v - v_l )· Δt + d_stop || ²

4. 重点子模块
• st_graph_generator 
￮ 主要功能：构建纵向决策运动推演所需的st图（即每一时刻干涉自车未来行驶路线的障碍物纵向距离信息）
￮ 子函数说明： 
▪ set_cross_lane_state：设置虚拟车道左右半宽度
▪ generate_shifted_reference_line：结合自车位置、参考线等信息生成polygon虚拟车道
▪ preliminary_screen_obstacles：障碍物初筛，剔除纵向可忽略的障碍物，对于剩余障碍物，计算其初始状态和避让距离等信息
▪ compute_sl_polygon_sequence：构建障碍物polygon角点frenet坐标
▪ compute_st_boundaries：基于虚拟车道和障碍物信息构建st图障碍物边界
▪ translate_scenario_decisions：更新特殊场景及状态下的障碍物决策
▪ compute_st_graph_with_decisions：结合障碍物决策更新纵向候选避让和候选超越目标信息
▪ extract_info：基于障碍物st图信息筛选出每一时刻纵向需考虑避让的最近两个目标信息
▪ process_lane_keep_sv：考虑停止线位置更新最近避让目标距离信息
▪ process_rear_sv：将纵向候选避让目标从纵向候选超越目标集合中剔除
▪ set_obstacle_tag：基于障碍物位置速度等信息更新障碍物与自车的相对运动关系tag
▪ set_prebrake_signal_from_dynamic：判断是否触发动态障碍物预制动
▪ set_prebrake_signal_from_static：判断是否触发静态障碍物预制动
• speed_boundary_decider 
￮ 主要功能：确定纵向决策规划限速（包括地图限速、预制动限速、绕障变道限速等）
￮ 子函数说明： 
▪ set_speed_limit：综合地图限速、方向盘转角限速、动静态障碍物筛选预制动限速、路口预制动限速、曲率限速、变道限速、cutin预制动限速、绕障限速、预测不确定性限速、横向决策限速、POI通行限速，确定纵向决策规划实际考虑的限速及限加速度信息
▪ calculate_acc_limit：根据障碍物相对距离及相对速度，确定自车加速度限制
￮ 预制动逻辑说明： 
▪ 仅对纵向Ignore==True的目标生效，因为这部分目标不会进入ST图
▪ prebrake_fast：对于目标纵向速度分量不是特别小（vel_lon > ego_vel + std::min(ego_acc, kAccSlipBrake) * kdtDelay）的目标，当下述条件均满足时触发，每触发一次，保持生效至少20帧（2s），对应的预制动减速度与溜车减速度相当（相当于只是松油处理），对规划的影响体现在速度期望修正： 
• 横向距离较近：std::fabs(dr_obs)<kdlThldRoadFast*gain_type
• 纵向ttc没有明显小于横向ttc：ttc_lon+8.0>ttc_lat
• 目标初始纵向距离没有特别远：ds_obs<ds_soft_brake+vel_lon*vel_lon/2.0/kAccHardBrake
• 目标纵向速度没有明显大于自车车速：vel_lon<ego_vel+kdvBuff
▪ prebrake_slow：对于目标纵向速度分量较小（vel_lon <= ego_vel + std::min(ego_acc, kAccSlipBrake) * kdtDelay）的目标，当下述条件均满足时触发，每触发一次，保持生效至少20帧（2s），对应的预制动减速度比溜车减速度略低些（相当于松油的同时轻带点刹车），对规划的影响体现在速度期望修正： 
• 横向距离较近：std::fabs(dr_obs) < dl_thld * gain_type * std::max(1.0, std::min(2.0, 0.2 * (ego_vel - vel_lon)))
• 目标初始纵向距离没有特别远：ds_obs < ds_soft_brake + vel_lon * vel_lon / 2.0 / kAccHardBrake
• 目标横纵向ttc差异没有特别大：ttc_lon > ttc_lat - ttc_lat_buff && ttc_lon < ttc_lat + kdTTCThld
• 自车是否能够舒适减速避让目标：a_brake < 0.0 && a_brake > kAccSoftBrake
▪ prebrake_merge：对于目标纵向速度分量较小（vel_lon <= ego_vel + std::min(ego_acc, kAccSlipBrake) * kdtDelay）的目标，当prebrake_slow条件未满足，但下述条件满足时，触发汇流预制动，预制动持续时长和减速段大小与prebrake_slow相同 
• 处于匝道汇流借道：merge_from_ramp
• 自车与目标在汇流点附近相遇：(dt_merge < ttc_lon + kdTTCMerge && ttc_lon < dt_merge + 6.0)
• 或自车与目标纵向距离较近且纵向速度相近：(std::fabs(ds_obs) < 10.0 && std::fabs(ttc_lon) > 10.0)
￮ 预加速逻辑说明： 
▪ 仅对纵向Ignore==True的目标生效，因为这部分目标不会进入ST图
▪ 当纵向Ignore目标未触发预制动，且下述条件满足时，触发预加速，每触发一次，保持生效至少10帧（1s），对应的预加速加速度未1.3m/s2，对规划的影响体现在速度期望修正： 
• 目标与自车纵向速度接近：ego_vel - vel_lon < kdvMaxPreacc && ego_vel - vel_lon > kdvMinPreacc
• 目标横向逐渐靠近：ttc_lat > 0.0 && ttc_lat < 10.0
• 目标纵向ttc小于横ttc：ttc_lon < ttc_lat - 1.0
• 自车加速能在目标横向侵入前超出一定纵向安全距离：ds_acc_tcc > 0
• 或自车纵向减速避让减速度过大：a_brake < kAccHardBrake
• longitudinal_behavior_planner 
￮ 主要功能：基于ST图下的自车与他车运动状态推演，确定纵向障碍物决策，并计算纵向规划所需的速度期望、跟车距离期望、纵向位置上下界约束、jerk上下界约束、多目标规划权重系数
￮ 子函数说明： 
▪ get_limit_from_map：根据路口通行方向设置运动推演acc及jerk参数、预测硬约束考虑时长、路权相关参数
▪ get_constrain_time：根据自车车速设置硬约束区间
▪ get_s_limit：基于分段匀jerk模型在st图下进行运动推演，确定每一时刻纵向yield/overtake的障碍物位置信息
▪ update_from_nearest_obj：根据纵向避让的最近障碍物信息计算对应速度期望，以及是否需要配合横向减速
▪ update_sr_dsafe：综合纵向障碍物决策、变道选空决策，生成纵向规划位置约束上下边界、jerk约束上下边界，并更新纵向规划速度期望
▪ smooth_vr_from_vset：根据速度期望生成纵向规划参考速度序列
▪ get_v_upper：设置速度上界约束
▪ set_acc_max：设置加速度上界约束
￮ 算法流程：
 
 
 
 
 

• 变量说明： 
￮ kDtVvr_：参考临界时距（判断前车是否较远）
￮ kDvLim_：速度期望差值参考阈值（判断当前车速与期望速度偏差是否较大）
￮ kTs_：规划时间步长
￮ kVBuff_：限速buffer
￮ kVslowdiff_：前车与自车速度差参考阈值（快速cutin识别）
￮ kAStop_：自车刹停参考减速度
￮ kDTStop_：自车刹停最小参考时间
￮ kDisBefInter_：到路口距离参考阈值
￮ kStopLineBuffer_：自车距停止线参考距离阈值
￮ kEta_：时延补偿参数
￮ kVSetBuff_：速度期望buffer
￮ kVlBuff_：速度差buffer
￮ kVlCross_：横穿目标纵向速度阈值
￮ kYawRelBuff：并排目标相对参考线yaw角阈值
￮ kYawTransverse：横穿目标相对参考线yaw角阈值
￮ kEgoHalfWidth：半车宽
￮ kNudgeLength_：横向配合避让目标纵向距离buffer
￮ kAccBackup：自车缓速刹停参考减速度
￮ kN_Nudge：纵向允许横向配合避让的时间区间Index
￮ kVminNudge_：纵向允许横向配合避让的最小车速限制
￮ kNudgeProbThreshold_：纵向允许横向配合避让的预测概率限制
￮ kdVLCPreFront_：空前车期望速度差buffer
￮ kdVLCPreRear_：空后车期望速度差buffer
￮ kPreProbThresholdBP_：cutin目标预测概率参考阈值
￮ kPreProbThresholdCons_：预测按硬约束考虑的概率阈值
￮ kCutInScoreThld_：预测cutin score参考阈值
￮ kCutInNudgeThld_：纵向允许横向配合避让的cutin score阈值
￮ kJerkCutin_：避免cutin目标减速jerk最小值
￮ kJerkVmax_：速度上限jerk平滑参数
￮ kNumThld_：期望速度计算时的距离buffer索引平滑参数
￮ kDtNudgeMin_：横向配合避让的最大时长
￮ kMaxSigma_：基于sigma信息的overtake距离修正阈值
￮ kTTC_：参考时距
￮ kdtPreAcc_：预加速持续时长
￮ kAccOvertake_：overtake参考加速度
￮ kAccSoftBrake_：非紧急减速度参考阈值
￮ kOvertakeAccDuration_：最大overtake时长index
￮ kOvertakeSmax_：近距离overtake目标距离阈值
￮ kOvertakeVmargin_：overtake速度期望增益
￮ kMinAcc_：最小参考加速度
￮ kdVlBuff_：低速参考值
￮ kVmax_：最大巡航速度
￮ tmin_pre：硬约束最小时长
￮ tmax_pre：硬约束最大时长
￮ tmax_ped：行人硬约束最大时长
￮ tmax_ofo：ofo硬约束最大时长
￮ tmin_ped：行人硬约束最小时长
￮ tmin_ofo：ofo硬约束最小时长
￮ j_cross：纵向决策推演参考jerk
￮ a_cross：纵向决策推演参考加速度
￮ dis_ped：行人预测考虑时长修正参考距离
￮ dis_ofo：ofo预测考虑时长修正参考距离
￮ eta_freeMove：freemove预测距离参考系数
￮ ds_road：overtake参考距离buffer
￮ tpre_buff：预测硬约束参考时长buffer
￮ power_base_dis：overtake距离指数修正基底参数
￮ dis_buf：overtake距离指数修正指数二次项系数
￮ ratio_dis：overtake距离指数修正指数一次项系数
￮ power_base_dt：overtake距离时间修正指数
￮ decay_dis：overtake距离修正参数
￮ decay_dt：overtake时间修正参数
￮ GSVelRotation：直行预测速度旋转角度阈值
￮ UTVelRotation：掉头预测速度旋转角度阈值
￮ decay_GS/decay_TR/decay_TL/decay_UT/decay_FM：直行/右转/左转/掉头/Freemove路权修正系数
￮ decay_IniPos：overtake初始距离修正系数
￮ decay_Cutin：overtake cutin socre修正系数
￮ decay_target_intention：overtake目标意图修正系数
￮ ds_IniPos：overtake初始位置修正距离buffer
￮ dis2Obj：干涉点到目标距离
￮ cost_weight_：规划权重
￮ dt：最近干涉点时间
￮ a0：i<num上界平滑参考加速度
￮ v0：i<num上界平滑初始速度
￮ vl_bound：最近目标低速判断参考阈值
￮ accident_back_：事故车变道返回标志
￮ lc_pause_id_：导致变道悬置目标id
￮ lc_overtake_id_：变道overtake目标id
￮ lc_rear_obs_dis_：变道目标空后车距离
￮ must_lc_：是否必须执行的变道
￮ lc_pause_：变道悬置标志
￮ find_pause_obs_：导致变道悬置目标是否被构建
￮ tr_pause_l_：导致变道悬置目标横向位置
￮ tr_pause_s_：导致变道悬置目标纵向位置
￮ findlead_：ST图修正后前方是否存在合理干涉目标
￮ lead_index_：合理干涉目标排序
￮ HasRoadRight_：自车是否占路权
￮ HasRelRoadRight_：自车相对干涉目标是否占路权
￮ isLaneChange_：自车是否处于变道状态
￮ AEBLatCond_：允许触发AEB的干涉时间条件
￮ AEBAccCond_：允许触发AEB的加速度条件
￮ NeedAEB_：是否需要触发AEB
￮ HasOvertakeObs_：是否存在overtake目标
￮ gap_behind_brake_：是否需要减速进入后方空
￮ vel_limit_：最大限速
￮ a_limit_：最大加速度限制
￮ s_limit_：限速点纵向位置
￮ acc_smooth_：限速平滑加速度
￮ enable_prebrake_：是否触发预制动
￮ enable_preacc_：是否触发预加速
￮ prebrake_acc_：预制动加速度
￮ prebrake_duration_：预制动持续时间
￮ preacc_duration_：预加速持续时间
￮ N_hard：硬约束区间上限
￮ N_SL：最近目标筛选区间上限
￮ N_hard_type：不同类型目标硬约束区间上限
￮ yield_cnt：目标纵向避让累计帧数
￮ decay_vr：速度期望cost权重衰减系数
￮ decay_acc：加速度cost权重衰减系数
￮ decay_jerk：jerk cost权重衰减系数
￮ decay_ds：跟车cost权重衰减系数
￮ vr：参考速度序列
￮ TR：时间延迟序列
￮ d_safe：前车距离约束序列
￮ d_rear：后车距离约束序列
￮ d_stop：期望跟停距离序列
￮ Merge_Dis_：汇流点距离
￮ TTC_max：纵向最大TTC
￮ ds_flag：跟车cost开关系数
￮ s_l：纵向避让目标位置序列
￮ v_l：纵向避让目标速度序列
￮ s_r：纵向overtake目标位置序列
￮ v_r：纵向overtake目标速度序列
￮ a_max：最大加速度序列
￮ d_merge：纵向距离约束
￮ a_yield：纵向避让加速度序列
￮ a_yield_min：纵向避让最大减速度
￮ vl_yield_min：纵向最大减速度避让目标速度
￮ ttc_lead_min：纵向避让目标最ttc
￮ ds_lon_min：有横向重叠的纵向最近目标距离
￮ ds_buffer：纵向距离约束初始值
￮ s_yield_backup：有横向重叠的纵向避让目标位置序列
￮ s_max：s上界序列
￮ sl_max_hard：有横向重叠的纵向最近目标位置序列
￮ vl_max_hard：有横向重叠的纵向最近目标速度序列
￮ s_min：s下界序列
￮ v_min：overtake目标速度序列
￮ v_upper：速度上界序列
￮ cutin_score：目标预测cutin score
￮ TTC_Lon：期望跟车时距序列
￮ ar：期望速度序列平滑加速度
￮ prob_smax：纵向避让目标预测概率
￮ yaw_frenet：目标相对参考线夹角
￮ s_start：目标车尾初始位置序列
￮ s_end：目标车头初始位置序列
￮ r0_max：目标初始横向位置序列
￮ r_lead：纵向避让最近目标横向位置
￮ dr_yield：目标横向最近距离
￮ v_lat：目标横向速度
￮ v_lon：目标纵向速度
￮ ds_lim：overtake参考距离
￮ ds_overtake：overtake参考距离序列
￮ ds_sigma：overtake距离sigma修正序列
￮ ini_dis：目标到干涉点距离序列
￮ r_yield：目标到参考线最近距离
￮ jerk_min/max：jerk上下界约束序列
￮ ds_min_ini：overtake最近目标距离
￮ ID_max/min：纵向yield/overtake目标id
￮ Type_max/min：纵向yield/overtake目标类型
￮ traj_shape：预测轨迹形状类型
￮ IsActCons：是否生效硬约束
￮ ID_lead：纵向避让最近目标id
￮ Merge_Type_：汇流点类型
￮ obs_yield：纵向最大减速度避让目标id
￮ inter_dir：路口通行
￮ Type_lead：纵向避让最近目标类型
￮ ID_overtake：纵向overtake最近目标id
￮ Type_overtake：纵向overtake最近目标类型
￮ N_pre：预测总步数
￮ need_nudge：是否需要横向配合避让
￮ isSlowCloseLead：是否存在近距离低速目标
￮ isSlowLead：是否存在低速目标
￮ isTargetLaneObs：纵向避让目标是否位于目标车道
￮ isRetrograde：纵向避让目标是否为逆行目标
￮ HasOverlap：干涉点是否与自车有横向重叠
￮ soft_brake：刹车紧急程度判断
￮ Merge_Right_：汇流路权判断
￮ isCloseLaneFollow_：跟车距离是否过近
￮ exist_leadone_：是否存在横向重叠前车
￮ flagFirst：避让最近目标是否存在合理约束
￮ flagFastCutIn：是否避让快速目标
￮ flagDecay：纵向距离约束是否发生衰减
￮ flagCloseLead：是否存在近距离纵向避让目标
￮ flagType0：纵向避让最近目标类型是否unknown
￮ flagInvalid：是否存在预测修正目标
￮ ID_nudge：横向配合避让目标id
￮ Dir_nudge：横向配合避让方向
￮ nudge_priority：横向配合避让优先级
￮ dt_nudge：横向配合避让时长
￮ flagVlim：前车较远/无车场景判断
￮ flagVmin：前车较近但非紧急场景判断
￮ flagStop：需要立即刹停场景判断
￮ ds_merge：同向汇流overtake距离补偿
￮ ds_converge：非同向汇流overtake距离补偿
￮ v_set：期望速度
￮ s_set：与v_set对应纵向位置
￮ t_set：与v_set对应的时刻
￮ a_set：期望加速度
￮ ds_vset：计算v_set考虑的距离buffer
￮ v_set_lead：前车决定的期望速度
￮ s_set_lead：与v_set_lead对应的纵向位置
￮ v_set_obs：纵向避让障碍物对应的期望速度
￮ v_set_lc：考虑变道选空对应的期望速度
￮ ttc_base：基础跟车时距
￮ ttc_buff：跟车时距buffer
￮ enable_vel_gain_：是否需要加速速度收敛
￮ num：纵向避让最近目标时间index
￮ num_overtake：纵向overtake最近目标时间index
￮ sl/vl：纵向避让最近目标距离、速度
￮ s/v_overtake：纵向overtake最近目标距离、速度
￮ cs_lead：纵向避让最近目标cutin_score
￮ yaw_lead：纵向避让最近目标yaw
￮ smin_lead：纵向避让最近目标车尾位置
￮ sigma_yield/overtake：纵向yield/overtake目标预测sigma信息




# 06 纵向规划说明文档
1. 功能概述：
• 根据障碍物纵向决策、自车速度期望、限速/限加速度信息，生成满足安全性、舒适性、运动学约束要求的最优（s,v）序列
• 纵向规划求解失败或不满足安全检查时，生成backup速度序列
2. 问题定义：MPC优化问题
• 优化变量：X=jerk[i]
• 运动模型：质点运动模型
• constraint：
 
 
 
 
• cost function：
 
 
 
 
• 求解器：acado，参考http://acado.github.io/
3. 重点子模块
• update：自车规划初始状态更新
• LaneKeeping_Compute：纵向规划MPC问题定义及优化求解
• set_planning_result：更新纵向规划结果输出、速度序列检查及backup速度序列生成
• generateBackUpSpeedData：按照匀减速模型生成最小减速度安全避让速度序列，若不存在有效解，则输出最大减速刹停速度序列



# 07 横向规划说明文档
一、算法简介
 1、借助frenet坐标系将运动分解，并将道路边界与障碍物表示在l-s坐标系中

 
 
 
 
 2、建立优化问题
 1）确定优化变量：x={ l[i], l′[i], l″[i] }

 
 
 
 
 2）优化目标：最小化横向位移 l 和其一阶导l′ 以保证跟踪全局路径，最小化其二阶、三阶 导数以保证路径的舒适性

 
 
 
 
 3）约束条件考虑障碍物躲避、车辆的运动学约束、路径的连续性

 
 
 
 
二、输入
• 横向决策信息：变道、借道等行为决策，需要避让的障碍物和避让方向、横向主动偏移、
• 纵向决策规划信息：纵向规划的位置序列和速度序列、需要横向配合避让的障碍物与避让形式、
• 障碍物信息：障碍物当前的运动状态信息、预测信息等
• 地图信息：道路边界及相关属性，车道及相关属性，参考线的位置和曲率，路口、分汇流等语义信息等
• 自车状态信息：自车的位置、速度等
三、输出
• 将规划出的路径与纵向规划的结果结合成轨迹输出
• 横向规划的执行状况的反馈
四、重要结构体说明
structDynamicObs{// 动态障碍物预处理接口
 dyn_edge_points ： 保存了障碍物预测序列的polygon信息、对应插值的时间位置信息、纵向速度
 lat_decider_source ：障碍物决策来源，横向决策或纵向决策规划
 side_pass ： 障碍物避让方向
 lon_decider ： 障碍物的纵向决策，follow或overtake
 start_time ： 横向考虑避让的开始时间
 time_buffer ： 横向考虑避让的结束时间
 obstacle_type_buffer ：特殊障碍物的横向避让补偿
 id ： 障碍物id
 priority ： 障碍物避让优先级，-3纵向给到横向考虑的后方障碍物，-2，-1其他不同距离的后方障碍物，0纵向overtake并需要横向考虑的障碍物
 is_trunk ： 大车标志
 avd_decider ： 障碍物膨胀信息
 };

structObsBoundDeciderInfo{ //动态障碍物边界提取的决策信息
 init_lat_buffer_coef ： 横向初始膨胀
 init_lon_start_buffer_coef ：纵向向前初始膨胀
 init_lon_end_buffer_coef ：纵向向后初始膨胀
 lat_atten_time ： 横向膨胀开始衰减的时间
 lat_atten_coef ： 横向膨胀衰减的系数
 lon_atten_time ： 纵向膨胀开始衰减的时间
 lon_atten_coef ： 纵向膨胀衰减的系数
 constrain_time_buffer ： 计算约束边界时考虑预测的时长
 desire_time_buffer ： 计算期望边界时考虑预测的时长
 rear_constrain ： 计算车尾约束时是否考虑该障碍物
 is_positive ：主动大幅避让的障碍物
 is_ignore ： 忽略避让的障碍物
 };

structSLBoundaryEx {// 静态障碍物预处理接口
 id ： 障碍物id
 priority ： 障碍物避让优先级
 lon_ignore ： 是否纵向忽略、这是横向规划考虑的障碍物
 is_positive : 主动大幅避让的障碍物
 is_trunk : 大车标志
 is_cone_bucket : 锥桶
 obstacle_type_buffer ：特殊障碍物的横向避让补偿
 side_pass ： 避让方向
 lon_decider ：障碍物的纵向决策，follow或overtake
 edge_points ：保存了障碍物的polygon信息
 };

structRefPointInfo {//地图预处理信息接口，离散后，横向规划每一步上的地图信息
 curvature ： 曲率
 current_lane_width ： 当前车道宽
 left_lane_width ：左车道宽
 right_lane_width ：右车道宽
 left_lane_border ：参考线到左车道线距离
 right_lane_border ：参考线到右车道线距离
 left_road_border ： 参考线到左道路边缘距离
 right_road_border ：参考线到右道路边缘距离
 left_road_border_type ：左道路边缘的属性，1物理边界，0虚拟边界
 right_road_border_type ：右道路边缘的属性，1物理边界，0虚拟边界
 left_lane_border_type ： 左车道线的属性，1实线，0虚线
 right_lane_border_type ：右车道线的属性，1实线，0虚线
 };

五、重要函数说明
 1、PathPlanDecider
 1）update_lane_borrow 更新借道信息，用于限制地图的可通行区间。
 纵向来不及减速的障碍物，会给到横向紧急避让，此时横向决策虽然没有给到借道避让，但横向规划仍然会按借道决策处理；
 在车道保持并本车道内避让静态障碍物时，判断车道内避让障碍物的空间是否过小，如果过小将默认借旁边车道，并限制借道的宽度；
 2) update_lateral_offset 更新横向位置偏移，用于基础的横向位置期望（如果不设置的话，横向位置的期望就是参考线）。
 横向决策会有横向主动偏移的premove决策，用于贴线等动作；
 障碍物引起的横向位置偏移包括 ：变道时判断目标车道的后车是否有碰撞风险，如果有危险，横向位置期望的序列设置成当前位置，起到变道悬置的效果；
 变道时目标车道上有障碍物和自车有纵向重叠，此时自车会保持一个合适的横向位置；（priority== -3表示纵向给的后方车）
 车道保持或借道避让场景，避让障碍物后回归参考线上时，如果横向回参考线的方向上有障碍物，会保持一个合适的横向位置；
 3) process_spicial_obstacle 用于特殊类型障碍物的识别
 cutin障碍物识别：纵向对follow或者overtake的障碍物识别到cutin的属性。同时汇流时如果他车占路权，需要将其筛除cutin的判断；
 前方需要主动大幅避让的障碍物的识别：短时间或者短距离内要超过有比较大横向重叠的前方障碍物；
 变道时后方可忽略障碍物的识别：判断自车能否超越并横向卡住后车；
 4) update_buffer_decider 更新动态障碍物横纵向膨胀信息。（由于历史原因，现在有lidar版本的处理和没有lidar版本的处理还是分开的，不久就会把这部分统一了）
通用：
后车(obs.priority< 0)：相对普通障碍物，减小膨胀
纵向overtake障碍物（obs.priority== 0）：考虑纵向需要横向考虑的预测时长
纵向follow障碍物（obs.priority==1&&obs.lon_decider== “follow”）：计算期望边界时考虑3秒预测时长
曲线的外侧障碍物：当前位置膨胀加大，预测的未来轨迹膨胀减小
 有lidar版本：
非分汇流处车道保持时，后方、侧向、前方follow的障碍物减小膨胀
cutin障碍物：减小障碍物膨胀
低概率预测轨迹：减小预测膨胀
较远处障碍物：减少约束和期望使用预测的时长
对向障碍物：远处时减小膨胀、靠近后增大膨胀
主动大幅避让障碍物：增大纵向膨胀
纯视觉版本：
非分汇流处车道保持时，后方、侧向的障碍物减小膨胀
大车：大型车目前感知可能抖动，所以对未侵入本车道的大车减小膨胀
变道后方可忽略的障碍物：提取边界时忽略
cutin的后方车辆：减小膨胀
前方和主动避让的障碍物：增大膨胀
 2、PathBoundsDecider
 1) cal_motion_buffer 用于推演自车可能的行驶轨迹。
因为规划的结果是分段三次多项式，所以已知初始状态的情况下，可以通过设定dddl的序列，推演出设定的行驶轨迹。代码中主要有三个地方使用到该函数：
第一次是设定了接近极限的dddl序列，推演出轨迹，用于拓展车道线约束边界；
第二次是设定了接近极限的dddl序列，推演出轨迹，用于和障碍物约束边界检查，判断是否与障碍物有碰撞，如果有碰撞，则不需进行求解，进入backup逻辑；
第三次是设定了比较舒适的dddl序列，推演出轨迹，用于平滑横向位置的参考；
 2）check_motion_collide 用于判断极限规划时，是否仍会与障碍物碰撞，cal_motion_buffer的第二次使用即在此。
 3) GetBoundaryFromDynamicObstacles 用于提取动态障碍物的约束边界。
第一层遍历是对s的遍历，遍历规划的每一步的s；
第二层遍历是对动态障碍物的遍历，预处理中已对障碍物的预测进行了插值处理，已知规划的每一步s和对应时间t，即可知自车行驶到s位置，预测t时刻的障碍物位置，便可提取s位置的障碍物边界；
同时提取了车头、车体中心、车尾的边界约束；
 4) GetRefLineFromDynamicObstacles 用于提取动态障碍物的期望边界。
同动态障碍物的约束边界提取流程相似，不同的主要的横纵向膨胀大小。
 5) obstacle_avoid_edge_expand 将障碍物原始的多边形，根据横纵向膨胀的参数膨胀成新的多边形。对纵向给的紧急避让的障碍物会采取更保守的膨胀方式。
 6） cal_bound_temp 边界提取时，当遍历到s处，判断膨胀后的障碍物多边形在s或者s附近有纵向重叠，若多边形覆盖到s处，便提取到横向位置的边界。
 7）ExtractRefLine 根据期望边界，提取l的期望序列。
首先根据期望边界ref_bound和基础参考lat_behavior_offset_的位置关系提取出一条基础的ref_line；
在限定场景内（现在场景限制的比较多，可慢慢拓展开来），设定比较舒适的dddl序列，利用推演的轨迹平滑基础的ref_line；
为了障碍物处的横向位置收敛较好，在需要避让的位置上叠加了 l 的权重，其中ref_line变量pair中的第一个值表示的是l的位置期望，第二个值即叠加的权重值；
为了保证多次同方向避让连续，使用判断斜率slope的方式，将多次避让平滑成一次避让；
计算l 期望时可能会超出横向决策决定的地图可通行区域，此时将规划远端的期望限定在决策的地图可通行区域内；



# 08 红绿灯决策说明文档
• 1 概述
• 2 红绿灯决策特殊场景处理 
￮ 2.1 路口绿闪决策 
▪ 2.1.1 当前路口决策方案
▪ 2.1.2 路口决策方案优化过程
￮ 2.2 红绿灯遮挡决策 
▪ 2.2.1 遮挡决策跳转逻辑
▪ 2.2.2 基于车流信息的遮挡通行决策
￮ 2.3 低速路口通行决策
￮ 2.4 其他故障保护逻辑 
▪ 2.4.1 黑灯保护逻辑
▪ 2.4.2 左转箭头灭灯保护逻辑
▪ 2.4.3 黄闪保护逻辑(原)
• 3 附录


## 1 概述
 
 
 
 
左图介绍了红绿灯决策状态机的状态跳转与自车位置间的关系。右图介绍了状态机的跳转逻辑。
1）如右图所示，每次进入自动驾驶状态后，累计一帧红绿灯信息，INIT状态将跳转至LANE_KEPPING状态。
【0,0,0】——【1,0,0】
2）如左图所示，自下向上看，当自车距离路口停止线大于90米时，状态机处于LANE_KEPPING状态。当距离路口停止线距离小于90米时，状态机进入APPROACH_STOPLINE状态。
【1,0,0】——【3,0,0】
3）如右图所示，在APPROACH_STOPLINE状态下，当红绿灯为绿灯，或绿闪/黄灯决策为通行，或OFF（灭灯），或黄灯闪烁状态时，自车保持APPROACH_STOPLINE状态继续行驶；当红绿灯为红灯，或绿闪/黄灯决策为停止时，状态机将进入RED_LIGHT_STOP状态。当红绿灯被遮挡时，状态机进入COVER_LIGHT状态。
【3,0,0】——【4,0,0】
【3,0,0】——【5,0,0】/【5,1,0】
4）在RED_LIGHT_STOP状态，状态机将给出停车决策。当红绿灯变为绿灯或OFF时，将再次进入APPROACH_STOPLINE状态。
【4,0,0】——【4,1,0】
【4,1,0】——【3,0,0】
5）在COVER_LIGHT状态，当看见绿灯、黄灯闪烁、灭灯，或看见绿闪且为通行决策时，再次进入APPROACH_STOPLINE状态。
当看见红灯，或看见绿闪且为停车决策时，状态机将进入RED_LIGHT_STOP状态。若连续多帧看不见灯，将结合最后一帧看见红绿灯的状态，和自车车速，距路口距离等因素进入遮挡决策。
（常见场景，当前方有大车遮挡红绿灯时，自车将给出停车决策，与前车拉开一定车距，可以看见红绿灯时，再做出对应的决策）
【5,0,0】/【5,1,0】——【4,1,0】
【5,0,0】/【5,1,0】——【3,0,0】
6）在APPROACH_STOPLINE状态下，当自车通过路口停止线，进入路口时，状态机将进入CROSSING状态。
【3,0,0】——【2,0,0】
7）在路口内CROSSING状态下，若红绿灯为红灯或黄灯，且满足低速路口通行决策（intersection_decision）的条件时，自车会给出停车决策。
等再次看见绿灯后重新给出通行决策。（常见场景，若自车已过停止线，但路口内较为拥堵，继续行驶有干扰横向交通的风险，此时会触发低速路口通行决策）
当驶离路口后，状态机将从CROSSING状态跳转为LANE_KEPPING状态。
【2,0,0】——【2,1,0】
【2,0,0】——【1,0,0】
## 2 红绿灯决策特殊场景处理
### 2.1 路口绿闪决策
2.1.1 当前路口决策方案


1）绿闪通行决策的因素，包含了规划可通行距离因素，以及舒适可通行距离因素两方面。
2）结合视觉感知的延时，通常设定绿闪固定闪烁时间为2.1s。当前方有纵向目标时，为避免低速进入路口，导致变灯后路口内拥堵，因此每多一个纵向目标时，绿闪可通行时间减少0.1秒。绿闪开始后，从视觉感知获取绿闪持续时间。用绿闪设定时间，减去纵向目标拥堵损耗时间，减去绿闪持续时间，即为绿闪可通行时间。计算绿闪可通行时间内的纵向规划通行距离，即为绿闪可通行距离。
3）舒适度可通行距离的计算是依据人体主观感受舒适的最大jerk值计算。设定允许最大加速度amax，允许最大jerk值k，获取自车加速度a0，可以计算出从a0减速至amax所需时间t1。同时，获取自车车速v0，并计算刹停所需时间 t。
基于jerk舒适度可通行距离的计算方法链接：
基于jerk舒适度因素的绿闪决策
4）当规划可通行距离＞自车距停止线距离 且 舒适刹停所需距离＞自车距停止线距离时，绿闪为通行决策。
5）当自车刹停所需减速度非常大，超过基于自车速度的舒适度阈值时，会给出通行决策。
6）在确保安全的基础上设定较为保守的通行决策，既可以提高通行效率，又避免了一些场景下的制动过重问题。同时，较为保守的绿闪决策避免了驶入路口后因横向车道变成绿灯而造成的一系列路口内问题。
2.1.2 路口决策方案优化过程
1）第一版
a = v² / 2 * (dist_to_stopline_) 的加速度舒适度方案 +
s = v * t 的可通行距离方案
2）第二版
基于jerk 因素舒适度方案 +
s = v * t 的可通行距离方案
3）第三版
基于jerk 因素舒适度方案 +
基于可通行距离因素的距离方案
4）第四版
基于jerk 因素舒适度方案 +
基于可通行距离因素的距离方案（根据纵向目标数量调整可通行时间）
5）第五版
基于jerk 因素舒适度方案 +
基于可通行距离因素的距离方案（根据纵向目标数量调整可通行时间）+
最大加速度舒适度上限
### 2.2 红绿灯遮挡决策
2.2.1 遮挡决策跳转逻辑
 
 
 
 
由于目前红绿灯融合过程中，前1.5s融合结果均为UNKNOWN状态，因此每一个路口都有一个初始融合结果UNKNOWN的保护逻辑。
该保护逻辑对每个路口前9帧UNKNOWN会给出通行决策，以避免因初始化的UNKNOWN造成的误制动。
2.2.2 基于车流信息的遮挡通行决策
Step1：目标车道筛选
筛选出与自车行驶方向相同的车道
Step2：目标车辆筛选
横向边界约束
纵向位置约束
朝向约束
Step3：目标车辆意图判断
对于筛选出的目标车辆，通过获取其行驶速度v 及到停止线的距离 dist_to_stopline_
估算其刹停所需减速度 a = v² / 2 * (dist_to_stopline_)
当刹停所需减速度大于舒适度阈值时认为目标车辆意图为通行
### 2.3 低速路口通行决策
设计目的：对于自车低速进入路口后，由于路口内信号灯发生变化，而需要停车等待的场景，设计了低速路口通行决策
低速路口通行决策条件：
1，信号灯颜色：红灯、黄灯、长路口+绿闪
2，自车位置：在路口内+可以看见红绿灯（红绿灯在摄像头视野内）
3，速度条件（满足其中一项）
速度条件1：自车速度<0.3m/s
速度条件2：自车速度<1.0m/s 且 3秒内规划可通行距离+过路口距离< 10米
速度条件3：自车速度<1.0m/s 且 纵向跟随目标数>4
4，通过路口距离条件：自车通过路口距离 < 路口长度 * 25%，当 路口长度 * 25% < 7m时以7m计
5，通行条件：自车本身为通行决策
### 2.4 其他故障保护逻辑
2.4.1 黑灯保护逻辑
Step1：路口真假黑灯判断
1）默认路口融合结果为真正的黑灯（即灯不亮）
2）当路口内出现红灯、绿闪、黄灯等有颜色的灯时，说明给出的黑灯是假黑灯（即误检的黑灯）
3）假黑灯保护逻辑：当判断为假黑灯，但是持续5s融合结果给出黑灯，则认为是真黑灯（即有颜色的灯是误检到的）
Step2：真假黑灯决策
1）真黑灯：通行
2）假黑灯：绿闪中混杂假黑灯，与绿闪决策保持一致；红灯中混杂假黑灯，与红灯决策保持一致
3）假黑灯持续过久：通行决策
Step3：黑灯flag刷新
每次通过路口停止线会刷新黑灯flag为默认值
2.4.2 左转箭头灭灯保护逻辑
对于同时存在直行灯（圆灯）和左转箭头的路口
当左转箭头为黑灯时，会以直行灯（圆灯）的颜色进行决策



# 09 ST图构建说明文档
ST图构建模块的主要职能：
• 构建变道截止点虚拟障碍物信息
• 根据指定的参考线筛选纵向需要考虑的障碍物
• 构建障碍物预测轨迹在frenet参考系的序列化信息
• 根据状态和场景生成部分显式的纵向决策
• 结合上游决策和障碍物信息生成给纵向决策需要的ST图信息
• 输出部分预制动信息

主要成员变量说明：
left_border_right_border_ 虚拟车道左右边界
reference_line_segments_ 虚拟车道分段梯形结构
obstacle_first_time_state_map_ 所有障碍物初始感知信息的提取和维护
insight_static_obstacle_map_ 静态障碍物集合
insight_dynamic_obstacle_map_ 动态障碍物集合
last_insight_dynamic_obstacle_map_ 上一帧动态障碍物集合
lane_keep_sv_ 下游纵向决策使用的ST图结构信息
normal_sv_ 未包含纵向决策的ST图结构信息
rear_sv_ 包含超车决策的ST图结构信息
yield_sv_ 包含跟车决策的ST图结构信息
obstacle_tag_list_ 障碍物意图信息
dynamic_prebrake_signal_ 动态障碍物预制动信号
static_prebrake_signal_ 静态障碍物预制动信号
is_prebrake_target_transverse_ 静态预制动目标是否处于横穿姿态
is_in_lane_crossing_stage_ 是否处于变道/借道执行状态
is_obs_considered_ 作为临时变量判定障碍物是否已被ST图考虑
is_lane_interval_transformed_ 虚拟车道是否相对当前车道产生了车道级别的整体偏移

主要函数说明：
construct_virtual_obstacles 根据变道任务信息生成虚拟墙并赋予follow决策，避免变道压实线
set_cross_lane_state 结合变道/借道信息生成粗略的虚拟车道范围
set_reference_line_border 结合上游横向决策信息和变道/借道信息生成更紧致的虚拟车道范围，并以多段梯形polygon的方式维护
generate_shifted_reference_line 根据虚拟车道信息构建包含车道边界线和参考中心线信息的虚拟车道数据结构
preliminary_screen_obstacles 基于几何关系和相对运动趋势等物理信息初筛需要考虑的障碍物
compute_obs_sl_polygon_seq对于需要考虑的障碍物均构建其frenet预测轨迹信息
compute_sl_polygon_sequence 基于笛卡尔系下特定障碍物的预测轨迹信息构建相应的在frenet坐标系下的投影，以polygon的形式维护（对于动态障碍物需要考虑完整的时域，并且对于远距离轨迹点基于单点和尺寸信息扩展成polygon，对于需要精细构建的近距离轨迹点需要使用完整的角点信息均做坐标转换，甚至在参考线大曲率处对笛卡尔系polygon进行各边插值得到更多的角点进行转换）
compute_st_boundaries 根据虚拟车道和frenet坐标系下障碍物预测轨迹信息构建没有任何纵向决策障碍物的相应的ST图信息
translate_scenario_decisions根据状态和场景生成部分显式的纵向决策，比如变道阶段目标车道后方车需要给出overtake决策，以及行人交互的follow决策
check_stop_for_pedestrian 行人交互函数，会对近距离低速行人在之前存在跟停决策之后做纵向跟随决策的一致性考虑，设置最大跟停时间和调整其横向避让buffer以维持跟停决策，并在使能过程中清除掉横向避让决策
compute_st_graph_with_decisions 计算已经包含纵向决策障碍物的ST图信息
set_obstacle 遍历需要考虑的障碍物更新需要输出给下游纵向决策的定制化ST图结构
set_potential_obstacle 对ST图中未考虑的障碍物检查是否需要做预制动
extract_info 生成输出给下游模块的结构信息
process_lane_keep_sv 对输出给下游纵向决策的定制化ST图结构加入红绿灯停止线信息
process_rear_sv对输出给下游纵向决策的定制化ST图结构根据纵向超车的ST图信息做修正
set_obstacle_tag 对障碍物赋予语义信息
set_prebrake_signal_from_dynamic 生成对动态障碍物的预制动信息
set_prebrake_signal_from_static 生成对静态障碍物的预制动信息
mpc_obs_num_ 障碍物在ST图中考虑的数目
frenet_coord_ frenet参考系
config_ 配置参数

其他相关文件和函数说明：
st_boundary_mapper.cpp 一种基于笛卡尔系下参考路径离散点和预测轨迹离散点逐点进行碰撞检查的ST图构建流程，不需要显示进行坐标转化，但是也因此缺乏frenet信息输出
主函数ComputeSTBoundary 对每个障碍物根据是否存在纵向决策分别提取其ST图上的表示信息
GetOverlapBoundaryPoints 对单一障碍物和基于离散点序列的固定路径对每一时刻障碍物是否与路径发生干涉以及干涉的纵向范围进行计算
MapStopDecision 对具有跟停决策的目标构建其ST图信息
CheckOverlap 碰撞检查函数

流程说明：
主程序流程：
 
 
 
 

虚拟障碍物设置流程：
• 首先判别自车当前车道方向是否符合地图任务的导航方向is_current_lane_on_right_direction
• 然后根据完整的地图任务信息（可能包含多个连续变道任务）和当前目标车道信息获取后续需要跨越的车道数目cross_lanes_num
• 基于虚线剩余距离作为默认设置虚拟障碍物的位置，根据cross_lanes_num调整留出足够阈值。同时对选空信息做相应的适配和保护，避免虚拟障碍物设置在选空静止后方车的后方
• 另一方面根据自车当前所处的状态，在处于最后一次地图任务变道执行或者主动变道执行阶段屏蔽设置 也即should_construct_virtual_obs
• 在路口尽管可能没有地图任务，但依然会结合车道线和出路口地图任务信息设置虚拟障碍物，避免自车快速通过路口使得后续变道任务难以执行
• 最终结合is_current_lane_on_right_direction 和 should_construct_virtual_obs 标志确定是否需要设置虚拟障碍物，并针对变道和借道选空信息再次调整设置距离

行人交互策略
 
 
 
 
行人交互主要需要考虑的因素是如何应对预测不准而导致的纵向避让决策的不一致性，因此在设计上目前对一定距离范围内速度阈值小于设定阈值的行人/ofo做筛选，认为这类行人的意图属于高度不确定的。并根据是否在之前时刻的决策规划过程中对这类行人或者ofo产生过纵向跟随follow决策来触发一致性的保护，也即通过清除其当前帧的横向避让决策和设置更大的横向避让膨胀距离compensate_avoid_buffer间接使得其纳入纵向st图考虑的范围内，并自然在后续纵向决策过程中产生稳定的follow决策。
在设置针对触发了交互决策障碍物的compensate_avoid_buffer时，会根据设定的最大生效时长kPedestrianStopTimeout 和 最小生效时长kMinPedestrianStopTime来动态调整，使得在行人远离时和本身横向相距自车就比较远时灵活触发交互机制。

动态障碍物sl_polygon_seq的构建过程：
本质上非常明确，就是基于其笛卡尔系的预测轨迹点序列逐点进行其polygon的提取和对frenet坐标系的投影转换，具体还会结合需求调整计算策略，尽可能较少计算量
• 识别障碍物初始预测轨迹点相距自车是否足够远，满足条件则使能rough_mode 粗分辨率构建模式
• 对于障碍物预测轨迹点做遍历 
￮ 如果使能了enable_heuristic_search 策略则会根据对此障碍物前一时刻的sl_polygon纵向范围去估算这一时刻大致的sl_polygon范围，引导frenet坐标转换做更少的区间查找
￮ 如果使能了rough_mode构建模式，则只会针对障碍物当前中心坐标点做一次frenet坐标转换generate_rough_obs_sl_polygon，并在转换后近似认为frenet坐标转换为仿射转换拓展出box的其余四个角点的frenet坐标信息
￮ 如果没有使能rough_mode构建模式，则会提取完整的笛卡尔系polygon信息GetPolygonAtPoint，并逐点进行frenet坐标转换，并且最终还会进一步根据其附近frenet曲率信息大小决定是否进行多边细化插值，构建更准确的凸包generate_precise_sl_polygon
• 在遍历过程中会主动判别当前时刻是否构建成功，并在构建失败时认定为是失效时间区间，最终得到完整的invalid_time_sections，在后续插值查找过程中，如果时间落在此区间内则会以失败返回，认为对应时刻并没有发生干涉或者说进入到st图中



# 10 部分功能文件说明文档

speed_profile_generator
主要功能：生成不同运动过程下的速度剖面
主要函数：
GenerateFallbackSpeed 根据输入刹停距离stop_distance 和初始速度init_v和加速度init_a 生成对应的速度剖面
GenerateFixedDistanceCreepProfile 根据输入的最终刹停距离distance 和最大速度限制输出一条前段能够以最大速度行驶最终以指定急刹车减速刹停的速度剖面
GenerateConstAccSpeed根据输入的最大速度max_speed 初始速度init_v 指定加速度acc 和时域长度t_limit 输出匀加速速度剖面
GenerateStopProfile根据输入的初始速度init_speed 初始加速度init_acc 和指定加速度acc 生成匀减速刹停速度剖面

baseline_info
主要功能：
• 维护车道结构信息，包含左右车道边缘线和参考中心线
• 维护基于车道参考中心线构建的frenet坐标系，以及在此frenet坐标系下的自车和其他障碍物的信息
• 提供车道盲区检测等功能函数
主要成员变量：
lane_id_ 车道相对自车当前所处车道的id，一般为{-1,0,1}
is_valid_ 车道信息是否构建完全
reference_line_ 笛卡尔系下车道边缘线和参考中心线信息维护
raw_refline_points_ 地图原始参考中心线
ego_state_manager_ 包含frenet信息的自车状态信息维护
obstacle_manager_ 包含frenet信息的所有障碍物信息维护
right_of_way_status_ 路权属性
convex_hull_ 车道几何范围的凸包表示
beside_bus_stop_ 是否邻近公交车站
beside_facility_exit 是否邻近地库出口
construct_failed_reason_ 构建失败原因

主要功能函数说明
is_adc_on_lane 判别自车当前是否大部分处于该车道，判定标准以自车侵入车道面积占自车面积的比例为准
is_obstacle_intersect_with_lane 判别障碍物是否和该车道发生了干涉
get_fov_in_lon_direction 盲区检测函数 输入s_begin s_end为纵向检测范围，shift_l 和shift_side_l为横向检测范围，其中shift_l是检测中心线偏移，shift_side_l为靠近视角一侧的横向边界，ignore_obs_ids 为预设输入的忽略检测障碍物。输出blocked_s_sections物理干涉的纵向区间集合和对应的速度信息blocked_vel_sections，以及视线遮挡的区间集合invisible_s_sections
get_bus_stop_occlusion 检查车道旁公交车站是否存在盲区遮挡（以此使能盲区预制动）
detect_brokendown_car_in_solidline 检查变道实线区故障车是否存在非遮挡的前方可行空（以此使能压实线变道的功能）
construct_reference_line 地图中该参考线中笛卡尔信息的提取和处理，frenet参考系的构建和相应自车状态的维护
construct_obstacle_managerfrenet参考系下所有障碍物感知信息的更新和维护
compute_convex_hull 计算车道几何范围的凸包表示





