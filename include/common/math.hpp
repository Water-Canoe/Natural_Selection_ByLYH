#pragma once

#include <iostream>
#include <vector>
#include <cmath>
#include <opencv2/opencv.hpp>
#include "common/type.hpp"

namespace common{

    // 计算点的平均值
    cv::Point2i Point_Avg(const cv::Point2i& pt, int val);
    // 计算两个点的点积
    double Point_Ddot(const cv::Point2i& pt1,const cv::Point2i& pt2);

    // 计算集合的平均值
    template<typename _Tp,typename _ForwardIterator>
    auto Average(_ForwardIterator begin, _ForwardIterator end)
    {
        if(begin == end) return _Tp{};
        _Tp val = *begin;
        int cnt = 1;
        ++begin;
        while(begin != end)
        {
            val += *begin;
            ++cnt;
            ++begin;
        }
        return val / cnt;
    }

    template<typename _Tp,typename _ForwardIterator>
    auto Average(_ForwardIterator begin, _ForwardIterator end,_Tp(* _avg)(const _Tp&,int))
    {
        if(begin == end) return _Tp{};
        _Tp val = *begin;
        int cnt = 1;
        ++begin;
        while(begin != end)
        {
            val += *begin;
            ++cnt;
            ++begin;
        }
        return _avg(val,cnt);
    }

    // 计算集合的方差
    template<typename _Tp,typename _Container>
    double Variance(_Container& container)
    {
        _Tp avg = Average<_Tp>(container.begin(),container.end());

        double sigma = 0;
        int cnt = 0;

        for(_Tp item : container)
        {
            item -= avg;
            sigma += pow(item,2);
            ++cnt;
        }
        return sigma / cnt;
    }

    template<typename _Tp,typename _Container>
    double Variance(_Container& container,_Tp(* _avg)(const _Tp&,int),double(* _ddot)(const _Tp&,const _Tp&))
    {
        _Tp avg = Average<_Tp>(container.begin(),container.end(),_avg);

        double sigma = 0;
        int cnt = 0;

        for(_Tp item : container)
        {
            item -= avg;
            sigma += _ddot(item,item);
            ++cnt;
        }
        return sigma / cnt;
    }

    // 计算阶乘
    int Factorial(int n);

    // 贝塞尔曲线
    std::vector<POINT> Bazier(double dt,std::vector<POINT> points);

    // 将double转换为字符串
    auto Double_To_String(double val,int fixed);

    // 计算点到直线的距离
    double Distance_Point_To_Line(POINT a,POINT b,POINT p);

    // 计算两点之间的距离
    double Distance_Point_To_Point(POINT a, POINT b);

    // 计算两点之间的斜率
    float Slope_Point_To_Point(const POINT& p1, const POINT& p2);

    // 连接两个点
    std::vector<POINT>  Link_Point_To_Point(const POINT& p1,const POINT& p2);
}