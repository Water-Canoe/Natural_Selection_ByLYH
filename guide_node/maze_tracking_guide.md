# 迷宫法巡线系统使用指南

## 1. 系统概述

本系统基于迷宫法算法实现智能车赛道边线提取，使用深度优先搜索(DFS)和广度优先搜索(BFS)的混合策略。

## 2. 核心特性

### 2.1 算法特点
- **深度优先搜索(DFS)**：适合复杂路径，能找到最优解
- **广度优先搜索(BFS)**：适合简单路径，保证最短路径
- **方向优先级**：直行 > 右转 > 左转 > 后退
- **路径平滑**：移动平均滤波去除噪声
- **置信度评估**：路径连续性分析

### 2.2 参数配置
```cpp
struct MazeParams {
    int wall_threshold = 50;      // 墙壁阈值（黑色像素值）
    int path_threshold = 200;     // 路径阈值（白色像素值）
    int max_search_depth = 100;   // 最大搜索深度
    int min_path_width = 3;       // 最小路径宽度
    bool use_dfs = true;          // 是否使用DFS
};
```

## 3. 使用流程

### 3.1 基本使用
```cpp
#include "tracking.hpp"

// 在Tracking类中调用
void Tracking::Track_Recognition() {
    if (!Find_Start_Point()) {
        return;
    }
    
    // 系统会自动执行迷宫法巡线
    // 结果会显示在图像上并输出到控制台
}
```

### 3.2 参数调整
```cpp
// 创建迷宫法系统实例
MazeTrackingSystem maze_system;

// 设置参数
MazeTrackingSystem::MazeParams params;
params.wall_threshold = 30;      // 更严格的墙壁检测
params.path_threshold = 180;     // 更宽松的路径检测
params.max_search_depth = 150;   // 增加搜索深度
params.use_dfs = false;          // 使用BFS

maze_system.SetParams(params);
```

## 4. 算法原理

### 4.1 迷宫法核心思想
```
1. 将赛道图像视为迷宫
2. 黑色区域 = 墙壁（不可通行）
3. 白色区域 = 路径（可通行）
4. 从起点开始搜索到终点的路径
```

### 4.2 搜索策略
```
优先级顺序：
1. 直行（保持当前方向）
2. 右转（顺时针90度）
3. 左转（逆时针90度）
4. 后退（反向180度）
```

### 4.3 方向定义
```cpp
// 方向数组：上右下左 (顺时针)
const int dir[4][2] = {
    {0,-1},  // 上
    {1,0},   // 右
    {0,1},   // 下
    {-1,0}   // 左
};
```

## 5. 图像预处理

### 5.1 预处理步骤
1. **灰度转换**：彩色图像转灰度
2. **高斯滤波**：去除噪声
3. **自适应二值化**：适应不同光照条件
4. **形态学操作**：去除小噪点

### 5.2 预处理代码
```cpp
cv::Mat PreprocessImage(const cv::Mat& input) {
    cv::Mat processed;
    
    // 转换为灰度图
    if (input.channels() == 3) {
        cv::cvtColor(input, processed, cv::COLOR_BGR2GRAY);
    } else {
        processed = input.clone();
    }
    
    // 高斯滤波去噪
    cv::GaussianBlur(processed, processed, cv::Size(5, 5), 0);
    
    // 自适应二值化
    cv::adaptiveThreshold(processed, processed, 255, 
                        cv::ADAPTIVE_THRESH_GAUSSIAN_C, 
                        cv::THRESH_BINARY, 11, 2);
    
    // 形态学操作
    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
    cv::morphologyEx(processed, processed, cv::MORPH_OPEN, kernel);
    
    return processed;
}
```

## 6. 路径分析

### 6.1 路径长度计算
```cpp
double total_length = 0;
for (size_t i = 1; i < path.size(); i++) {
    double dx = path[i].x - path[i-1].x;
    double dy = path[i].y - path[i-1].y;
    total_length += sqrt(dx*dx + dy*dy);
}
```

### 6.2 曲率计算
```cpp
vector<double> curvatures;
for (size_t i = 1; i < path.size() - 1; i++) {
    double angle = atan2(path[i+1].y - path[i].y, path[i+1].x - path[i].x) -
                  atan2(path[i].y - path[i-1].y, path[i].x - path[i-1].x);
    curvatures.push_back(abs(angle));
}
```

### 6.3 置信度评估
```cpp
double GetPathConfidence() const {
    // 计算路径的连续性
    int continuous_count = 0;
    for (size_t i = 1; i < track_path.size(); i++) {
        int dx = track_path[i].x - track_path[i-1].x;
        int dy = track_path[i].y - track_path[i-1].y;
        if (abs(dx) <= 1 && abs(dy) <= 1) {
            continuous_count++;
        }
    }
    
    return (double)continuous_count / (track_path.size() - 1);
}
```

## 7. 可视化输出

### 7.1 绘制元素
- **绿色圆点**：原始路径点
- **蓝色线条**：平滑后的路径
- **红色圆点**：起始点
- **黄色圆圈**：当前位置
- **白色文字**：路径信息

### 7.2 控制台输出
```
Start point found at: (320, 479)
Track recognition completed!
Path length: 156
Confidence: 0.95
Path Analysis:
  Total length: 245.67 pixels
  Average curvature: 0.12 radians
```

## 8. 性能优化建议

### 8.1 参数调优
- **wall_threshold**：根据赛道对比度调整
- **path_threshold**：根据光照条件调整
- **max_search_depth**：根据赛道复杂度调整
- **use_dfs**：复杂赛道用DFS，简单赛道用BFS

### 8.2 算法优化
- 使用BFS时考虑路径长度限制
- 添加路径质量评估
- 实现多线程搜索
- 缓存已访问的路径段

## 9. 常见问题解决

### 9.1 找不到起始点
- 检查图像底部是否有白色区域
- 调整path_threshold参数
- 检查图像预处理效果

### 9.2 路径不连续
- 降低wall_threshold
- 增加max_search_depth
- 检查图像噪声

### 9.3 搜索时间过长
- 减少max_search_depth
- 使用BFS替代DFS
- 优化图像分辨率

## 10. 扩展功能

### 10.1 多路径检测
```cpp
// 检测多条可能的路径
vector<vector<Point2f>> FindMultiplePaths(const cv::Mat& image);
```

### 10.2 实时跟踪
```cpp
// 实时更新路径
bool UpdatePath(const cv::Mat& current_frame);
```

### 10.3 路径预测
```cpp
// 预测下一步路径
Point2f PredictNextPoint(const vector<Point2f>& path);
```

## 11. 总结

迷宫法巡线系统是一个强大而灵活的赛道识别工具，通过合理的参数配置和算法选择，可以适应各种复杂的赛道环境。关键是要根据实际应用场景调整参数，并充分利用系统的可视化功能来调试和优化算法性能。 