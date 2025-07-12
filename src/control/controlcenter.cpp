#include "control/controlcenter.hpp"

using namespace common;
using namespace recognition;
using namespace std;

namespace control{


void ControlCenter::Fitting(Tracking& tracking)
{
    _sigma_center = 0;
    _control_center = tracking.Get_Width() / 2;
    _center_edge.clear();
    vector<POINT> v_center(4);
    _style = "STRAIGHT";

    // ========================================================== 计算赛贝尔曲线控制点 ==========================================================
    // 计算赛贝尔曲线控制点(双边情况)
    if(tracking.Get_Edge_Left().size() > 20 && tracking.Get_Edge_Right().size() > 20)
    {
        //起点位置中心点
        v_center[0] = {
            (tracking.Get_Edge_Left()[0].x + tracking.Get_Edge_Right()[0].x) / 2,
            (tracking.Get_Edge_Left()[0].y + tracking.Get_Edge_Right()[0].y) / 2};
        //1/3位置中心点
        v_center[1] = {
            (tracking.Get_Edge_Left()[tracking.Get_Edge_Left().size() / 3].x + tracking.Get_Edge_Right()[tracking.Get_Edge_Right().size() / 3].x) / 2,
            (tracking.Get_Edge_Left()[tracking.Get_Edge_Left().size() / 3].y + tracking.Get_Edge_Right()[tracking.Get_Edge_Right().size() / 3].y) / 2};
        //2/3位置中心点
        v_center[2] = {
            (tracking.Get_Edge_Left()[tracking.Get_Edge_Left().size() * 2 / 3].x + tracking.Get_Edge_Right()[tracking.Get_Edge_Right().size() * 2 / 3].x) / 2,
            (tracking.Get_Edge_Left()[tracking.Get_Edge_Left().size() * 2 / 3].y + tracking.Get_Edge_Right()[tracking.Get_Edge_Right().size() * 2 / 3].y) / 2};
        //90%位置中心点
        v_center[3] = {
            (tracking.Get_Edge_Left()[tracking.Get_Edge_Left().size() * 9 / 10].x + tracking.Get_Edge_Right()[tracking.Get_Edge_Right().size() * 9 / 10].x) / 2,
            (tracking.Get_Edge_Left()[tracking.Get_Edge_Left().size() * 9 / 10].y + tracking.Get_Edge_Right()[tracking.Get_Edge_Right().size() * 9 / 10].y) / 2};
        
        //使用贝塞尔曲线拟合平滑中心线，步长0.03表示曲线平滑度
        _center_edge = Bazier(0.03,v_center);
        _style = "STRAIGHT";
    }

    // ========================================================== 加权控制中心算法 ==========================================================
    int control_num = 1;    // 权重累加器
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