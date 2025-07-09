/**
 * @file calculator.hpp
 * @author Your Name
 * @brief 一个增强版计算器类，展示各种类型的成员变量和访问控制
 * @version 1.0
 * @date 2024-03-20
 */

#pragma once  // 防止重复包含

#include <string>
#include <vector>
#include <map>

// 前向声明
struct OperationResult;

// 计算器配置结构体
struct CalculatorConfig {
    bool enableHistory;      // 是否启用历史记录
    int maxHistorySize;      // 最大历史记录数
    double precision;        // 计算精度
};

// 计算器状态枚举
enum class CalculatorState {
    IDLE,           // 空闲状态
    CALCULATING,    // 计算中
    ERROR           // 错误状态
};

class Calculator {
public:
    // 构造函数
    Calculator();
    explicit Calculator(const CalculatorConfig& config);
    
    // 析构函数
    ~Calculator();
    
    // 静态常量（公有）
    static constexpr double PI = 3.14159265358979323846;
    static constexpr int MAX_OPERATIONS = 1000;
    
    // 公有方法
    double add(double a, double b);
    double subtract(double a, double b);
    double multiply(double a, double b);
    double divide(double a, double b);
    
    // Getter方法
    std::string getHistory() const;
    CalculatorState getState() const;
    const CalculatorConfig& getConfig() const;
    int getOperationCount() const;
    
    // Setter方法
    void setConfig(const CalculatorConfig& config);
    void clearHistory();
    
    // 静态方法
    static bool isValidNumber(double num);
    
private:
    // 私有成员变量
    std::string operationHistory;      // 操作历史
    int operationCount;                // 操作计数
    CalculatorState currentState;      // 当前状态
    CalculatorConfig config;           // 配置信息
    
    // 私有结构体成员
    struct OperationLog {
        std::string operation;         // 操作描述
        double result;                 // 计算结果
        std::string timestamp;         // 时间戳
    };
    std::vector<OperationLog> detailedHistory;  // 详细历史记录
    
    // 私有枚举
    enum class ErrorCode {
        NONE,
        DIVISION_BY_ZERO,
        INVALID_INPUT,
        OVERFLOW
    };
    ErrorCode lastError;              // 最后的错误代码
    
    // 私有辅助方法
    void logOperation(const std::string& operation);
    void updateState(CalculatorState newState);
    void handleError(ErrorCode code);
    std::string getCurrentTimestamp() const;
    
protected:
    // 保护成员变量（供派生类使用）
    std::map<std::string, double> memory;  // 内存存储
    double lastResult;                     // 最后一次计算结果
}; 