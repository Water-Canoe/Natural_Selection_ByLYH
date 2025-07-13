#pragma once

#include "common.hpp"
#include "recognition.hpp"
#include "control/controlcenter.hpp"

namespace control{

#define PWMSERVOMID 1500

class Motion
{

public:
    bool _motion_enable = false; //运动控制使能
    uint16_t _servo_pwm = PWMSERVOMID;
    float _speed = 0.3;     //电机速度控制值
    
    Motion();
    ~Motion();

    void Pose_Control(int control_center,recognition::Tracking& tracking);

    void Speed_Control(bool enable,bool slow_down,ControlCenter& control_center,recognition::Tracking& tracking);

    float _speed_low; //最低速
    float _speed_high; //最高速
    float _speed_bridge; //坡道速度
    float _speed_catering; //快餐店速度
    float _speed_layby; //临时停车区速度
    float _speed_obstacle; //障碍区速度
    float _speed_parking; //停车区速度
    float _speed_ring; //环岛速度
    float _speed_down; //特殊元素<慢行区>速度
    float _run_p1; //一阶比例系数：直线控制量
    float _run_p2; //二阶比例系数：弯道控制量
    float _run_p3; //三阶比例系数：弯道控制量
    float _turn_p; //一阶比例系数：转向控制量
    float _turn_d; //一阶微分系数：转向控制量

private:
    int _count_shift = 0;   //变速计数器,实现赛道平滑过渡


    // ========================================================== 参数 ==========================================================
    common::Parameter _parameter;
};

}