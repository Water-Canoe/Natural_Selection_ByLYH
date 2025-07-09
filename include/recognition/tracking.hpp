#pragma once

#include "common.hpp"
#include <vector>
#include <opencv2/opencv.hpp>

namespace recognition{

enum Corner_Type
{
    LEFT_UP = 0,
    LEFT_DOWN,
    RIGHT_UP,
    RIGHT_DOWN
};

class Tracking
{
public:
    common::Camera _camera;
    cv::Mat _draw_frame; // 绘制图像
    
    Tracking();
    ~Tracking();

    void Picture_Process();
    bool Find_Start_Point(int scan_start_y = 3, int scan_height = 10);
    void Track_Recognition();
    void Edge_Extract();
    void Draw_Edge();

    // 公共访问方法，用于获取处理结果
    const std::vector<common::POINT>& Get_Maze_Edge_Left() const { return _maze_edge_left; }
    const std::vector<common::POINT>& Get_Maze_Edge_Right() const { return _maze_edge_right; }
    const std::vector<common::POINT>& Get_Edge_Left() const { return _edge_left; }
    const std::vector<common::POINT>& Get_Edge_Right() const { return _edge_right; }
    const uint16_t Get_Valid_Row() const { return _valid_row; }
    const uint16_t Get_Height() const { return _height; }
    const uint16_t Get_Width() const { return _width; }
    const std::vector<int> Get_Width_Block() const { return _width_block; }
    const common::POINT Get_Corner(Corner_Type corner_type) const{
        switch(corner_type){
            case LEFT_UP: return _corner_left_up;
            case LEFT_DOWN: return _corner_left_down;
            case RIGHT_UP: return _corner_right_up;
            case RIGHT_DOWN: return _corner_right_down;
            default: return common::POINT(0,0);}}

private:
    common::Parameter _parameter;

    std::vector<common::POINT> _maze_edge_left;  // 迷宫左边线点集
    std::vector<common::POINT> _maze_edge_right; // 迷宫右边线点集
    std::vector<common::POINT> _edge_left;  // 左边缘点赛道左边缘点集
    std::vector<common::POINT> _edge_right; // 右边缘点赛道右边缘点集
    //std::vector<common::POINT> _width_block;   //每行色块宽度
    std::vector<common::POINT> _spurroad;   // 岔路信息
    std::vector<int> _width_block;   //每行色块宽度
    double stdev_edge_left; // 左边缘点方差
    double stdev_edge_right; // 右边缘点方差
    int valid_row_left = 0; // 左边缘点有效行数
    int valid_row_right = 0; // 右边缘点有效行数
    common::POINT garage_enable {0,0}; // 车库入口点
    uint16_t _valid_row;    //有效行数
    uint16_t row_cut_up;    //图像顶切
    uint16_t row_cut_down;  //图像底切
    int _width;
    int _height;
    int _border; // 边框宽度

    //=================================角点信息====================================
    common::POINT _corner_left_up {0,0};  //左上角点
    common::POINT _corner_left_down {0,0};  //左下角点
    common::POINT _corner_right_up {0,0};  //右上角点
    common::POINT _corner_right_down {0,0};  //右下角点
};
    
}
