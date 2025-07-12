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
    void Print_Camera_Info() const; // 打印摄像头信息
    int Get_Camera_Index() const; // 获取当前摄像头索引
    double Get_Actual_FPS() const; // 获取摄像头实际帧率

    int Get_Row_Cut_Up() const{return _row_cut_up;}
    int Get_Row_Cut_Bottom() const{return _row_cut_bottom;}

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

    int _row_cut_up;
    int _row_cut_bottom;

};

}