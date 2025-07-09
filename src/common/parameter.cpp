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