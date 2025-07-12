#pragma once

#include "common.hpp"
#include "recognition/tracking.hpp"
#include <string>
#include <fstream>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>

namespace control{

class ControlCenter
{

public:
    int _control_center;  //控制中心坐标
    std::vector<common::POINT> _center_edge;  //控制中心线
    uint16_t _left_valid_row = 0;  //左边有效行数
    uint16_t _right_valid_row = 0;  //右边有效行数
    double _sigma_center = 0;       //中心线方差

    void Fitting(recognition::Tracking& tracking);


private:
    std::string _style = " "; 



};

}

