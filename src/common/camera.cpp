#include "common/camera.hpp"
#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>

using namespace std;
using namespace cv;

namespace common{

Camera::Camera()
    : _input_mode(InputMode::CAMERA),_initialized(false),_config_loaded(false)
{
    try{
        Load_Config();
        _initialized = Init();
    }
    catch(const std::exception& e)
    {
        std::cerr << "Camera init failed: " << e.what() << std::endl;
        _initialized = false;
    }
}

Camera::~Camera()
{
    if(_cap.isOpened())
        _cap.release();
}

bool Camera::Init()
{
    try{
        std::string debug_mode = _parameter.Get_Parameter("Debug_Mode").get<string>();
        if(debug_mode == "picture")
        {
            _input_mode = InputMode::PICTURE;
            return Init_Picture();
        }
        else if(debug_mode == "video")
        {
            _input_mode = InputMode::VIDEO;
            return Init_Video();
        }
        else
        {
            _input_mode = InputMode::CAMERA;
            return Init_Camera();
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << "Failed to determine input mode: " << e.what() << std::endl;
        return false;
    }
}

bool Camera::Init_Camera()
{
    _cap.open(0);
    if(!_cap.isOpened())
    {
        std::cerr << "Failed to open camera" << std::endl;
        return false;
    }
    return true;
}

bool Camera::Init_Video()
{
    std::string video_path = _parameter.Get_Parameter("Debug_Video_Path").get<std::string>();
    std::cout << "DEBUG: 实际读取到的视频路径: " << video_path << std::endl;
    _cap.open(video_path);
    if(!_cap.isOpened())
    {
        std::cerr << "无法打开视频文件: " << video_path << std::endl;
        return false;
    }
    return true;
}

bool Camera::Init_Picture()
{
    std::string picture_path = _parameter.Get_Parameter("Debug_Picture_Path").get<std::string>();
    _frame = imread(picture_path);
    if(_frame.empty())
    {
        std::cerr << "Failed to load picture: " << picture_path << std::endl;
        return false;
    }
    return true;
}

void Camera::Load_Config()
{
    if(!_config_loaded)
    {
        _parameter.Load_Config();
        // 缓存常用参数
        try {
            _cached_threshold = _parameter.Get_Parameter("threshold").get<int>();
            int width = _parameter.Get_Parameter("Image_Width").get<int>();
            int height = _parameter.Get_Parameter("Image_Height").get<int>();
            _cached_size = cv::Size(width, height);
        } catch (const std::exception& e) {
            std::cerr << "Failed to load cached parameters: " << e.what() << std::endl;
            // 使用默认值
            _cached_threshold = 127;
            _cached_size = cv::Size(640, 480);
        }
        
        _config_loaded = true;
    }
}

int Camera::Get_Threshold_Value() const
{
    return _cached_threshold;
}

/**
 * @brief 捕获帧（每调用一次，获取一帧图像）
 * @return 是否捕获成功
 */
bool Camera::Capture()
{
    if (!_initialized) {
        std::cerr << "Camera not initialized" << std::endl;
        return false;
    }
    
    if (_input_mode == InputMode::PICTURE) {
        // 图片模式不需要捕获
        return true;
    }
    
    if (!_cap.isOpened()) {
        std::cerr << "Video capture not available" << std::endl;
        return false;
    }
    _cap >> _frame;
    if (_frame.empty()) {
        std::cerr << "Failed to capture frame" << std::endl;
        return false;
    }
    return true;
}

bool Camera::Frame_Process()
{
    // 检查原始图像是否为空
    if (_frame.empty()) {
        std::cerr << "错误：原始图像为空，无法进行处理" << std::endl;
        return false;
    }
    
    try {
        // 转换为灰度图像
        cv::cvtColor(_frame, _gray_frame, cv::COLOR_BGR2GRAY);
        
        // 检查灰度图像是否转换成功
        if (_gray_frame.empty()) {
            std::cerr << "错误：灰度图像转换失败" << std::endl;
            return false;
        }
        
        // 二值化处理
        cv::threshold(_gray_frame, _binary_frame, Get_Threshold_Value(), 255, cv::THRESH_BINARY);
        
        // 检查二值化图像是否处理成功
        if (_binary_frame.empty()) {
            std::cerr << "错误：二值化处理失败" << std::endl;
            return false;
        }
        
        return true;
    } catch (const cv::Exception& e) {
        std::cerr << "OpenCV处理异常: " << e.what() << std::endl;
        return false;
    } catch (const std::exception& e) {
        std::cerr << "图像处理异常: " << e.what() << std::endl;
        return false;
    }
}

// 设置帧
void Camera::Set_Frame(const cv::Mat& frame) {
    _frame = frame.clone();
}

// 调整帧大小
void Camera::Resize_Frame(int width, int height) {
    // 检查原始图像是否为空
    if (_frame.empty()) {
        std::cerr << "警告：原始图像为空，无法调整大小" << std::endl;
        return;
    }
    // 检查尺寸参数是否有效
    if (width <= 0 || height <= 0) {
        std::cerr << "错误：无效的尺寸参数 width=" << width << ", height=" << height << std::endl;
        return;
    }
    
    try {
        cv::resize(_frame, _frame, cv::Size(width, height));
    } catch (const cv::Exception& e) {
        std::cerr << "OpenCV调整大小异常: " << e.what() << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "调整大小异常: " << e.what() << std::endl;
    }
}

// 检查是否初始化成功
bool Camera::Is_Initialized() const {
    return _initialized;
}



//----------------------------------------getter方法----------------------------------------
cv::Mat Camera::Get_Frame() const
{
    // 检查是否已初始化
    if (!_initialized) {
        std::cerr << "警告：摄像头未初始化，返回空Mat" << std::endl;
        return cv::Mat();
    }
    
    // 检查原始图像是否为空
    if (_frame.empty()) {
        std::cerr << "警告：原始图像为空" << std::endl;
        return cv::Mat();
    }
    return _frame;
}

// 获取灰度帧
cv::Mat Camera::Get_Gray_Frame() {
    // 检查是否已初始化
    if (!_initialized) {
        std::cerr << "警告：摄像头未初始化，返回空Mat" << std::endl;
        return cv::Mat();
    }
    
    // 检查灰度图像是否为空
    if (_gray_frame.empty()) {
        std::cerr << "警告：灰度图像为空，尝试重新处理" << std::endl;
        // 尝试重新处理图像
        if (!_frame.empty()) {
            Frame_Process();
        }
        // 如果仍然为空，返回空Mat
        if (_gray_frame.empty()) {
            return cv::Mat();
        }
    }
    return _gray_frame;
}

// 获取二值化帧
cv::Mat Camera::Get_Binary_Frame() {
    // 检查是否已初始化
    if (!_initialized) {
        std::cerr << "警告：摄像头未初始化，返回空Mat" << std::endl;
        return cv::Mat();
    }
    
    // 检查二值化图像是否为空
    if (_binary_frame.empty()) {
        std::cerr << "警告：二值化图像为空，尝试重新处理" << std::endl;
        // 尝试重新处理图像
        if (!_frame.empty()) {
            Frame_Process();
        }
        // 如果仍然为空，返回空Mat
        if (_binary_frame.empty()) {
            return cv::Mat();
        }
    }
    return _binary_frame;
}




}