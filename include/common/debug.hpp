#ifndef DEBUG_HPP
#define DEBUG_HPP

#include <iostream>
#include <sstream>
#include <string>
#include <chrono>
#include <deque>
#include <fstream>
#include <iomanip>
#include <nlohmann/json.hpp>
#include <vector>

namespace common {

/**
 * @brief 调试和性能监控类
 * 
 * 整合了调试输出和帧率计算功能，提供统一的调试和性能监控接口
 */
class Debug {
private:
    // 调试输出相关
    bool print_enabled;  // 是否启用打印功能
    std::ostringstream buffer;  // 内部缓冲区
    
    // 性能监控相关
    std::chrono::high_resolution_clock::time_point start_time;  // 程序开始时间
    std::chrono::high_resolution_clock::time_point last_frame_time;  // 上一帧时间
    std::chrono::high_resolution_clock::time_point current_frame_time;  // 当前帧时间
    
    // 帧率计算相关
    std::deque<std::chrono::duration<double>> frame_times;  // 帧时间历史记录
    const int fps_history_size = 100;  // 保留最近100帧的时间
    
    // 性能统计变量
    double total_processing_time = 0.0;  // 总处理时间
    double max_fps = 0.0;  // 最高帧率
    double min_fps = 999999.0;  // 最低帧率
    int frame_count = 0;  // 帧计数器
    
    // 日志文件相关
    std::ofstream log_file;  // 性能日志文件
    std::string log_filename;  // 日志文件名

public:
    /**
     * @brief 构造函数
     * @param config_file_path 配置文件路径，默认为"config/config.json"
     */
    Debug(const std::string& config_file_path = "config/config.json");
    
    /**
     * @brief 析构函数
     */
    ~Debug();
    
    // ========== 调试输出功能 ==========
    
    /**
     * @brief 从配置文件加载Print_Mode设置
     * @param config_file_path 配置文件路径
     * @return 是否启用打印模式
     */
    bool load_print_mode(const std::string& config_file_path);
    
    /**
     * @brief 重新加载配置文件设置
     * @param config_file_path 配置文件路径
     */
    void reload_config(const std::string& config_file_path = "config/config.json");
    
    /**
     * @brief 手动设置打印模式
     * @param enabled 是否启用打印
     */
    void set_print_mode(bool enabled);
    
    /**
     * @brief 获取当前打印模式状态
     * @return 是否启用打印
     */
    bool is_print_enabled() const;
    
    /**
     * @brief 模板函数：处理各种类型的输出
     * @param value 要输出的值
     * @return 当前对象引用，支持链式调用
     */
    template<typename T>
    Debug& operator<<(const T& value) {
        if (print_enabled) {
            buffer << value;
        }
        return *this;
    }
    
    /**
     * @brief 特殊处理：处理std::endl等流操作符
     * @param manip 流操作符函数指针
     * @return 当前对象引用
     */
    Debug& operator<<(std::ostream& (*manip)(std::ostream&));
    
    /**
     * @brief 强制输出：即使Print_Mode为false也会输出
     * @param value 要输出的值
     * @return 当前对象引用
     */
    template<typename T>
    Debug& force_output(const T& value) {
        std::cout << value;
        return *this;
    }
    
    /**
     * @brief 强制输出并换行
     * @param value 要输出的值
     */
    template<typename T>
    void force_outputln(const T& value) {
        std::cout << value << std::endl;
    }
    
    // ========== 性能监控功能 ==========
    
    /**
     * @brief 开始新帧的处理
     */
    void start_frame();
    
    /**
     * @brief 开始图像处理计时
     */
    void start_processing();
    
    /**
     * @brief 结束图像处理计时
     */
    void end_processing();
    
    /**
     * @brief 获取当前帧率
     * @return 当前帧率
     */
    double get_current_fps() const;
    
    /**
     * @brief 获取平均帧率
     * @return 平均帧率
     */
    double get_average_fps() const;
    
    /**
     * @brief 获取最高帧率
     * @return 最高帧率
     */
    double get_max_fps() const;
    
    /**
     * @brief 获取最低帧率
     * @return 最低帧率
     */
    double get_min_fps() const;
    
    /**
     * @brief 获取当前帧数
     * @return 当前帧数
     */
    int get_frame_count() const;
    
    /**
     * @brief 获取平均处理时间（毫秒）
     * @return 平均处理时间
     */
    double get_average_processing_time_ms() const;
    
    /**
     * @brief 获取总运行时间（秒）
     * @return 总运行时间
     */
    double get_total_runtime_seconds() const;
    
    /**
     * @brief 检查是否需要输出性能统计（每100帧）
     * @return 是否需要输出
     */
    bool should_log_performance() const;
    
    /**
     * @brief 输出性能统计到日志文件
     */
    void log_performance();
    
    /**
     * @brief 输出最终性能报告
     */
    void log_final_report();
    
    /**
     * @brief 获取日志文件名
     * @return 日志文件名
     */
    std::string get_log_filename() const;
};

// 全局调试对象
extern Debug debug;

} // namespace common

#endif // DEBUG_HPP 