#include "recognition/element.hpp"
#include "common.hpp"

using namespace common;
using namespace std;

namespace recognition
{

Element::Element()
{
    scene = Scene::NolmalScene;
    _crossroad_flag = false;
    _ring_flag = false;
    _bridge_flag = false;
    _obstaclee_flag = false;
    _zebra_flag = false;
    _crossroad_cnt = 0;
    _ring_cnt = 0;
    _bridge_cnt = 0;
    _obstaclee_cnt = 0;
    _zebra_cnt = 0;
}

Element::~Element()
{

}

Scene Element::Recognition_Element(Tracking &tracking)
{   
    //清除标志位
    scene = Scene::NolmalScene;
    _crossroad_flag = false;
    _ring_flag = false;
    _bridge_flag = false;
    _obstaclee_flag = false;
    _zebra_flag = false;

    uint16_t zebra_cnt[2] = {0,0};//斑马线计数
    uint8_t zebra_flag = 0;//斑马线识别标志

    uint16_t crossroad_cnt[2] = {0,0};//十字路口计数
    uint8_t crossroad_flag = 0;//十字路口识别标志

    uint16_t ring_cnt = 0;//环岛计数
    uint8_t ring_flag = 0;//环岛识别标志

    uint16_t obstacle_cnt = 0;//障碍物计数
    uint8_t obstacle_flag = 0;//障碍物识别标志

    // 更新补线信息
    _crossroad_left_line.clear();
    _crossroad_right_line.clear();

    uint16_t valid_row = tracking.Get_Valid_Row();

    for(int i = 0;i < valid_row;i++)
    {
        //======================================斑马线识别========================================
        // 1.处于中间区域
        if(i > tracking.Get_Height() / 3 && i < tracking.Get_Height() / 3 * 2)
        {
            // 2.赛道宽度合理
            for(int j = tracking.Get_Edge_Left()[i].x + 20;j < tracking.Get_Edge_Right()[i].x - 20;j++)
            {
                if(tracking.Get_Width_Block()[i] < tracking.Get_Width() * 0.6)
                {
                    // 3.满足连续跳变
                    if(tracking._camera.Get_Binary_Frame().at<uchar>(i,j) == WHITE 
                    && tracking._camera.Get_Binary_Frame().at<uchar>(i,j+1) == BLACK
                    && tracking._camera.Get_Binary_Frame().at<uchar>(i,j+2) == BLACK)
                    {   //白变黑
                        zebra_cnt[0]++;
                    }
                    if(tracking._camera.Get_Binary_Frame().at<uchar>(i,j) == BLACK 
                    && tracking._camera.Get_Binary_Frame().at<uchar>(i,j+1) == WHITE
                    && tracking._camera.Get_Binary_Frame().at<uchar>(i,j+2) == WHITE)
                    {   //黑变白
                        zebra_cnt[1]++;
                    }
                    if(zebra_cnt[1] > 30 && zebra_cnt[0] > 30)
                    {
                        return Scene::ZebraScene;
                    }
                }
            }
        }
        //======================================十字路口识别========================================
        // 1.赛道存在宽度突变
        if(i> 20 && tracking.Get_Width_Block()[i] > tracking.Get_Width() * 0.9)
        {
            crossroad_cnt[0]++;
        }
        // 2.存在角点
        if(crossroad_cnt[0] >= 20)
        {
            // 十字上拐点区域识别
            if(tracking.Get_Corner(LEFT_UP).x > 20 
                && (tracking.Get_Corner(RIGHT_UP).x > 20 && tracking.Get_Corner(RIGHT_UP).x < tracking.Get_Width() - 20)) // 上拐点存在
            {
                crossroad_cnt[1]++;
            }
            // 十字下拐点区域识别
            if(tracking.Get_Corner(LEFT_DOWN).x > 20 
                && (tracking.Get_Corner(RIGHT_DOWN).x > 20 && tracking.Get_Corner(RIGHT_DOWN).x < tracking.Get_Width() - 20)) // 下拐点存在
            {
                crossroad_cnt[1]++;
            }
        }
        // 十字路口补线
        if(crossroad_cnt[1] >= 2)
        {
            // if(tracking.Get_Corner(LEFT_UP).y > 20 && tracking.Get_Corner(LEFT_DOWN).y > 20)
            // {
            //     _crossroad_left_line = Link_Point_To_Point(tracking.Get_Corner(LEFT_UP),tracking.Get_Corner(LEFT_DOWN));
            // }
            if(tracking.Get_Corner(LEFT_UP).y > 20)
            {
                _crossroad_left_line = Link_Point_To_Point(tracking.Get_Corner(LEFT_UP),tracking.Get_Edge_Left()[0]);
            }
            // if(tracking.Get_Corner(RIGHT_UP).y > 20 && tracking.Get_Corner(RIGHT_DOWN).y > 20)
            // {
            //     _crossroad_right_line = Link_Point_To_Point(tracking.Get_Corner(RIGHT_UP),tracking.Get_Corner(RIGHT_DOWN));
            // }
            if(tracking.Get_Corner(RIGHT_UP).y > 20)
            {
                _crossroad_right_line = Link_Point_To_Point(tracking.Get_Corner(RIGHT_UP),tracking.Get_Edge_Right()[0]);
            }
            return Scene::CrossScene;
        }
        

        // =========================================环岛识别========================================
        // 1.宽度变宽

        // 环岛补线


        // =========================================障碍物识别========================================
        // 1.宽度变窄

        // 2.存在角点
        

        // 障碍物补线

    }
    return Scene::NolmalScene;
}



/**
 * @brief 绘制各种元素补线
 */
void Element::Draw_Edge(Tracking &tracking)
{
    // 绘制左边缘点（蓝色）
    for(size_t i = 0; i < tracking.Get_Edge_Left().size();i++)
    {
        cv::circle(tracking._draw_frame,
                   cv::Point(tracking.Get_Edge_Left()[i].x,tracking.Get_Edge_Left()[i].y),
                   1,  // 圆圈半径
                   cv::Scalar(255,0,0),  // 蓝色 (B,G,R)
                   -1);  // 实心圆
        // 绘制十字补线
        if( i < _crossroad_left_line.size())
        {
            cv::circle(tracking._draw_frame,
                        cv::Point(_crossroad_left_line[i].x,_crossroad_left_line[i].y),
                        1,
                        cv::Scalar(255,0,0),
                        -1);
        }
    }
    
    // 绘制右边缘点（红色）
    for(size_t i = 0; i < tracking.Get_Edge_Right().size();i++)
    {
        cv::circle(tracking._draw_frame,
                   cv::Point(tracking.Get_Edge_Right()[i].x,tracking.Get_Edge_Right()[i].y),
                   1,  // 圆圈半径
                   cv::Scalar(0,0,255),  // 红色 (B,G,R)
                   -1);  // 实心圆
        // 绘制十字补线
        if( i < _crossroad_right_line.size())
        {
            cv::circle(tracking._draw_frame,
                        cv::Point(_crossroad_right_line[i].x,_crossroad_right_line[i].y),
                        1,
                        cv::Scalar(0,0,255),
                        -1);
        }
    }

    // 绘制角点
    cv::circle(tracking._draw_frame,
               cv::Point(tracking.Get_Corner(LEFT_DOWN).x,tracking.Get_Corner(LEFT_DOWN).y),
               2,  // 圆圈半径
               cv::Scalar(0,255,0),  // 绿色 (B,G,R)
               -1);  // 实心圆
    cv::circle(tracking._draw_frame,
               cv::Point(tracking.Get_Corner(RIGHT_DOWN).x,tracking.Get_Corner(RIGHT_DOWN).y),
               2,  // 圆圈半径
               cv::Scalar(0,255,0),  // 绿色 (B,G,R)
               -1);  // 实心圆
    cv::circle(tracking._draw_frame,
               cv::Point(tracking.Get_Corner(LEFT_UP).x,tracking.Get_Corner(LEFT_UP).y),
               2,  // 圆圈半径
               cv::Scalar(0,255,255),  // 绿色 (B,G,R)
               -1);  // 实心圆
    cv::circle(tracking._draw_frame,
               cv::Point(tracking.Get_Corner(RIGHT_UP).x,tracking.Get_Corner(RIGHT_UP).y),
               2,  // 圆圈半径
               cv::Scalar(0,255,255),  // 绿色 (B,G,R)
               -1);  // 实心圆
    debug << "左边斜率：" << tracking.Get_Edge_Left()[tracking.Get_Height()/2].slope << std::endl;
    debug << "右边斜率：" << tracking.Get_Edge_Right()[tracking.Get_Height()/2].slope << std::endl;
}

}