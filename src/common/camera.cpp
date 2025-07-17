#include "common/camera.hpp"
#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <cstdlib>
#include <string>

using namespace std;
using namespace cv;

namespace common{

// 辅助函数：使用v4l2-ctl设置摄像头参数
bool set_camera_params_v4l2(int camera_index, int width, int height, int fps) {
    // 优先尝试MJPG格式（支持更高帧率）
    std::string cmd_mjpg = "v4l2-ctl --device=/dev/video" + std::to_string(camera_index) + 
                           " --set-fmt-video=width=" + std::to_string(width) + 
                           ",height=" + std::to_string(height) + 
                           ",pixelformat=MJPG";
    
    std::cout << "尝试MJPG格式: " << cmd_mjpg << std::endl;
    int result = system(cmd_mjpg.c_str());
    
    if(result == 0) {
        std::cout << "v4l2-ctl设置MJPG成功" << std::endl;
        return true;
    } else {
        std::cout << "MJPG设置失败，尝试YUYV格式..." << std::endl;
        
        // 如果MJPG失败，尝试YUYV格式
        std::string cmd_yuyv = "v4l2-ctl --device=/dev/video" + std::to_string(camera_index) + 
                               " --set-fmt-video=width=" + std::to_string(width) + 
                               ",height=" + std::to_string(height) + 
                               ",pixelformat=YUYV";
        
        std::cout << "尝试YUYV格式: " << cmd_yuyv << std::endl;
        result = system(cmd_yuyv.c_str());
        
        if(result == 0) {
            std::cout << "v4l2-ctl设置YUYV成功" << std::endl;
            return true;
        } else {
            std::cout << "v4l2-ctl设置失败，错误码: " << result << std::endl;
            return false;
        }
    }
}

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
        _debug_mode = _parameter.Get_Parameter("Debug_Mode").get<string>();
        if(_debug_mode == "picture")
        {
            _input_mode = InputMode::PICTURE;
            return Init_Picture();
        }
        else if(_debug_mode == "video")
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
    // 从配置文件读取摄像头参数
    int camera_index = _parameter.Get_Parameter("Camera_Index").get<int>();
    int camera_width = _parameter.Get_Parameter("Camera_Width").get<int>();
    int camera_height = _parameter.Get_Parameter("Camera_Height").get<int>();
    int camera_fps = _parameter.Get_Parameter("Camera_FPS").get<int>();
    
    std::cout << "尝试打开摄像头 " << camera_index << std::endl;
    
    // 先使用v4l2-ctl设置摄像头参数
    std::cout << "使用v4l2-ctl预设置摄像头参数..." << std::endl;
    set_camera_params_v4l2(camera_index, camera_width, camera_height, camera_fps);
    
    // 尝试打开指定摄像头，强制使用V4L2后端
    _cap.open(camera_index, cv::CAP_V4L2);
    if(_cap.isOpened()) {
        std::cout << "成功打开摄像头 " << camera_index << std::endl;
        
        // 设置摄像头参数 - 使用更强制的方法
        std::cout << "正在设置摄像头参数..." << std::endl;
        std::cout << "  目标分辨率: " << camera_width << "x" << camera_height << std::endl;
        std::cout << "  目标帧率: " << camera_fps << " FPS" << std::endl;
        
        // 先设置像素格式，优先使用MJPG（支持更高帧率）
        bool mjpg_success = _cap.set(cv::CAP_PROP_FOURCC, cv::VideoWriter::fourcc('M', 'J', 'P', 'G'));
        if(!mjpg_success) {
            std::cout << "MJPG设置失败，尝试YUYV格式..." << std::endl;
            _cap.set(cv::CAP_PROP_FOURCC, cv::VideoWriter::fourcc('Y', 'U', 'Y', 'V'));
        }
        
        _cap.set(cv::CAP_PROP_FRAME_WIDTH, camera_width);
        _cap.set(cv::CAP_PROP_FRAME_HEIGHT, camera_height);
        _cap.set(cv::CAP_PROP_FPS, camera_fps);
        
        // 强制刷新设置
        cv::Mat dummy_frame;
        _cap >> dummy_frame;
        
        // 再次设置帧率，确保生效
        _cap.set(cv::CAP_PROP_FPS, camera_fps);
        
        // 验证设置是否生效
        double actual_width = _cap.get(cv::CAP_PROP_FRAME_WIDTH);
        double actual_height = _cap.get(cv::CAP_PROP_FRAME_HEIGHT);
        double actual_fps = _cap.get(cv::CAP_PROP_FPS);
        double fourcc = _cap.get(cv::CAP_PROP_FOURCC);
        
        std::cout << "摄像头参数设置结果：" << std::endl;
        std::cout << "  分辨率: " << actual_width << "x" << actual_height;
        if(actual_width == camera_width && actual_height == camera_height) {
            std::cout << " ✓" << std::endl;
        } else {
            std::cout << " ✗ (目标: " << camera_width << "x" << camera_height << ")" << std::endl;
        }
        
        std::cout << "  帧率: " << actual_fps << " FPS";
        if(actual_fps >= camera_fps * 0.8) {  // 允许20%的误差
            std::cout << " ✓" << std::endl;
        } else {
            std::cout << " ✗ (目标: " << camera_fps << " FPS)" << std::endl;
            std::cout << "  尝试强制设置帧率..." << std::endl;
            
            // 尝试多种方法强制设置帧率
            _cap.set(cv::CAP_PROP_FPS, camera_fps);
            _cap.set(cv::CAP_PROP_BUFFERSIZE, 1);  // 减少缓冲区大小
            
            // 再次验证
            double new_fps = _cap.get(cv::CAP_PROP_FPS);
            std::cout << "  重新设置后帧率: " << new_fps << " FPS" << std::endl;
        }
        
        std::cout << "  像素格式: " << (char)((int)fourcc & 0xFF) << (char)(((int)fourcc >> 8) & 0xFF) 
                  << (char)(((int)fourcc >> 16) & 0xFF) << (char)(((int)fourcc >> 24) & 0xFF) << std::endl;
        
        // 测试读取一帧确保摄像头正常工作
        cv::Mat test_frame;
        _cap >> test_frame;
        if(test_frame.empty()) {
            std::cerr << "警告：摄像头打开但无法读取图像，尝试其他摄像头" << std::endl;
            _cap.release();
        } else {
            return true;
        }
    }
    
    // 如果指定摄像头失败，尝试自动检测
    std::cout << "指定摄像头打开失败，尝试自动检测..." << std::endl;
    
    // 优先尝试已知存在的摄像头设备
    std::vector<int> camera_list = {2, 0}; // 根据v4l2-ctl输出，优先尝试video2和video0
    
    for(int i : camera_list) {
        if(i == camera_index) continue; // 跳过已尝试的摄像头
        
        std::cout << "尝试摄像头 " << i << "..." << std::endl;
        _cap.open(i, cv::CAP_V4L2);
        if(_cap.isOpened()) {
            std::cout << "自动检测到可用摄像头 " << i << std::endl;
            
            // 设置摄像头参数 - 使用更强制的方法
            bool mjpg_success = _cap.set(cv::CAP_PROP_FOURCC, cv::VideoWriter::fourcc('M', 'J', 'P', 'G'));
            if(!mjpg_success) {
                _cap.set(cv::CAP_PROP_FOURCC, cv::VideoWriter::fourcc('Y', 'U', 'Y', 'V'));
            }
            
            _cap.set(cv::CAP_PROP_FRAME_WIDTH, camera_width);
            _cap.set(cv::CAP_PROP_FRAME_HEIGHT, camera_height);
            _cap.set(cv::CAP_PROP_FPS, camera_fps);
            
            // 强制刷新设置
            cv::Mat dummy_frame;
            _cap >> dummy_frame;
            
            // 再次设置帧率，确保生效
            _cap.set(cv::CAP_PROP_FPS, camera_fps);
            
            // 测试读取一帧确保摄像头正常工作
            cv::Mat test_frame;
            _cap >> test_frame;
            if(!test_frame.empty()) {
                std::cout << "摄像头 " << i << " 成功读取图像，分辨率: " 
                          << test_frame.cols << "x" << test_frame.rows << std::endl;
                return true;
            } else {
                std::cerr << "摄像头 " << i << " 无法读取图像，尝试下一个" << std::endl;
                _cap.release();
            }
        }
    }
    
    std::cerr << "未找到可用的摄像头" << std::endl;
    return false;
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
            _row_cut_up = _parameter.Get_Parameter("Row_Cut_Up").get<int>();
            _row_cut_bottom = _parameter.Get_Parameter("Row_Cut_Bottom").get<int>();
            _video_delay = _parameter.Get_Parameter("Video_Delay").get<int>();
        } catch (const std::exception& e) {
            std::cerr << "Failed to load cached parameters: " << e.what() << std::endl;
            // 使用默认值
            _cached_threshold = 127;
            _cached_size = cv::Size(640, 480);
            _row_cut_up = 10;
            _row_cut_bottom = 10;
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
        // 检查图像尺寸是否有效
        if (_frame.cols <= 0 || _frame.rows <= 0) {
            std::cerr << "错误：图像尺寸无效 cols=" << _frame.cols << ", rows=" << _frame.rows << std::endl;
            return false;
        }
        
        // 转换为灰度图像
        cv::cvtColor(_frame, _gray_frame, cv::COLOR_BGR2GRAY);
        
        // 检查灰度图像是否转换成功
        if (_gray_frame.empty()) {
            std::cerr << "错误：灰度图像转换失败" << std::endl;
            return false;
        }
        
        // 检查灰度图像尺寸
        if (_gray_frame.cols <= 0 || _gray_frame.rows <= 0) {
            std::cerr << "错误：灰度图像尺寸无效" << std::endl;
            return false;
        }
        
        // 二值化处理
        cv::threshold(_gray_frame, _binary_frame, Get_Threshold_Value(), 255, cv::THRESH_BINARY);
        
        // 检查二值化图像是否处理成功
        if (_binary_frame.empty()) {
            std::cerr << "错误：二值化处理失败" << std::endl;
            return false;
        }
        
        // 检查二值化图像尺寸
        if (_binary_frame.cols <= 0 || _binary_frame.rows <= 0) {
            std::cerr << "错误：二值化图像尺寸无效" << std::endl;
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
bool Camera::Is_Initialized() const
{
    return _initialized;
}

void Camera::Print_Camera_Info() const
{
    if(!_initialized || !_cap.isOpened()) {
        std::cout << "摄像头未初始化" << std::endl;
        return;
    }
    
    std::cout << "=== 摄像头信息 ===" << std::endl;
    std::cout << "摄像头索引: " << Get_Camera_Index() << std::endl;
    std::cout << "分辨率: " << _cap.get(cv::CAP_PROP_FRAME_WIDTH) << "x" 
              << _cap.get(cv::CAP_PROP_FRAME_HEIGHT) << std::endl;
    std::cout << "帧率: " << _cap.get(cv::CAP_PROP_FPS) << " FPS" << std::endl;
    std::cout << "亮度: " << _cap.get(cv::CAP_PROP_BRIGHTNESS) << std::endl;
    std::cout << "对比度: " << _cap.get(cv::CAP_PROP_CONTRAST) << std::endl;
    std::cout << "饱和度: " << _cap.get(cv::CAP_PROP_SATURATION) << std::endl;
    std::cout << "色相: " << _cap.get(cv::CAP_PROP_HUE) << std::endl;
    std::cout << "==================" << std::endl;
}

int Camera::Get_Camera_Index() const
{
    // 注意：OpenCV没有直接获取摄像头索引的方法
    // 这里返回配置文件中设置的索引
    // 由于是const函数，我们需要使用一个临时变量
    Parameter temp_param;
    return temp_param.Get_Parameter("Camera_Index").get<int>();
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

double Camera::Get_Actual_FPS() const {
    if (!_cap.isOpened()) {
        return 0.0;
    }
    return _cap.get(cv::CAP_PROP_FPS);
}

}