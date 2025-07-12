#pragma once

#include <string>
#include <opencv2/opencv.hpp>

namespace common{

#define WHITE 255
#define BLACK 0

/**
 * @brief 场景类型
 */
enum class Scene
{
    NormalScene = 0, // 基础赛道
    CrossScene,      // 十字道路
    RingScene,       // 环岛道路
    BridgeScene,     // 坡道区
    ObstacleScene,   // 障碍区
    CateringScene,   // 快餐店
    LaybyScene,      // 临时停车区
    ParkingScene,    // 停车区
    StopScene        // 停车（结束）
};

/**
 * @brief 点结构体
 */
struct POINT
{
    int x;           // x坐标
    int y;           // y坐标
    float slope;     // 斜率
    
    // 默认构造函数
    POINT() : x(0), y(0), slope(0.0f) {}
    
    // 带参数的构造函数
    POINT(int x, int y) : x(x), y(y), slope(0.0f) {}
    
    // 完整构造函数
    POINT(int x, int y, float slope) : x(x), y(y), slope(slope) {}

    // 拷贝构造函数
    POINT(const POINT& point){x = point.x, y = point.y, slope = point.slope;}

    //赋值运算符重载
    POINT& operator=(const POINT& point){x = point.x, y = point.y, slope = point.slope;return *this;}
    // 加法运算符重载
    POINT& operator+=(const POINT& rhs) { x += rhs.x; y += rhs.y; return *this; }
    // 减法运算符重载
    POINT& operator-=(const POINT& rhs) { x -= rhs.x; y -= rhs.y; return *this; }
    // 除法运算符重载（用于平均）
    POINT operator/(int val) const { return POINT(x / val, y / val); }
};





// enum class JUNCTION_TYPE
// {
//     L_UP,   //左侧上拐点——上、左
//     L_DOWN, //左侧下拐点——左、上
//     R_UP,   //右侧上拐点——上、右
//     R_DOWN, //右侧下拐点——右、上
// };

// /**
//  * @brief 拐点结构体
//  */
// struct JUNCTION
// {
//     int x;           // x坐标
//     int y;           // y坐标
//     float slope_left; // 左斜率
//     float slope_right; // 右斜率
//                     //拐点类型
//     // 默认构造函数
//     JUNCTION() : x(0), y(0), slope_left(0.0f), slope_right(0.0f), slope(0.0f) {}
    
//     // 带参数的构造函数
//     POINT(int x, int y) : x(x), y(y), slope(0.0f) {}
    
//     // 完整构造函数
//     POINT(int x, int y, float slope) : x(x), y(y), slope(slope) {}
// };

} // namespace common