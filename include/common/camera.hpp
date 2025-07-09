#pragma once

#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include "common/parameter.hpp"

namespace common
{

class Camera
{
public:
    enum class InputMode {
        CAMERA,
        VIDEO,
        PICTURE
    };
    
    Camera();
    ~Camera();
    bool Init();
    bool Capture();

    cv::Mat Get_Frame() const;
    cv::Mat Get_Gray_Frame();
    cv::Mat Get_Binary_Frame();
    bool Frame_Process();
    void Set_Frame(const cv::Mat& frame); // 设置帧
    void Resize_Frame(int width, int height); //设置图像大小
    bool Is_Initialized() const; // 检查是否初始化成功

private:
    bool Init_Camera();
    bool Init_Video();
    bool Init_Picture();
    void Load_Config();
    int Get_Threshold_Value() const;

    cv::VideoCapture _cap;   //摄像头
    cv::Mat _frame;          //一帧图像
    cv::Mat _blur_frame;     //高斯模糊图像
    cv::Mat _gray_frame;     //灰度图像
    cv::Mat _binary_frame;   //二值图像
    
    Parameter _parameter;    //参数管理器
    InputMode _input_mode;   //输入模式
    bool _initialized;       //初始化标志
    bool _config_loaded;     //配置加载标志

    int _cached_threshold;   //缓存阈值
    cv::Size _cached_size;   //缓存大小

};

}