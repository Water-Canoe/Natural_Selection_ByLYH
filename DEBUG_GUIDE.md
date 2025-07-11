# 调试和性能监控功能使用指南

## 功能概述

`Debug`类整合了调试输出和性能监控功能，提供统一的调试和性能监控接口。所有功能都封装在`src/common/debug.cpp`中，让main函数更加简洁。

## 主要功能

### 1. 调试输出功能
- 基于配置文件的调试输出控制
- 支持流式输出操作符
- 强制输出功能（不受配置控制）

### 2. 性能监控功能
- 实时帧率计算
- 平均帧率统计
- 最高/最低帧率记录
- 图像处理时间统计
- 自动性能日志记录

## 使用方法

### 1. 基本调试输出

```cpp
#include "common.hpp"

using namespace common;

// 基本输出（受Print_Mode控制）
debug << "这是一条调试信息" << std::endl;
debug << "数字: " << 42 << std::endl;

// 强制输出（不受Print_Mode控制）
debug.force_outputln("这条信息总是会显示");
```

### 2. 性能监控

```cpp
// 在main循环中
while(true) {
    // 开始新帧的处理
    debug.start_frame();
    
    // 开始图像处理计时
    debug.start_processing();
    
    // ... 图像处理代码 ...
    
    // 结束图像处理计时
    debug.end_processing();
    
    // 获取性能数据
    double current_fps = debug.get_current_fps();
    double avg_fps = debug.get_average_fps();
    int frame_count = debug.get_frame_count();
    
    // 检查是否需要输出性能统计
    if(debug.should_log_performance()) {
        debug.log_performance();
    }
}
```

### 3. 在图像上显示性能信息

```cpp
// 在图像上显示帧率信息
if (!draw_frame.empty()) {
    string fps_text = "FPS: " + to_string(static_cast<int>(debug.get_current_fps()));
    string avg_fps_text = "Avg FPS: " + to_string(static_cast<int>(debug.get_average_fps()));
    string frame_text = "Frame: " + to_string(debug.get_frame_count());
    
    // 绘制文本到图像上
    cv::putText(draw_frame, fps_text, cv::Point(10, 30), 
                cv::FONT_HERSHEY_SIMPLEX, 0.6, cv::Scalar(0, 255, 0), 2);
}
```

## 配置文件设置

在`config/config.json`中设置`Print_Mode`：

```json
{
    "Print_Mode": true,  // true: 启用调试输出, false: 禁用调试输出
    // ... 其他配置
}
```

## 性能日志文件

程序会自动创建性能日志文件，文件名格式：`performance_log_YYYYMMDD_HHMMSS.txt`

日志内容包括：
- 程序开始和结束时间
- 每100帧的性能统计
- 最终性能报告
- 调试模式状态

## 主要方法说明

### 调试输出方法
- `debug << value` - 流式输出（受配置控制）
- `debug.force_output(value)` - 强制输出
- `debug.force_outputln(value)` - 强制输出并换行
- `debug.is_print_enabled()` - 检查调试模式状态

### 性能监控方法
- `debug.start_frame()` - 开始新帧处理
- `debug.start_processing()` - 开始图像处理计时
- `debug.end_processing()` - 结束图像处理计时
- `debug.get_current_fps()` - 获取当前帧率
- `debug.get_average_fps()` - 获取平均帧率
- `debug.get_frame_count()` - 获取当前帧数
- `debug.should_log_performance()` - 检查是否需要输出性能统计
- `debug.log_performance()` - 输出性能统计到日志文件

## 优势

1. **代码简洁**: main函数不再混乱，所有调试和性能监控代码都封装在debug类中
2. **功能完整**: 整合了调试输出和性能监控的所有功能
3. **易于使用**: 简单的API接口，易于集成
4. **自动管理**: 自动创建日志文件，自动记录性能数据
5. **配置灵活**: 通过配置文件控制调试输出

## 注意事项

1. 确保配置文件路径正确
2. 性能日志文件会自动创建在程序运行目录
3. 程序异常退出时也会保存已记录的数据
4. 所有性能数据都会保存到文件中，不会干扰控制台输出

## 示例输出

### 控制台输出
```
成功从 config/config.json 加载配置文件，Print_Mode: true
调试输出模式已启用
检测到场景：斑马线 125
性能日志已保存到: performance_log_20240711_143052.txt
```

### 性能日志文件内容
```
=== 智能车视觉巡线性能日志 ===
开始时间: 2024-07-11 14:30:52
调试输出模式: 启用
=================================

=== 性能统计 (第100帧) ===
当前帧率: 25.67 FPS
平均帧率: 24.89 FPS
最高帧率: 30.12 FPS
最低帧率: 18.45 FPS
平均处理时间: 35.234 ms
总运行时间: 4.02 秒
=================================

=== 最终性能报告 ===
总帧数: 1250
总运行时间: 50.25 秒
平均帧率: 24.88 FPS
最高帧率: 30.12 FPS
最低帧率: 18.45 FPS
平均处理时间: 35.123 ms
===================
结束时间: 2024-07-11 14:31:42
``` 