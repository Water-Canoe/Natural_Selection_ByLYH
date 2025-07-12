#include "common/debug.hpp"

namespace common {

// 全局调试对象实例
Debug debug;

// ========== 构造函数和析构函数 ==========

Debug::Debug(const std::string& config_file_path) {
    // 初始化时间
    start_time = std::chrono::high_resolution_clock::now();
    last_frame_time = start_time;
    current_frame_time = start_time;
    
    // 加载配置文件
    print_enabled = load_print_mode(config_file_path);
    
    // 创建性能日志文件
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << "performance_log_" << std::put_time(std::localtime(&time_t), "%Y%m%d_%H%M%S") << ".txt";
    log_filename = ss.str();
    
    log_file.open(log_filename);
    if (log_file.is_open()) {
        log_file << "=== 智能车视觉巡线性能日志 ===" << std::endl;
        log_file << "开始时间: " << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S") << std::endl;
        log_file << "调试输出模式: " << (print_enabled ? "启用" : "禁用") << std::endl;
        log_file << "=================================" << std::endl << std::endl;
    }
}

Debug::~Debug() {
    if (log_file.is_open()) {
        log_final_report();
        log_file.close();
    }
}

// ========== 调试输出功能实现 ==========

bool Debug::load_print_mode(const std::string& config_file_path) {
    // 尝试多个可能的配置文件路径
    std::vector<std::string> possible_paths = {
        config_file_path,  // 原始路径
        "../../config/config.json",  // 从build/bin/目录
        "../config/config.json",     // 从build/目录
        "config/config.json"         // 相对路径
    };
    
    for (const auto& path : possible_paths) {
        try {
            std::ifstream file(path);
            if (file.is_open()) {
                nlohmann::json config;
                file >> config;
                
                // 检查Print_Mode字段是否存在
                if (config.contains("Print_Mode")) {
                    bool result = config["Print_Mode"].get<bool>();
                    std::cout << "成功从 " << path << " 加载配置文件，Print_Mode: " << (result ? "true" : "false") << std::endl;
                    return result;
                } else {
                    std::cerr << "警告：配置文件 " << path << " 中未找到Print_Mode字段" << std::endl;
                    continue;
                }
            }
        } catch (const std::exception& e) {
            // 静默处理，继续尝试下一个路径
            continue;
        }
    }
    
    // 所有路径都失败，输出警告并返回默认值
    std::cerr << "警告：无法从任何路径找到有效的配置文件，默认禁用调试输出" << std::endl;
    std::cerr << "尝试的路径：" << std::endl;
    for (const auto& path : possible_paths) {
        std::cerr << "  - " << path << std::endl;
    }
    return false;
}

void Debug::reload_config(const std::string& config_file_path) {
    print_enabled = load_print_mode(config_file_path);
}

void Debug::set_print_mode(bool enabled) {
    print_enabled = enabled;
}

bool Debug::is_print_enabled() const {
    return print_enabled;
}

Debug& Debug::operator<<(std::ostream& (*manip)(std::ostream&)) {
    if (print_enabled) {
        // 将缓冲区内容输出到标准输出
        std::cout << buffer.str();
        // 应用流操作符（如endl）
        manip(std::cout);
        // 清空缓冲区
        buffer.str("");
        buffer.clear();
    }
    return *this;
}

// ========== 性能监控功能实现 ==========

void Debug::start_frame() {
    current_frame_time = std::chrono::high_resolution_clock::now();
    frame_count++;
}

void Debug::start_processing() {
    // 这个函数主要用于标记处理开始，实际计时在end_processing中计算
}

void Debug::end_processing() {
    // 计算图像处理时间
    auto process_end = std::chrono::high_resolution_clock::now();
    auto process_duration = std::chrono::duration<double>(process_end - current_frame_time);
    total_processing_time += process_duration.count();
    
    // 记录上次处理时间（毫秒）
    last_processing_time_ms = process_duration.count() * 1000.0;
    
    // 计算帧率 - 使用当前帧时间和上一帧时间
    if (frame_count > 1) {
        auto frame_duration = std::chrono::duration<double>(current_frame_time - last_frame_time);
        double current_fps = 1.0 / frame_duration.count();
        
        // 更新帧时间历史
        frame_times.push_back(frame_duration);
        if (frame_times.size() > fps_history_size) {
            frame_times.pop_front();
        }
        
        // 更新最高和最低帧率
        if (current_fps > max_fps) max_fps = current_fps;
        if (current_fps < min_fps && current_fps > 0) min_fps = current_fps;
    }
    
    // 更新上一帧时间
    last_frame_time = current_frame_time;
}

double Debug::get_current_fps() const {
    if (frame_count < 2) return 0.0;
    
    // 使用当前时间和上一帧时间计算帧率
    auto now = std::chrono::high_resolution_clock::now();
    auto frame_duration = std::chrono::duration<double>(now - last_frame_time);
    double duration_seconds = frame_duration.count();
    
    // 防止除零错误和异常值
    if (duration_seconds <= 0.0 || duration_seconds > 10.0) {
        return 0.0;  // 返回0表示无法计算
    }
    
    return 1.0 / duration_seconds;
}

double Debug::get_average_fps() const {
    if (frame_times.empty()) return 0.0;
    double total_time = 0.0;
    for (const auto& time : frame_times) {
        total_time += time.count();
    }
    return frame_times.size() / total_time;
}

double Debug::get_max_fps() const {
    return max_fps;
}

double Debug::get_min_fps() const {
    return min_fps;
}

int Debug::get_frame_count() const {
    return frame_count;
}

double Debug::get_average_processing_time_ms() const {
    if (frame_count == 0) return 0.0;
    return (total_processing_time / frame_count) * 1000.0;
}

int Debug::get_last_processing_time_ms() const {
    return static_cast<int>(last_processing_time_ms);
}

double Debug::get_total_runtime_seconds() const {
    auto total_time = std::chrono::duration<double>(current_frame_time - start_time);
    return total_time.count();
}

bool Debug::should_log_performance() const {
    return frame_count % 100 == 0 && frame_count > 0;
}

void Debug::log_performance() {
    if (!log_file.is_open()) return;
    
    log_file << "=== 性能统计 (第" << frame_count << "帧) ===" << std::endl;
    log_file << "当前帧率: " << std::fixed << std::setprecision(2) << get_current_fps() << " FPS" << std::endl;
    log_file << "平均帧率: " << std::fixed << std::setprecision(2) << get_average_fps() << " FPS" << std::endl;
    log_file << "最高帧率: " << std::fixed << std::setprecision(2) << get_max_fps() << " FPS" << std::endl;
    log_file << "最低帧率: " << std::fixed << std::setprecision(2) << get_min_fps() << " FPS" << std::endl;
    log_file << "平均处理时间: " << std::fixed << std::setprecision(3) << get_average_processing_time_ms() << " ms" << std::endl;
    log_file << "总运行时间: " << std::fixed << std::setprecision(2) << get_total_runtime_seconds() << " 秒" << std::endl;
    log_file << "=================================" << std::endl << std::endl;
    
    // 强制刷新文件缓冲区，确保数据写入磁盘
    log_file.flush();
}

void Debug::log_final_report() {
    if (!log_file.is_open()) return;
    
    auto end_time = std::chrono::high_resolution_clock::now();
    auto total_runtime = std::chrono::duration<double>(end_time - start_time);
    double final_avg_fps = frame_count / total_runtime.count();
    
    log_file << "\n=== 最终性能报告 ===" << std::endl;
    log_file << "总帧数: " << frame_count << std::endl;
    log_file << "总运行时间: " << std::fixed << std::setprecision(2) << total_runtime.count() << " 秒" << std::endl;
    log_file << "平均帧率: " << std::fixed << std::setprecision(2) << final_avg_fps << " FPS" << std::endl;
    log_file << "最高帧率: " << std::fixed << std::setprecision(2) << get_max_fps() << " FPS" << std::endl;
    log_file << "最低帧率: " << std::fixed << std::setprecision(2) << get_min_fps() << " FPS" << std::endl;
    log_file << "平均处理时间: " << std::fixed << std::setprecision(3) << get_average_processing_time_ms() << " ms" << std::endl;
    log_file << "===================" << std::endl;
    
    // 记录结束时间
    auto end_time_t = std::chrono::system_clock::to_time_t(end_time);
    log_file << "结束时间: " << std::put_time(std::localtime(&end_time_t), "%Y-%m-%d %H:%M:%S") << std::endl;
    
    std::cout << "性能日志已保存到: " << log_filename << std::endl;
}

std::string Debug::get_log_filename() const {
    return log_filename;
}

} // namespace common 