#include "control/motion.hpp"

using namespace common;
using namespace recognition;
using namespace std;

namespace control{

Motion::Motion()
{
    _speed_low = _parameter.Get_Parameter("Speed_Low");
    _speed_high = _parameter.Get_Parameter("Speed_High");
    _speed_bridge = _parameter.Get_Parameter("Speed_Bridge");
    _speed_catering = _parameter.Get_Parameter("Speed_Catering");
    _speed_layby = _parameter.Get_Parameter("Speed_Layby");
    _speed_obstacle = _parameter.Get_Parameter("Speed_Obstacle");
    _speed_parking = _parameter.Get_Parameter("Speed_Parking");
    _speed_ring = _parameter.Get_Parameter("Speed_Ring");
    _speed_down = _parameter.Get_Parameter("Speed_Down");
    _run_p1 = _parameter.Get_Parameter("Run_P1");
    _run_p2 = _parameter.Get_Parameter("Run_P2");
    _run_p3 = _parameter.Get_Parameter("Run_P3");
    _turn_p = _parameter.Get_Parameter("Turn_P");
    _turn_d = _parameter.Get_Parameter("Turn_D");
}

Motion::~Motion()
{
    // 析构函数实现 - 清理资源
    // 由于没有动态分配的内存，这里可以为空
}


/**
 * @brief 姿态控制
 * @param control_center 控制中心
 * @param tracking 
 */
void Motion::Pose_Control(int control_center,Tracking& tracking)
{
    int width = tracking.Get_Width();
    //偏差计算
    float error = control_center - width / 2; 
    //偏差平滑处理
    static int error_last = 0;
    if(abs(error - error_last) > width/10) //偏差过大,直接赋值
    {
        error = error > error_last ? (error_last + width/10) : (error_last - width/10);
    }
    //自适应比例系数
    _turn_p = abs(error) * _run_p2 + _run_p3;
    //PD控制输出计算
    int pwm_diff = (error * _turn_p) + (error - error_last) * _turn_d;
    error_last = error;
    //PWM信号生成
    _servo_pwm = (uint16_t)(PWMSERVOMID + pwm_diff);
}


/**
 * @brief 速度控制
 * @param enable 是否启用加速摸索模式
 * @param slow_down 是否启用慢速模式
 * @param control_center 控制中心
 * @param tracking 跟踪器
 */
void Motion::Speed_Control(bool enable,bool slow_down,ControlCenter& control_center,Tracking& tracking)
{
    uint8_t control_low = 0;    //速度控制下限
    uint8_t control_mid = 5;    //速度控制中值
    uint8_t control_high = 10;  //速度控制上限

    // int width = tracking.Get_Width();  // 暂时注释掉未使用的变量
    int height = tracking.Get_Height();
    // =====================================慢速模式=====================================
    if(slow_down)
    {
        _count_shift = control_low;
        _speed = _speed_down;
    }
    // =====================================正常模式=====================================
    else if(enable)
    {
        if(control_center._center_edge.size() < 10) //赛道中心点数量过少,直接进入低速模式
        {
            _speed = _speed_low;
            _count_shift = control_low;
            return;
        }
        if(control_center._center_edge[control_center._center_edge.size() - 1].y > height / 2) //赛道中心点在图像下方,直接进入低速模式
        {
            _speed = _speed_low;
            _count_shift = control_low;
            return;
        }
        if(abs(control_center._sigma_center) < 100.0)   //路径平滑，加速
        {
            _count_shift ++;
            if(_count_shift > control_high)
            {
                _count_shift = control_high;
            }
        }
        else    //复杂路况减速
        {
            _count_shift --;
            if(_count_shift < control_low)
            {
                _count_shift = control_low;
            }
        }

        if(_count_shift > control_mid)
            _speed = _speed_high;   //高速模式
        else
            _speed = _speed_low;   //低速模式
    }
    // =====================================禁用加速摸索模式=====================================
    else
    {
        _count_shift = control_low;
        _speed = _speed_low;
    }
}


}