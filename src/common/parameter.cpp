#include "common/parameter.hpp"
#include <fstream>
#include <iostream>

using namespace std;
using json = nlohmann::json;

namespace common{

Parameter::Parameter()
{
    _config_path = "../../config/config.json";
    _config = json::parse(ifstream(_config_path));
}

Parameter::~Parameter()
{

}

void Parameter::Load_Config()
{
    _threshold = _config["threshold"];
    _debug_mode = _config["Debug_Mode"];
    _debug_picture_path = _config["Debug_Picture_Path"];
    _debug_video_path = _config["Debug_Video_Path"];
    _image_width = _config["Image_Width"];
    _image_height = _config["Image_Height"];
    _row_cut_up = _config["Row_Cut_Up"];
    _row_cut_bottom = _config["Row_Cut_Bottom"];
    _corner_left_up_slope1_min = _config["Corner_Left_Up_Slope1_Min"];
    _corner_left_up_slope1_max = _config["Corner_Left_Up_Slope1_Max"];
    _corner_left_up_slope2 = _config["Corner_Left_Up_Slope2"];
    _corner_right_up_slope1_min = _config["Corner_Right_Up_Slope1_Min"];
    _corner_right_up_slope1_max = _config["Corner_Right_Up_Slope1_Max"];
    _corner_right_up_slope2 = _config["Corner_Right_Up_Slope2"];
    _speed_low = _config["Speed_Low"];
    _speed_high = _config["Speed_High"];
    _speed_bridge = _config["Speed_Bridge"];
    _speed_catering = _config["Speed_Catering"];
    _speed_layby = _config["Speed_Layby"];
    _speed_obstacle = _config["Speed_Obstacle"];
    _speed_parking = _config["Speed_Parking"];
    _speed_ring = _config["Speed_Ring"];
    _speed_down = _config["Speed_Down"];
    _run_p1 = _config["Run_P1"];
    _run_p2 = _config["Run_P2"];
    _run_p3 = _config["Run_P3"];
    _turn_p = _config["Turn_P"];
    _turn_d = _config["Turn_D"];
    _motion_enable = _config["Motion_Enable"];
}

void Parameter::Save_Config()
{
    ofstream file(_config_path);
    file << _config.dump(4); //将配置文件写入文件
    file.close();
}

void Parameter::Add_Parameter(const std::string& key, const nlohmann::json& value)
{
    _config[key] = value;
    Save_Config();
}

nlohmann::json Parameter::Get_Parameter(const std::string& key)
{
    return _config[key];
}


}