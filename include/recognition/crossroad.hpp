#include "common.h"
#include <opencv2/opencv.hpp>
#include "recognition/tracking.hpp"

namespace recognition
{
class Crossroad
{
public:
    Crossroad();
    ~Crossroad();





private:
    common::POINT _corner_left_up {0,0};  //左上角点
    common::POINT _corner_left_down {0,0};  //左下角点
    common::POINT _corner_right_up {0,0};  //右上角点
    common::POINT _corner_right_down {0,0};  //右下角点
    std::vector<POINT> _left_line_fixed;          //补线后左线
    std::vector<POINT> _right_line_fixed;         //补线后右线
}

}