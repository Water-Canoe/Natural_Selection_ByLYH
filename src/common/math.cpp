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
 * @brief 计算POINT的平均值
 */
POINT Point_Avg(const POINT& pt, int val)
{
    return {pt.x / val, pt.y / val};
}

/**
 * @brief 计算两个POINT的点积
 */
double Point_Ddot(const POINT& pt1, const POINT& pt2)
{
    return pt1.x * pt2.x + pt1.y * pt2.y;
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
 * @brief 计算组合数 C(n,k)
 * @param n 总数
 * @param k 选择数
 * @return 组合数值
 */
inline int Combination(int n, int k)
{
    if (k > n - k) k = n - k;  // 利用对称性优化
    int result = 1;
    for (int i = 0; i < k; ++i) {
        result *= (n - i);
        result /= (i + 1);
    }
    return result;
}

/**
 * @brief 优化的贝塞尔曲线生成算法
 * @param dt 基础步长
 * @param points 控制点
 * @return 贝塞尔曲线点集
 */
std::vector<POINT> Bazier(double dt, std::vector<POINT> points)
{
    std::vector<POINT> output;
    
    if (points.size() < 2) {
        return output;  // 至少需要2个控制点
    }

    int n = points.size() - 1;  // 曲线次数
    
    // 预计算组合数，避免重复计算
    std::vector<int> combinations(n + 1);
    for (int i = 0; i <= n; ++i) {
        combinations[i] = Combination(n, i);
    }
    
    // 自适应步长：根据控制点距离调整
    double total_length = 0;
    for (size_t i = 1; i < points.size(); ++i) {
        total_length += Distance_Point_To_Point(points[i-1], points[i]);
    }
    
    // 根据总长度调整步长，确保生成足够多的点
    double adaptive_dt = std::min(dt, 1.0 / (total_length * 0.1));
    adaptive_dt = std::max(adaptive_dt, 0.001);  // 最小步长限制
    
    double t = 0.0;
    while (t <= 1.0) {
        POINT p = {0, 0};
        
        // 使用预计算的组合数计算伯恩斯坦多项式
        for (int i = 0; i <= n; ++i) {
            double bernstein = combinations[i] * pow(t, i) * pow(1.0 - t, n - i);
            p.x += static_cast<int>(points[i].x * bernstein);
            p.y += static_cast<int>(points[i].y * bernstein);
        }
        
        output.push_back(p);
        t += adaptive_dt;
    }
    
    // 确保包含终点
    if (output.empty() || (output.back().x != points.back().x || output.back().y != points.back().y)) {
        output.push_back(points.back());
    }
    
    return output;
}

/**
 * @brief 使用德卡斯特茹算法的贝塞尔曲线（更高效）
 * @param dt 步长
 * @param points 控制点
 * @return 贝塞尔曲线点集
 */
std::vector<POINT> Bazier_DeCasteljau(double dt, std::vector<POINT> points)
{
    std::vector<POINT> output;
    
    if (points.size() < 2) {
        return output;
    }
    
    double t = 0.0;
    while (t <= 1.0) {
        // 使用德卡斯特茹算法计算贝塞尔曲线上的点
        std::vector<POINT> temp_points = points;
        
        // 递推计算
        for (int level = 1; level < static_cast<int>(points.size()); ++level) {
            for (int i = 0; i < static_cast<int>(points.size()) - level; ++i) {
                temp_points[i].x = static_cast<int>((1 - t) * temp_points[i].x + t * temp_points[i + 1].x);
                temp_points[i].y = static_cast<int>((1 - t) * temp_points[i].y + t * temp_points[i + 1].y);
            }
        }
        
        output.push_back(temp_points[0]);
        t += dt;
    }
    
    // 确保包含终点
    if (output.empty() || (output.back().x != points.back().x || output.back().y != points.back().y)) {
        output.push_back(points.back());
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



/**
 * @brief 连接两点之间的直线
 * @param p1 起点
 * @param p2 终点
 * @return 直线点集
 */
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
    // 使用改进的Bresenham算法，确保包含起点和终点
    int x = p1.x;
    int y = p1.y;
    int x2 = p2.x;
    int y2 = p2.y;
    // 计算步长
    int step_x = (dx > 0) ? 1 : (dx < 0) ? -1 : 0;
    int step_y = (dy > 0) ? 1 : (dy < 0) ? -1 : 0;
    // 添加起点
    points.push_back(POINT(x, y));
    // 如果x方向距离更大，以x为步长
    if (abs(dx) >= abs(dy)) {
        int error = 2 * abs(dy) - abs(dx);
        while (x != x2) {
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
        while (y != y2) {
            y += step_y;
            if (error >= 0) {
                x += step_x;
                error -= 2 * abs(dy);
            }
            error += 2 * abs(dx);
            points.push_back(POINT(x, y));
        }
    }
    // 确保终点被包含（如果还没有的话）
    if (points.back().x != p2.x || points.back().y != p2.y) {
        points.push_back(POINT(p2.x, p2.y));
    }
    return points;
}


/**
 * @brief 根据斜率连接两点之间的直线（Y方向距离）
 * @param p1 起点
 * @param slope 斜率 (dy/dx)
 * @param y_distance Y方向距离（正数向上，负数向下）
 * @return 直线点集
 */
std::vector<POINT> Link_Point_Y_Slope(const POINT& p1, float slope, int y_distance)
{
    int y = abs(y_distance);
    // 检查斜率是否为无穷大（垂直线）
    if (std::abs(slope) > 1000) {  // 假设斜率大于1000为垂直线
        POINT p2 = {p1.x, p1.y + y_distance};
        return Link_Point_To_Point(p1, p2);
    }
    // 计算终点坐标
    // 根据斜率公式：slope = dy/dx，所以 dx = dy/slope
    int dx = static_cast<int>(y / slope);
    // 确保起点和终点顺序正确
    if (y_distance >= 0) {
        POINT p2 = {p1.x + dx, p1.y - y};
        return Link_Point_To_Point(p1, p2);
    } else {
        POINT p2 = {p1.x + dx, p1.y + y};
        return Link_Point_To_Point(p2, p1);
    }
}

/**
 * @brief 根据斜率连接两点之间的直线（X方向距离）
 * @param p1 起点
 * @param slope 斜率 (dy/dx)
 * @param x_distance X方向距离（正数向右，负数向左）
 * @return 直线点集
 */
std::vector<POINT> Link_Point_X_Slope(const POINT& p1, float slope, int x_distance)
{
    int x = abs(x_distance);
    // 检查斜率是否为无穷大（垂直线）
    if (std::abs(slope) > 1000) {
        // 垂直线，X坐标不变
        POINT p2 = {p1.x, p1.y + (x_distance > 0 ? 1 : -1)};
        return Link_Point_To_Point(p1, p2);
    }
    // 计算终点坐标
    // 根据斜率公式：slope = dy/dx，所以 dy = slope * dx
    int dy = static_cast<int>(slope * x);
    // 确保起点和终点顺序正确
    if (x_distance >= 0) {
        POINT p2 = {p1.x - x, p1.y + dy};
        return Link_Point_To_Point(p1, p2);
    } else {
        POINT p2 = {p1.x + x, p1.y + dy};
        return Link_Point_To_Point(p2, p1);
    }
}


}