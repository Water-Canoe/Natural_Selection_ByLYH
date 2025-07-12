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

    uint8_t zebra_cnt[2] = {0,0};//斑马线计数
    // uint8_t zebra_flag = 0;//斑马线识别标志  // 暂时注释掉未使用的变量

    uint8_t crossroad_cnt[2] = {0,0};//十字路口计数
    // uint8_t crossroad_flag = 0;//十字路口识别标志  // 暂时注释掉未使用的变量

    uint8_t obstacle_cnt[4] = {0,0,0,0};//障碍物计数
    // uint8_t obstacle_flag = 0;//障碍物识别标志  // 暂时注释掉未使用的变量

    // uint16_t ring_cnt = 0;//环岛计数  // 暂时注释掉未使用的变量
    // uint8_t ring_flag = 0;//环岛识别标志  // 暂时注释掉未使用的变量

    // 更新补线信息
    _crossroad_left_line.clear();
    _crossroad_right_line.clear();
    _obstacle_left_line.clear();
    _obstacle_right_line.clear();

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
                if(tracking.Get_Width_Block()[i] < tracking.Get_Width() * 0.7 && tracking.Get_Width_Block()[i] > tracking.Get_Width() * 0.6)
                {
                    // 3.满足连续跳变
                    if(tracking._camera.Get_Binary_Frame().at<uchar>(i,j) == WHITE 
                    && tracking._camera.Get_Binary_Frame().at<uchar>(i,j+1) == BLACK)
                    {   //白变黑
                        zebra_cnt[0]++;
                    }
                    if(tracking._camera.Get_Binary_Frame().at<uchar>(i,j) == BLACK 
                    && tracking._camera.Get_Binary_Frame().at<uchar>(i,j+1) == WHITE)
                    {   //黑变白
                        zebra_cnt[1]++;
                    }
                    if(zebra_cnt[1] > 60 && zebra_cnt[0] > 60 && (zebra_cnt[0]/zebra_cnt[1]>0.85 || zebra_cnt[1]/zebra_cnt[0]>0.85))
                    {
                        return Scene::ZebraScene;
                    }
                }
            }
        }
        //======================================十字路口识别========================================
        // 1.赛道存在宽度突变
        if(i> 20 && tracking.Get_Width_Block()[i] > tracking.Get_Width() * 0.95)
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
            if(tracking.Get_Corner(LEFT_UP).y < tracking.Get_Corner(LEFT_DOWN).y && tracking.Get_Corner(LEFT_UP).y > 20)  //上下角点都存在，且上角点在上
            {
                _crossroad_left_line = Link_Point_To_Point(tracking.Get_Corner(LEFT_DOWN),tracking.Get_Corner(LEFT_UP));
            }
            else if(tracking.Get_Corner(LEFT_UP).y > tracking.Get_Corner(LEFT_DOWN).y && tracking.Get_Corner(LEFT_DOWN).y > 20)  //上下角点都存在，且上角点在下
            {
                float slope = Slope_Point_To_Point(tracking.Get_Corner(LEFT_DOWN), tracking.Get_Corner(LEFT_UP));
                int y_distance = tracking.Get_Corner(LEFT_UP).y - tracking.Get_Edge_Left()[0].y;
                _crossroad_left_line = Link_Point_Y_Slope(tracking.Get_Corner(LEFT_UP), slope, y_distance);
            }
            else if(tracking.Get_Corner(LEFT_UP).y > 20 && tracking.Get_Corner(LEFT_DOWN).y <= 20)   // 上角点存在，下角点不存在
            {
                _crossroad_left_line = Link_Point_To_Point(tracking.Get_Edge_Left()[0],tracking.Get_Corner(LEFT_UP));
            }
            else if(tracking.Get_Corner(LEFT_UP).y <= 20 && tracking.Get_Corner(LEFT_DOWN).y > 20)   // 上角点不存在，下角点存在
            {
                _crossroad_left_line = Link_Point_Y_Slope(tracking.Get_Corner(LEFT_DOWN),
                                                            tracking.Get_Edge_Left()[(tracking.Get_Corner(LEFT_DOWN).y - 4)].slope,
                                                            tracking.Get_Corner(LEFT_DOWN).y - 4 - tracking.Get_Edge_Left()[0].y);
            }
            
            if(tracking.Get_Corner(RIGHT_UP).y < tracking.Get_Corner(RIGHT_DOWN).y && tracking.Get_Corner(RIGHT_UP).y > 20)    //上下角点都存在
            {
                _crossroad_right_line = Link_Point_To_Point(tracking.Get_Corner(RIGHT_DOWN),tracking.Get_Corner(RIGHT_UP));
            }
            else if(tracking.Get_Corner(RIGHT_UP).y > tracking.Get_Corner(RIGHT_DOWN).y && tracking.Get_Corner(RIGHT_DOWN).y > 20)
            {
                float slope = Slope_Point_To_Point(tracking.Get_Corner(RIGHT_DOWN), tracking.Get_Corner(RIGHT_UP));
                int y_distance = tracking.Get_Corner(RIGHT_UP).y - tracking.Get_Edge_Right()[0].y;
                _crossroad_right_line = Link_Point_Y_Slope(tracking.Get_Corner(RIGHT_UP), slope, y_distance);
            }
            else if(tracking.Get_Corner(RIGHT_UP).y > 20 && tracking.Get_Corner(RIGHT_DOWN).y <= 20)   // 上角点存在，下角点不存在
            {
                _crossroad_right_line = Link_Point_To_Point(tracking.Get_Edge_Right()[0],tracking.Get_Corner(RIGHT_UP));
            }
            else if(tracking.Get_Corner(RIGHT_UP).y <= 20 && tracking.Get_Corner(RIGHT_DOWN).y > 20)   // 上角点不存在，下角点存在
            {
                _crossroad_right_line = Link_Point_Y_Slope(tracking.Get_Corner(RIGHT_DOWN),
                                                            tracking.Get_Edge_Right()[(tracking.Get_Corner(RIGHT_DOWN).y - 4)].slope,
                                                            tracking.Get_Corner(RIGHT_DOWN).y - 4 - tracking.Get_Edge_Right()[0].y);
            }
            return Scene::CrossScene;
        }
        
        // =========================================障碍物识别========================================
        // 1.宽度变窄
        if(tracking.Get_Width_Block()[i] < tracking.Get_Width_Block()[i-5] * 0.8)
        {
            obstacle_cnt[0]++;
        }
        if(obstacle_cnt[0] > 4)
        {
            if(i>20 && tracking.Get_Width_Block()[i] < tracking.Get_Width() * 0.6
            && tracking.Get_Edge_Left()[i].x > 20 && tracking.Get_Edge_Right()[i].x < tracking.Get_Width() - 20)
            {
                obstacle_cnt[1]++;
            }
        }
        // 2.存在角点（一侧有一侧无）
        if(obstacle_cnt[1] >= 20)
        {
            if(tracking.Get_Corner(LEFT_DOWN).y > 30 && tracking.Get_Corner(LEFT_UP).y > 30
                && tracking.Get_Corner(LEFT_DOWN).x > 30 && tracking.Get_Corner(LEFT_UP).x > 30
                && tracking.Get_Corner(LEFT_DOWN).x *1.0f / tracking.Get_Corner(LEFT_UP).x < 1.1 && tracking.Get_Corner(LEFT_DOWN).x *1.0f / tracking.Get_Corner(LEFT_UP).x > 0.9)
            {
                obstacle_cnt[2]++;
            }
            if(tracking.Get_Corner(RIGHT_DOWN).y > 30 && tracking.Get_Corner(RIGHT_UP).y > 30
                && tracking.Get_Corner(RIGHT_DOWN).x < tracking.Get_Width() - 30 && tracking.Get_Corner(RIGHT_UP).x < tracking.Get_Width() - 30
                && tracking.Get_Corner(RIGHT_DOWN).x *1.0f / tracking.Get_Corner(RIGHT_UP).x < 1.1 && tracking.Get_Corner(RIGHT_DOWN).x *1.0f / tracking.Get_Corner(RIGHT_UP).x > 0.9)
            {
                obstacle_cnt[3]++;
            }
        }
        // 障碍物补线生成 - 修复逻辑，让左右两边都能被检测
        if(obstacle_cnt[2] > 0) // 左障碍物
        {
            // 计算贝塞尔控制点
            vector<POINT> left_bezier(3);
            left_bezier[0] = tracking.Get_Edge_Left()[2];   //起点
            left_bezier[1] = {
                (tracking.Get_Edge_Left()[2].x + tracking.Get_Corner(LEFT_UP).x)* 2 / 3,   // 偏右1/3点
                (tracking.Get_Edge_Left()[2].y + tracking.Get_Corner(LEFT_UP).y) / 2};
            left_bezier[2] = tracking.Get_Corner(LEFT_UP);  //终点
            _obstacle_left_line = Bazier(1.0f / abs(left_bezier[0].y - left_bezier[2].y),left_bezier);
        }
        if(obstacle_cnt[3] > 0) // 右障碍物
        {
            vector<POINT> right_bezier(3);
            right_bezier[0] = tracking.Get_Edge_Right()[2];   //起点
            right_bezier[1] = {
                (tracking.Get_Edge_Right()[2].x + tracking.Get_Corner(RIGHT_UP).x) / 3,   // 偏左1/3点
                (tracking.Get_Edge_Right()[2].y + tracking.Get_Corner(RIGHT_UP).y) / 2};
            right_bezier[2] = tracking.Get_Corner(RIGHT_UP);  //终点
            _obstacle_right_line = Bazier(1.0f / abs(right_bezier[0].y - right_bezier[2].y),right_bezier);
        }
        // 如果任一边检测到障碍物，返回障碍物场景
        if(obstacle_cnt[2] > 0 || obstacle_cnt[3] > 0)
        {
            return Scene::ObstacleScene;
        }

        // =========================================环岛识别========================================

        

        // 障碍物补线

    }
    return Scene::NolmalScene;
}



/**
 * @brief 应用补线到边线中
 * @param current_y 当前边线点的Y坐标
 * @param supplement_line 补线点集
 * @param supplement_index 补线索引（引用，会被更新）
 * @return 是否成功应用补线
 */
bool Element::Apply_Supplement_Line(int current_y, const std::vector<common::POINT>& supplement_line, int& supplement_index)
{
    if(supplement_line.size() == 0 || supplement_index >= static_cast<int>(supplement_line.size()))
        return false;
    int best_index = supplement_index;
    int min_diff = abs(current_y - supplement_line[supplement_index].y);
    for(int j = supplement_index; j < static_cast<int>(supplement_line.size()) && j < supplement_index + 5; j++)
    {
        int diff = abs(current_y - supplement_line[j].y);
        if(diff < min_diff)
        {
            min_diff = diff;
            best_index = j;
        }
    }
    if(min_diff <= 2)
    {
        supplement_index = best_index + 1;
        return true;
    }
    return false;
}

float Element::Get_Middle_Error(Tracking &tracking)
{
    _left_line.clear();
    _right_line.clear();
    _middle_line.clear();
    _middle_error = 0;
    
    // 将元素补线融合到边线中
    int crossroad_left_index = 0;
    int crossroad_right_index = 0;
    int obstacle_left_index = 0;
    int obstacle_right_index = 0;

    for(int i = 0;i < tracking.Get_Valid_Row();i++)
    {
        _left_line.push_back(tracking.Get_Edge_Left()[i]);
        _right_line.push_back(tracking.Get_Edge_Right()[i]);
        //================================十字补线========================================
        if(Apply_Supplement_Line(_left_line[i].y, _crossroad_left_line, crossroad_left_index))
        {
            _left_line[i] = _crossroad_left_line[crossroad_left_index-1];
        }
        if(Apply_Supplement_Line(_right_line[i].y, _crossroad_right_line, crossroad_right_index))
        {
            _right_line[i] = _crossroad_right_line[crossroad_right_index-1];
        }
        //================================障碍物补线========================================
        if(Apply_Supplement_Line(_left_line[i].y, _obstacle_left_line, obstacle_left_index))
        {
            _left_line[i] = _obstacle_left_line[obstacle_left_index-1];
        }
        if(Apply_Supplement_Line(_right_line[i].y, _obstacle_right_line, obstacle_right_index))
        {
            _right_line[i] = _obstacle_right_line[obstacle_right_index-1];
        }

        //================================中间线计算========================================
        // 计算当前行的中间线位置
        _middle_line.push_back({(_left_line[i].x + _right_line[i].x) / 2, _left_line[i].y});
        
        // 只在有效区域内计算误差（避免边缘区域的影响）
        if(i > tracking._camera.Get_Row_Cut_Bottom() && i < tracking.Get_Height() - tracking._camera.Get_Row_Cut_Up())
        {
            // 计算当前中间线相对于图像中心的偏差
            // 正值表示中间线在中心右侧（需要向左调整）
            // 负值表示中间线在中心左侧（需要向右调整）
            float current_error = _middle_line[i].x - tracking.Get_Width() / 2.0f;
            
            // 根据距离远近给予不同权重（近处权重更大，远处权重更小）
            float weight = 1.0f - (float)i / tracking.Get_Height(); // 越近权重越大
            _middle_error += current_error * weight;
        }
    }
    // 计算有效区域的行数
    int valid_rows = tracking.Get_Height() - tracking._camera.Get_Row_Cut_Bottom() - tracking._camera.Get_Row_Cut_Up();
    // 计算平均误差
    float average_error = _middle_error / valid_rows;
    // 添加调试信息
    debug << "中间线误差：" << _middle_error << "，平均误差：" << average_error << std::endl;
    debug << "有效行数：" << valid_rows << "，图像中心：" << tracking.Get_Width() / 2 << std::endl;
    return average_error;
}


/**
 * @brief 绘制各种元素补线
 */
void Element::Draw_Edge(Tracking &tracking)
{
    // // 绘制左边缘点（蓝色）
    // for(size_t i = 0; i < tracking.Get_Edge_Left().size();i++)
    // {
    //     cv::circle(tracking._draw_frame,
    //                cv::Point(tracking.Get_Edge_Left()[i].x,tracking.Get_Edge_Left()[i].y),
    //                1,  // 圆圈半径
    //                cv::Scalar(255,0,0),  // 蓝色 (B,G,R)
    //                -1);  // 实心圆
    //     // 绘制十字补线
    //     if( i < _crossroad_left_line.size())
    //     {
    //         cv::circle(tracking._draw_frame,
    //                     cv::Point(_crossroad_left_line[i].x,_crossroad_left_line[i].y),
    //                     1,
    //                     cv::Scalar(255,0,0),
    //                     -1);
    //     }
    // }
    // // 绘制右边缘点（红色）
    // for(size_t i = 0; i < tracking.Get_Edge_Right().size();i++)
    // {
    //     cv::circle(tracking._draw_frame,
    //                cv::Point(tracking.Get_Edge_Right()[i].x,tracking.Get_Edge_Right()[i].y),
    //                1,  // 圆圈半径
    //                cv::Scalar(0,0,255),  // 红色 (B,G,R)
    //                -1);  // 实心圆
    //     // 绘制十字补线
    //     if( i < _crossroad_right_line.size())
    //     {
    //         cv::circle(tracking._draw_frame,
    //                     cv::Point(_crossroad_right_line[i].x,_crossroad_right_line[i].y),
    //                     1,
    //                     cv::Scalar(0,0,255),
    //                     -1);
    //     }
    // }
    for(size_t i = 0;i < _middle_line.size();i++)
    {
        cv::circle(tracking._draw_frame,
                   cv::Point(_middle_line[i].x,_middle_line[i].y),
                   1,  // 圆圈半径
                   cv::Scalar(0,255,255),  // 绿色 (B,G,R)
                   -1);  // 实心圆
        cv::circle(tracking._draw_frame,
                   cv::Point(_left_line[i].x,_left_line[i].y),
                   1,  // 圆圈半径
                   cv::Scalar(255,0,0),  // 蓝色 (B,G,R)
                   -1);  // 实心圆
        cv::circle(tracking._draw_frame,
                   cv::Point(_right_line[i].x,_right_line[i].y),
                   1,  // 圆圈半径
                   cv::Scalar(0,0,255),  // 红色 (B,G,R)
                   -1);  // 实心圆
    }

    // cv::line(tracking._draw_frame,
    //          cv::Point(0,tracking.Get_Height()/3),
    //          cv::Point(tracking.Get_Width(),tracking.Get_Height()/3),
    //          cv::Scalar(0,255,255),
    //          1);
    // cv::line(tracking._draw_frame,
    //          cv::Point(0,tracking.Get_Height()/3*2),
    //          cv::Point(tracking.Get_Width(),tracking.Get_Height()/3*2),
    //          cv::Scalar(0,255,255),
    //          1);

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