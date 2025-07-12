#pragma once

#include <nlohmann/json.hpp>

namespace common{

class Parameter
{

public:
    Parameter();
    ~Parameter();
    void Load_Config();
    void Save_Config();

    void Add_Parameter(const std::string& key, const nlohmann::json& value);
    nlohmann::json Get_Parameter(const std::string& key);

private:
    nlohmann::json _config; //配置文件
    std::string _config_path; //配置文件路径

    int _threshold; //图像二值化阈值
    std::string _debug_mode; //调试模式
    std::string _debug_picture_path; //调试图片路径
    std::string _debug_video_path; //调试视频路径
    int _image_width; //图像宽度
    int _image_height; //图像高度
    int _row_cut_up; //图像顶切
    int _row_cut_bottom; //图像底切
    double _corner_left_up_slope1_min; //左上角点斜率1最小值
    double _corner_left_up_slope1_max; //左上角点斜率1最大值
    double _corner_left_up_slope2; //左上角点斜率2
    double _corner_right_up_slope1_min; //右上角点斜率1最小值
    double _corner_right_up_slope1_max; //右上角点斜率1最大值
    double _corner_right_up_slope2; //右上角点斜率2

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
    float _turn_p; //一阶比例系数：转弯控制量
    float _turn_d; //一阶微分系数：转弯控制量
    bool _motion_enable; //运动控制使能
};


}