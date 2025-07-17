#include "control/controlcenter.hpp"

using namespace common;
using namespace recognition;
using namespace std;

namespace control{


/**
 * @brief 计算中心点，用于单边情况
 * @param points_edge 边缘点
 * @param side 单边情况
 * @param tracking 跟踪器
 * @return 中心点
 */
vector<POINT> Center_Compute(vector<POINT> points_edge,int side,Tracking& tracking)
{
    int step = 4;   // 步长
    int off_set_width = tracking.Get_Width() / 2;   //首行偏移量
    int off_set_height = 0;     //纵向偏移量
    int border = 2;  // 边框宽度，从配置文件获取
    vector<POINT> center_v;
    
    if(side == 0)   //左单边可见
    {
        uint16_t counter = 0, rowStart = 0;
        for(size_t i = 0; i < points_edge.size(); i++)
        {
            // 修复：考虑边框，检查点是否在有效区域内
            if(points_edge[i].x > border)
            {
                counter ++;
                if(counter > 2)
                {
                    rowStart = i - 2;
                    break;
                }
                else
                    counter = 0;
            }
        }
        // 计算纵向偏移量
        off_set_height = points_edge[rowStart].y - points_edge[0].y;
        counter = 0;
        // 从有效行开始，每隔step个点计算一个中心点
        for(size_t i = rowStart; i < points_edge.size();i+=step)
        {
            int px = points_edge[i].x + off_set_width;
            // 修复：考虑边框的边界检查
            if(px > tracking.Get_Width() - border - 1)
            {
                counter ++;
                if(counter > 2)
                    break;
            }
            else
            {
                counter = 0;
                // 修复：POINT构造函数参数顺序应该是(x, y)，不是(y, x)
                center_v.emplace_back(px, points_edge[i].y - off_set_height);
            }
        }
    }
    else if(side == 1)   //右单边可见
    {
        uint16_t counter = 0, rowStart = 0;
        for(size_t i = 0; i < points_edge.size(); i++)
        {
            // 修复：考虑边框，检查点是否在有效区域内
            if(points_edge[i].x < tracking.Get_Width() - border - 1)
            {
                counter ++;
                if(counter > 2)
                {
                    rowStart = i - 2;
                    break;
                }
                else
                    counter = 0;
            }
        }
        // 计算纵向偏移量
        off_set_height = points_edge[rowStart].y - points_edge[0].y;
        counter = 0;
        // 从有效行开始，每隔step个点计算一个中心点
        for(size_t i = rowStart; i < points_edge.size();i+=step)
        {
            int px = tracking.Get_Width() - points_edge[i].x - off_set_width;
            // 修复：考虑边框的边界检查
            if(px < border + 1)
            {
                counter ++;
                if(counter > 2)
                    break;
            }
            else
            {
                counter = 0;
                // 修复：POINT构造函数参数顺序应该是(x, y)，不是(y, x)
                center_v.emplace_back(px, points_edge[i].y - off_set_height);
            }
        }
    }
    return center_v;
}

void ControlCenter::Fitting(Tracking& tracking,Element& element)
{
    _sigma_center = 0;
    _control_center = tracking.Get_Width() / 2;
    _center_edge.clear();
    vector<POINT> v_center(4);
    _style = "STRAIGHT";

    // ========================================================== 计算赛贝尔曲线控制点 ==========================================================
    // 双边情况
    if(tracking.Get_Edge_Left().size() > 20 && tracking.Get_Edge_Right().size() > 20
        && tracking.Get_Lost_Left().size() < 15 && tracking.Get_Lost_Right().size() < 15)

    {
        //起点位置中心点
        v_center[0] = {
            (element._left_line[0].x + element._right_line[0].x) / 2,
            (element._left_line[0].y + element._right_line[0].y) / 2};
        //1/3位置中心点
        v_center[1] = {
            (element._left_line[element._left_line.size() / 3].x + element._right_line[element._right_line.size() / 3].x) / 2,
            (element._left_line[element._left_line.size() / 3].y + element._right_line[element._right_line.size() / 3].y) / 2};
        //2/3位置中心点
        v_center[2] = {
            (element._left_line[element._left_line.size() * 2 / 3].x + element._right_line[element._right_line.size() * 2 / 3].x) / 2,
            (element._left_line[element._left_line.size() * 2 / 3].y + element._right_line[element._right_line.size() * 2 / 3].y) / 2};
        //90%位置中心点
        v_center[3] = {
            (element._left_line[element._left_line.size() * 9 / 10].x + element._right_line[element._right_line.size() * 9 / 10].x) / 2,
            (element._left_line[element._left_line.size() * 9 / 10].y + element._right_line[element._right_line.size() * 9 / 10].y) / 2};
        
        //使用贝塞尔曲线拟合平滑中心线，步长0.03表示曲线平滑度
        _center_edge = Bazier(0.03,v_center);
        _style = "STRAIGHT";
    }
    // // 左单边情况
    // else if(tracking.Get_Edge_Left().size() > 20 && tracking.Get_Lost_Left().size() < 10
    //     && tracking.Get_Edge_Right().size() < 20 && tracking.Get_Lost_Right().size() > 10
    // {
    // }
    //左单边情况
    // else if(tracking.Get_Lost_Right() > 10 && tracking.Get_Lost_Left() < 10)
    // {

    // }
    // //右单边情况
    // else if()
    // {

    // }
    else    //暂时都以双边策略
    {
        //起点位置中心点
        v_center[0] = {
            (element._left_line[0].x + element._right_line[0].x) / 2,
            (element._left_line[0].y + element._right_line[0].y) / 2};
        //1/3位置中心点
        v_center[1] = {
            (element._left_line[element._left_line.size() / 3].x + element._right_line[element._right_line.size() / 3].x) / 2,
            (element._left_line[element._left_line.size() / 3].y + element._right_line[element._right_line.size() / 3].y) / 2};
        //2/3位置中心点
        v_center[2] = {
            (element._left_line[element._left_line.size() * 2 / 3].x + element._right_line[element._right_line.size() * 2 / 3].x) / 2,
            (element._left_line[element._left_line.size() * 2 / 3].y + element._right_line[element._right_line.size() * 2 / 3].y) / 2};
        //90%位置中心点
        v_center[3] = {
            (element._left_line[element._left_line.size() * 9 / 10].x + element._right_line[element._right_line.size() * 9 / 10].x) / 2,
            (element._left_line[element._left_line.size() * 9 / 10].y + element._right_line[element._right_line.size() * 9 / 10].y) / 2};
        
        //使用贝塞尔曲线拟合平滑中心线，步长0.03表示曲线平滑度
        _center_edge = Bazier(0.03,v_center);
        _style = "STRAIGHT";
    }

    // ========================================================== 加权控制中心算法 ==========================================================
    int control_num = 1;    // 权重累加器(关注远端)
    for(auto p:_center_edge)
    {
        if(p.y < tracking.Get_Height() / 2) //在图像上半部分
        {
            control_num += tracking.Get_Height() / 2;
            _control_center += p.x * tracking.Get_Height() / 2;
        }
        else    //在图像下半部分
        {
            control_num += tracking.Get_Height() - p.y; //权重为距底部的距离
            _control_center += p.x * (tracking.Get_Height() - p.y);
        }
    }
    //计算加权平均值
    if(control_num > 0)
    {
        _control_center = _control_center / control_num;
    }
    if(_control_center > tracking.Get_Width())
    {
        _control_center = tracking.Get_Width() - 1;
    }
    else if(_control_center < 0)
    {
        _control_center = 0;
    }
    // 评估路径平滑度
    if(_center_edge.size() > 20)
    {
        vector<POINT> center_v;
        int filt = _center_edge.size() / 5; //过滤前后1/5的点
        for(size_t i = filt; i < _center_edge.size() - filt; i++)
        {
            center_v.push_back(_center_edge[i]);
        }
        _sigma_center = Variance<POINT,vector<POINT>>(center_v,Point_Avg,Point_Ddot);
    }
    else
    {
        _sigma_center = 1000;
    }
}

}