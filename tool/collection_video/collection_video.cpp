/**
 * @file collection_video.cpp
 * @brief 摄像头视频录制工具
 * @details 该程序用于打开摄像头并录制视频保存到本地文件
 * 
 * 功能特性：
 * - 自动检测并打开可用摄像头
 * - 支持自定义录制参数（分辨率、帧率、编码格式）
 * - 实时显示录制预览
 * - 支持键盘控制（开始/停止录制、退出程序）
 * - 自动生成带时间戳的文件名
 * 
 * 使用方法：
 * - 运行程序后按 'r' 开始录制
 * - 录制过程中按 's' 停止录制
 * - 按 'q' 退出程序
 * 
 * @author AI Assistant
 * @date 2024
 */

#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <string>
#include <chrono>
#include <iomanip>
#include <sstream>

using namespace cv;
using namespace std;

/**
 * @brief 生成带时间戳的文件名
 * @return 格式化的时间戳字符串，用于文件名
 * 
 * 生成格式：YYYY-MM-DD_HH-MM-SS
 * 例如：2024-01-15_14-30-25
 */
string generate_timestamp() {
    auto now = chrono::system_clock::now();
    auto time_t = chrono::system_clock::to_time_t(now);
    auto tm = *localtime(&time_t);
    
    stringstream ss;
    ss << setfill('0') 
       << setw(4) << (tm.tm_year + 1900) << "-"
       << setw(2) << (tm.tm_mon + 1) << "-"
       << setw(2) << tm.tm_mday << "_"
       << setw(2) << tm.tm_hour << "-"
       << setw(2) << tm.tm_min << "-"
       << setw(2) << tm.tm_sec;
    
    return ss.str();
}

/**
 * @brief 录制配置结构体
 * @details 包含所有录制相关的参数设置
 */
struct RecordingConfig {
    int camera_index = 1;        // 摄像头索引，通常0是默认摄像头
    int frame_width = 1280;       // 录制视频宽度
    int frame_height = 720;      // 录制视频高度
    int fps = 30;                // 录制帧率
    string output_dir = "./";    // 输出目录
    string filename_prefix = "recording_"; // 文件名前缀
    
    // 视频编码器设置
    int fourcc_code = VideoWriter::fourcc('M', 'J', 'P', 'G'); // MJPG编码器
    string file_extension = ".avi"; // 文件扩展名
};

/**
 * @brief 录制状态枚举
 */
enum class RecordingState {
    IDLE,       // 空闲状态，等待开始录制
    RECORDING,  // 正在录制
    STOPPING    // 正在停止录制
};

/**
 * @brief 视频录制器类
 * @details 封装了摄像头操作和视频录制功能
 */
class VideoRecorder {
private:
    VideoCapture cap_;           // 摄像头捕获对象
    VideoWriter writer_;         // 视频写入对象
    RecordingConfig config_;     // 录制配置
    RecordingState state_;       // 当前录制状态
    string current_filename_;    // 当前录制文件名
    int frame_count_;           // 已录制帧数
    
public:
    /**
     * @brief 构造函数
     * @param config 录制配置参数
     */
    VideoRecorder(const RecordingConfig& config) 
        : config_(config), state_(RecordingState::IDLE), frame_count_(0) {
        cout << "=== 视频录制器初始化 ===" << endl;
        cout << "摄像头索引: " << config_.camera_index << endl;
        cout << "分辨率: " << config_.frame_width << "x" << config_.frame_height << endl;
        cout << "帧率: " << config_.fps << " FPS" << endl;
        cout << "输出目录: " << config_.output_dir << endl;
    }
    
    /**
     * @brief 析构函数，确保资源正确释放
     */
    ~VideoRecorder() {
        stop_recording();
        if (cap_.isOpened()) {
            cap_.release();
        }
        cout << "视频录制器已关闭" << endl;
    }
    
    /**
     * @brief 初始化摄像头
     * @return 初始化是否成功
     * 
     * 尝试打开指定摄像头，如果失败则自动尝试其他摄像头
     */
    bool initialize_camera() {
        cout << "\n正在初始化摄像头..." << endl;
        
        // 首先尝试打开指定摄像头
        cap_.open(config_.camera_index, CAP_V4L2);
        
        if (!cap_.isOpened()) {
            cout << "指定摄像头 " << config_.camera_index << " 打开失败，尝试其他摄像头..." << endl;
            
            // 尝试常见的摄像头索引
            vector<int> camera_indices = {0, 1, 2, 3};
            for (int idx : camera_indices) {
                if (idx == config_.camera_index) continue;
                
                cout << "尝试摄像头 " << idx << "..." << endl;
                cap_.open(idx, CAP_V4L2);
                if (cap_.isOpened()) {
                    config_.camera_index = idx;
                    cout << "成功打开摄像头 " << idx << endl;
                    break;
                }
            }
        }
        
        if (!cap_.isOpened()) {
            cerr << "错误：无法打开任何摄像头！" << endl;
            return false;
        }
        
        // 设置摄像头参数
        cout << "设置摄像头参数..." << endl;
        
        // 设置像素格式（优先使用MJPG以获得更高帧率）
        bool mjpg_success = cap_.set(CAP_PROP_FOURCC, config_.fourcc_code);
        if (!mjpg_success) {
            cout << "MJPG设置失败，使用YUYV格式" << endl;
            cap_.set(CAP_PROP_FOURCC, VideoWriter::fourcc('Y', 'U', 'Y', 'V'));
        }
        
        // 设置分辨率
        cap_.set(CAP_PROP_FRAME_WIDTH, config_.frame_width);
        cap_.set(CAP_PROP_FRAME_HEIGHT, config_.frame_height);
        
        // 设置帧率
        cap_.set(CAP_PROP_FPS, config_.fps);
        
        // 减少缓冲区大小以降低延迟
        cap_.set(CAP_PROP_BUFFERSIZE, 1);
        
        // 验证设置是否生效
        double actual_width = cap_.get(CAP_PROP_FRAME_WIDTH);
        double actual_height = cap_.get(CAP_PROP_FRAME_HEIGHT);
        double actual_fps = cap_.get(CAP_PROP_FPS);
        
        cout << "摄像头参数验证：" << endl;
        cout << "  分辨率: " << actual_width << "x" << actual_height << endl;
        cout << "  帧率: " << actual_fps << " FPS" << endl;
        
        // 测试读取一帧
        Mat test_frame;
        cap_ >> test_frame;
        if (test_frame.empty()) {
            cerr << "错误：摄像头无法读取图像！" << endl;
            return false;
        }
        
        cout << "摄像头初始化成功！" << endl;
        return true;
    }
    
    /**
     * @brief 开始录制视频
     * @return 录制是否成功开始
     * 
     * 创建视频文件并开始写入帧数据
     */
    bool start_recording() {
        if (state_ == RecordingState::RECORDING) {
            cout << "已经在录制中..." << endl;
            return true;
        }
        
        // 生成带时间戳的文件名
        string timestamp = generate_timestamp();
        current_filename_ = config_.output_dir + config_.filename_prefix + timestamp + config_.file_extension;
        
        cout << "\n开始录制视频..." << endl;
        cout << "文件名: " << current_filename_ << endl;
        
        // 创建视频写入器
        writer_.open(current_filename_, config_.fourcc_code, config_.fps, 
                    Size(config_.frame_width, config_.frame_height), true);
        
        if (!writer_.isOpened()) {
            cerr << "错误：无法创建视频文件！" << endl;
            return false;
        }
        
        state_ = RecordingState::RECORDING;
        frame_count_ = 0;
        
        cout << "录制已开始，按 's' 停止录制" << endl;
        return true;
    }
    
    /**
     * @brief 停止录制视频
     * @return 停止是否成功
     * 
     * 关闭视频文件并显示录制统计信息
     */
    bool stop_recording() {
        if (state_ != RecordingState::RECORDING) {
            return true;
        }
        
        cout << "\n停止录制..." << endl;
        
        // 关闭视频写入器
        writer_.release();
        
        state_ = RecordingState::IDLE;
        
        cout << "录制完成！" << endl;
        cout << "文件保存为: " << current_filename_ << endl;
        cout << "总帧数: " << frame_count_ << endl;
        cout << "录制时长: " << (frame_count_ / (double)config_.fps) << " 秒" << endl;
        
        return true;
    }
    
    /**
     * @brief 处理一帧图像
     * @param frame 当前帧图像
     * @return 处理是否成功
     * 
     * 如果正在录制，将帧写入视频文件
     */
    bool process_frame(const Mat& frame) {
        if (state_ == RecordingState::RECORDING) {
            writer_.write(frame);
            frame_count_++;
            
            // 每100帧显示一次进度
            if (frame_count_ % 100 == 0) {
                cout << "已录制 " << frame_count_ << " 帧" << endl;
            }
        }
        return true;
    }
    
    /**
     * @brief 获取当前录制状态
     * @return 当前状态
     */
    RecordingState get_state() const {
        return state_;
    }
    
    /**
     * @brief 获取摄像头对象
     * @return 摄像头引用
     */
    VideoCapture& get_camera() {
        return cap_;
    }
    
    /**
     * @brief 检查摄像头是否打开
     * @return 摄像头是否可用
     */
    bool is_camera_open() const {
        return cap_.isOpened();
    }
};

/**
 * @brief 显示帮助信息
 */
void show_help() {
    cout << "\n=== 视频录制工具使用说明 ===" << endl;
    cout << "录制控制：" << endl;
    cout << "  'r' - 开始录制" << endl;
    cout << "  's' - 停止录制" << endl;
    cout << "  'q' - 退出程序" << endl;
    cout << "  'h' - 显示此帮助信息" << endl;
    cout << "\n参数调整：" << endl;
    cout << "  '1' - 切换到摄像头 0" << endl;
    cout << "  '2' - 切换到摄像头 1" << endl;
    cout << "  '3' - 切换到摄像头 2" << endl;
    cout << "  '4' - 切换到摄像头 3" << endl;
    cout << "  'w' - 增加分辨率宽度 (+160)" << endl;
    cout << "  'e' - 减少分辨率宽度 (-160)" << endl;
    cout << "  'a' - 增加分辨率高度 (+120)" << endl;
    cout << "  'd' - 减少分辨率高度 (-120)" << endl;
    cout << "  'f' - 增加帧率 (+10)" << endl;
    cout << "  'g' - 减少帧率 (-10)" << endl;
    cout << "  'i' - 显示当前参数" << endl;
    cout << "===============================" << endl;
}

/**
 * @brief 显示当前录制参数
 * @param config 当前配置
 * @param current_camera 当前使用的摄像头索引
 */
void show_current_params(const RecordingConfig& config, int current_camera) {
    cout << "\n=== 当前参数设置 ===" << endl;
    cout << "摄像头索引: " << current_camera << endl;
    cout << "分辨率: " << config.frame_width << "x" << config.frame_height << endl;
    cout << "帧率: " << config.fps << " FPS" << endl;
    cout << "输出目录: " << config.output_dir << endl;
    cout << "文件名前缀: " << config.filename_prefix << endl;
    cout << "===============================" << endl;
}

/**
 * @brief 主函数
 * @return 程序退出码
 * 
 * 程序主流程：
 * 1. 初始化录制配置
 * 2. 创建录制器并初始化摄像头
 * 3. 进入主循环，处理用户输入和视频帧
 * 4. 清理资源并退出
 */
int main() {
    cout << "=== 智能车视频录制工具 ===" << endl;
    cout << "版本: 1.0" << endl;
    cout << "作者: AI Assistant" << endl;
    
    // 配置录制参数
    RecordingConfig config;
    config.camera_index = 1;     // 使用第二个摄像头
    config.frame_width = 1280;   // 视频宽度
    config.frame_height = 720;   // 视频高度
    config.fps = 30;             // 帧率
    config.output_dir = "./";    // 输出目录
    config.filename_prefix = "recording_"; // 文件名前缀
    
    // 创建视频录制器
    VideoRecorder recorder(config);
    
    // 初始化摄像头
    if (!recorder.initialize_camera()) {
        cerr << "摄像头初始化失败，程序退出" << endl;
        return -1;
    }
    
    // 显示帮助信息
    show_help();
    
    // 创建窗口用于显示预览
    namedWindow("视频录制预览", WINDOW_AUTOSIZE);
    
    cout << "\n程序已启动，等待用户操作..." << endl;
    
    // 主循环
    while (true) {
        // 读取摄像头帧
        Mat frame;
        recorder.get_camera() >> frame;
        
        if (frame.empty()) {
            cerr << "错误：无法读取摄像头帧！" << endl;
            break;
        }
        
        // 处理帧（如果正在录制则写入文件）
        recorder.process_frame(frame);
        
        // 在预览窗口上添加状态信息
        Mat display_frame = frame.clone();
        string status_text;
        Scalar text_color;
        
        if (recorder.get_state() == RecordingState::RECORDING) {
            status_text = "recording...";
            text_color = Scalar(0, 0, 255); // 红色
        } else {
            status_text = "idle...";
            text_color = Scalar(0, 255, 0); // 绿色
        }
        
        // 在图像上绘制状态文本
        putText(display_frame, status_text, Point(10, 30), 
                FONT_HERSHEY_SIMPLEX, 1.0, text_color, 2);
        
        // 显示预览
        imshow("视频录制预览", display_frame);
        
        // 处理键盘输入
        char key = waitKey(1) & 0xFF;
        
        switch (key) {
            case 'r': // 开始录制
                if (recorder.get_state() != RecordingState::RECORDING) {
                    recorder.start_recording();
                }
                break;
                
            case 's': // 停止录制
                if (recorder.get_state() == RecordingState::RECORDING) {
                    recorder.stop_recording();
                }
                break;
                
            case 'q': // 退出程序
                cout << "\n用户请求退出程序..." << endl;
                goto exit_loop;
                
            case 'h': // 显示帮助
                show_help();
                break;
                
            case 'i': // 显示当前参数
                show_current_params(config, config.camera_index);
                break;
                
            // 摄像头切换
            case '1': // 切换到摄像头 0
                if (recorder.get_state() != RecordingState::RECORDING) {
                    cout << "\n尝试切换到摄像头 0..." << endl;
                    config.camera_index = 0;
                    // 重新创建录制器
                    recorder.~VideoRecorder();
                    new (&recorder) VideoRecorder(config);
                    if (!recorder.initialize_camera()) {
                        cerr << "摄像头 0 初始化失败！" << endl;
                    }
                } else {
                    cout << "录制中无法切换摄像头，请先停止录制" << endl;
                }
                break;
                
            case '2': // 切换到摄像头 1
                if (recorder.get_state() != RecordingState::RECORDING) {
                    cout << "\n尝试切换到摄像头 1..." << endl;
                    config.camera_index = 1;
                    recorder.~VideoRecorder();
                    new (&recorder) VideoRecorder(config);
                    if (!recorder.initialize_camera()) {
                        cerr << "摄像头 1 初始化失败！" << endl;
                    }
                } else {
                    cout << "录制中无法切换摄像头，请先停止录制" << endl;
                }
                break;
                
            case '3': // 切换到摄像头 2
                if (recorder.get_state() != RecordingState::RECORDING) {
                    cout << "\n尝试切换到摄像头 2..." << endl;
                    config.camera_index = 2;
                    recorder.~VideoRecorder();
                    new (&recorder) VideoRecorder(config);
                    if (!recorder.initialize_camera()) {
                        cerr << "摄像头 2 初始化失败！" << endl;
                    }
                } else {
                    cout << "录制中无法切换摄像头，请先停止录制" << endl;
                }
                break;
                
            case '4': // 切换到摄像头 3
                if (recorder.get_state() != RecordingState::RECORDING) {
                    cout << "\n尝试切换到摄像头 3..." << endl;
                    config.camera_index = 3;
                    recorder.~VideoRecorder();
                    new (&recorder) VideoRecorder(config);
                    if (!recorder.initialize_camera()) {
                        cerr << "摄像头 3 初始化失败！" << endl;
                    }
                } else {
                    cout << "录制中无法切换摄像头，请先停止录制" << endl;
                }
                break;
                
            // 分辨率调整
            case 'w': // 增加宽度
                if (recorder.get_state() != RecordingState::RECORDING) {
                    config.frame_width += 160;
                    cout << "分辨率宽度调整为: " << config.frame_width << endl;
                    recorder.~VideoRecorder();
                    new (&recorder) VideoRecorder(config);
                    if (!recorder.initialize_camera()) {
                        cerr << "参数调整后摄像头初始化失败！" << endl;
                        config.frame_width -= 160; // 回滚
                    }
                } else {
                    cout << "录制中无法调整参数，请先停止录制" << endl;
                }
                break;
                
            case 'e': // 减少宽度
                if (recorder.get_state() != RecordingState::RECORDING) {
                    if (config.frame_width > 160) {
                        config.frame_width -= 160;
                        cout << "分辨率宽度调整为: " << config.frame_width << endl;
                        recorder.~VideoRecorder();
                        new (&recorder) VideoRecorder(config);
                        if (!recorder.initialize_camera()) {
                            cerr << "参数调整后摄像头初始化失败！" << endl;
                            config.frame_width += 160; // 回滚
                        }
                    } else {
                        cout << "分辨率宽度已达到最小值！" << endl;
                    }
                } else {
                    cout << "录制中无法调整参数，请先停止录制" << endl;
                }
                break;
                
            case 'a': // 增加高度
                if (recorder.get_state() != RecordingState::RECORDING) {
                    config.frame_height += 120;
                    cout << "分辨率高度调整为: " << config.frame_height << endl;
                    recorder.~VideoRecorder();
                    new (&recorder) VideoRecorder(config);
                    if (!recorder.initialize_camera()) {
                        cerr << "参数调整后摄像头初始化失败！" << endl;
                        config.frame_height -= 120; // 回滚
                    }
                } else {
                    cout << "录制中无法调整参数，请先停止录制" << endl;
                }
                break;
                
            case 'd': // 减少高度
                if (recorder.get_state() != RecordingState::RECORDING) {
                    if (config.frame_height > 120) {
                        config.frame_height -= 120;
                        cout << "分辨率高度调整为: " << config.frame_height << endl;
                        recorder.~VideoRecorder();
                        new (&recorder) VideoRecorder(config);
                        if (!recorder.initialize_camera()) {
                            cerr << "参数调整后摄像头初始化失败！" << endl;
                            config.frame_height += 120; // 回滚
                        }
                    } else {
                        cout << "分辨率高度已达到最小值！" << endl;
                    }
                } else {
                    cout << "录制中无法调整参数，请先停止录制" << endl;
                }
                break;
                
            // 帧率调整
            case 'f': // 增加帧率
                if (recorder.get_state() != RecordingState::RECORDING) {
                    config.fps += 10;
                    cout << "帧率调整为: " << config.fps << " FPS" << endl;
                    recorder.~VideoRecorder();
                    new (&recorder) VideoRecorder(config);
                    if (!recorder.initialize_camera()) {
                        cerr << "参数调整后摄像头初始化失败！" << endl;
                        config.fps -= 10; // 回滚
                    }
                } else {
                    cout << "录制中无法调整参数，请先停止录制" << endl;
                }
                break;
                
            case 'g': // 减少帧率
                if (recorder.get_state() != RecordingState::RECORDING) {
                    if (config.fps > 10) {
                        config.fps -= 10;
                        cout << "帧率调整为: " << config.fps << " FPS" << endl;
                        recorder.~VideoRecorder();
                        new (&recorder) VideoRecorder(config);
                        if (!recorder.initialize_camera()) {
                            cerr << "参数调整后摄像头初始化失败！" << endl;
                            config.fps += 10; // 回滚
                        }
                    } else {
                        cout << "帧率已达到最小值！" << endl;
                    }
                } else {
                    cout << "录制中无法调整参数，请先停止录制" << endl;
                }
                break;
                
            default:
                break;
        }
    }
    
exit_loop:
    // 确保停止录制
    if (recorder.get_state() == RecordingState::RECORDING) {
        recorder.stop_recording();
    }
    
    // 关闭窗口
    destroyAllWindows();
    
    cout << "程序已退出" << endl;
    return 0;
}
