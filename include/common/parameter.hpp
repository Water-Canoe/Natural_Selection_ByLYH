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
};


}