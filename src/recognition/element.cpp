#include "recognition/element.hpp"
#include "common.hpp"

using namespace common;
using namespace std;

namespace recognition
{

Element::Element()
{
    _crossroad_flag = false;
    _ring_flag = false;
    _bridge_flag = false;
    _obstaclee_flag = false;
    _zebra_flag = false;
}

Element::~Element()
{

}

Scene Element::Recognition_Element(Tracking &tracking)
{
    uint16_t zebra_cnt = 0;//斑马线计数
    uint8_t zebra_flag = 0;//斑马线识别标志

    uint16_t crossroad_cnt = 0;//十字路口计数
    uint8_t crossroad_flag = 0;//十字路口识别标志

    uint16_t ring_cnt = 0;//环岛计数
    uint8_t ring_flag = 0;//环岛识别标志

    uint16_t obstacle_cnt = 0;//障碍物计数
    uint8_t obstacle_flag = 0;//障碍物识别标志

    uint16_t valid_row = tracking.Get_Valid_Row();

    for(int i = 0;i < valid_row;i++)
    {
        //======================================斑马线识别========================================
        // 1.处于中间区域
        if(i > tracking.Get_Height() / 3 && i < tracking.Get_Height() / 3 * 2)
        {
            // 2.赛道宽度合理
            for(int j = tracking.Get_Edge_Left()[i].x;j < tracking.Get_Edge_Right()[i].x;j++)
            {   
                if(tracking.Get_Width_Block()[i] < tracking.Get_Width() * 0.7)
                {
                    // 3.满足连续跳变
                    if((tracking._camera.Get_Binary_Frame().at<uchar>(i,j) == WHITE 
                    && tracking._camera.Get_Binary_Frame().at<uchar>(i,j+1) == BLACK)
                    || (tracking._camera.Get_Binary_Frame().at<uchar>(i,j) == BLACK 
                    && tracking._camera.Get_Binary_Frame().at<uchar>(i,j+1) == WHITE))
                    {
                        zebra_cnt++;
                        if(zebra_cnt > 40)
                        {
                            _zebra_flag = true; //斑马线识别成功
                            return Scene::ZebraScene;
                        }
                    }
                }
            }
        }
        //======================================十字路口识别========================================
        // 1.赛道存在宽度突变
        if(tracking.Get_Width_Block()[i] > tracking.Get_Width() * 0.9)
        {
            crossroad_cnt++;
            crossroad_flag = 1;

        }
        // 2.存在角点
        if(crossroad_flag == 1)
        {
            // 十字上拐点区域识别
            if(tracking.Get_Corner(LEFT_UP).x > 20 
                || (tracking.Get_Corner(RIGHT_UP).x > 20 
                && tracking.Get_Corner(RIGHT_UP).x < tracking.Get_Width() - 20)) // 上拐点存在
            {

            }
            // 十字下拐点区域识别
            if(tracking.Get_Corner(LEFT_DOWN).x > 20 
                || (tracking.Get_Corner(RIGHT_DOWN).x > 20 
                && tracking.Get_Corner(RIGHT_DOWN).x < tracking.Get_Width() - 20)) // 下拐点存在
            {

            }
        }
        // 十字路口补线

        // =========================================环岛识别========================================
        // 1.宽度变宽

        // 环岛补线


        // =========================================障碍物识别========================================
        // 1.宽度变窄

        // 2.存在角点
        

        // 障碍物补线

    }

}

}