#include "common/math.hpp"
#include <iostream>
#include <vector>
#include <cmath>
#include <opencv2/opencv.hpp>

namespace common{

/**
 * @brief 计算点的平均值
 */
cv::Point2i Point_Avg(const cv::Point2i& pt, int val)
{
    return {pt.x / val,pt.y / val};
}

/**
 * @brief 计算两个点的点积
 */
double Point_Ddot(const cv::Point2i& pt1,const cv::Point2i& pt2)
{
    return pt1.ddot(pt2);
}




/**
 * @brief 计算阶乘
 */
inline int Factorial(int n)
{
    int f = 1;
    for(int i = 1;i <= n;++i)
    {
        f *= i;
    }
    return f;
}



/**
 * @brief 贝塞尔曲线
 * @param dt 步长
 * @param points 控制点
 * @return 贝塞尔曲线点集
 */
inline std::vector<POINT> Bazier(double dt,std::vector<POINT> points)
{
    std::vector<POINT> output;
    double t = 0;
    while(t <= 1)
    {
        POINT p;
        double sumX = 0.0;
        double sumY = 0.0;
        int i = 0;
        int n = points.size() - 1;
        while(i <= n)
        {
            double b = Factorial(n) / (Factorial(i) * Factorial(n - i)) * pow(t,i) * pow(1 - t,n - i);
            sumX += points[i].x * b;
            sumY += points[i].y * b;
            ++i;
        }
        p.x = sumX;
        p.y = sumY;
        output.push_back(p);
        t += dt;
    }
    return output;
}

/**
 * @brief 将double转换为字符串
 * @param val 值
 * @param fixed 小数位数
 * @return 字符串
 */
inline auto Double_To_String(double val,int fixed)
{
    auto str = std::to_string(val);
    return str.substr(0,str.find(".") + fixed + 1);
}

/**
 * @brief 计算点到直线的距离
 * @param a 直线起点
 * @param b 直线终点
 * @param p 点
 * @return 点到直线的距离
 */
inline double Distance_Point_To_Line(POINT a,POINT b,POINT p)
{
    // 计算线段ab的长度
    double ab_distance = sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y));

    // 计算点p到点a的距离
    double ap_distance = sqrt((a.x - p.x) * (a.x - p.x) + (a.y - p.y) * (a.y - p.y));
    
    // 计算点p到点b的距离
    double bp_distance = sqrt((p.x - b.x) * (p.x - b.x) + (p.y - b.y) * (p.y - b.y));

    // 使用海伦公式计算三角形面积
    // 半周长
    double half = (ab_distance + ap_distance + bp_distance) / 2;
    // 三角形面积：sqrt(s*(s-a)*(s-b)*(s-c))
    double area = sqrt(half * (half - ab_distance) * (half - ap_distance) * (half - bp_distance));

    // 点到直线的距离 = 2 * 面积 / 底边长度
    return (2 * area / ab_distance);
}

/**
 * @brief 计算两点之间的距离
 * @param a 点a
 * @param b 点b
 * @return 两点之间的距离
 */
inline double Distance_Point_To_Point(POINT a, POINT b)
{
    return sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y));
}

/**
 * @brief 计算两点之间的斜率
 * @param p1 第一个点
 * @param p2 第二个点
 * @return 斜率值
 */
float Slope_Point_To_Point(const POINT& p1, const POINT& p2)
{
    // 避免除零错误
    if(p1.x == p2.x) {
        return (p1.y > p2.y) ? 255 : -255;  // 垂直线
    }
    // 计算斜率
    float slope = (float)(p1.y - p2.y) / (float)(p1.x - p2.x);
    return slope;
}


std::vector<POINT>  Link_Point_To_Point(const POINT& p1,const POINT& p2)
{
    std::vector<POINT> points;
    
    // 计算两点间的距离
    int dx = p2.x - p1.x;
    int dy = p2.y - p1.y;
    
    // 如果两点重合，直接返回
    if (dx == 0 && dy == 0) {
        points.push_back(p1);
        return points;
    }
    
    // 确定步长方向
    int step_x = (dx > 0) ? 1 : (dx < 0) ? -1 : 0;
    int step_y = (dy > 0) ? 1 : (dy < 0) ? -1 : 0;
    
    // 使用Bresenham算法绘制直线
    int x = p1.x;
    int y = p1.y;
    
    // 添加起点
    points.push_back(POINT(x, y));
    
    // 如果x方向距离更大，以x为步长
    if (abs(dx) >= abs(dy)) {
        int error = 2 * abs(dy) - abs(dx);
        for (int i = 0; i < abs(dx); i++) {
            x += step_x;
            if (error >= 0) {
                y += step_y;
                error -= 2 * abs(dx);
            }
            error += 2 * abs(dy);
            points.push_back(POINT(x, y));
        }
    } else {
        // 如果y方向距离更大，以y为步长
        int error = 2 * abs(dx) - abs(dy);
        for (int i = 0; i < abs(dy); i++) {
            y += step_y;
            if (error >= 0) {
                x += step_x;
                error -= 2 * abs(dy);
            }
            error += 2 * abs(dx);
            points.push_back(POINT(x, y));
        }
    }
    
    return points;
}


}