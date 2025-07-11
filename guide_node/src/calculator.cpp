/**
 * @file calculator.cpp
 * @author Your Name
 * @brief 增强版计算器类的具体实现
 * @version 1.0
 * @date 2024-03-20
 */

#include "calculator.hpp"
#include <sstream>
#include <stdexcept>
#include <chrono>
#include <iomanip>
#include <cmath>

// 构造函数实现
Calculator::Calculator() : operationCount(0), currentState(CalculatorState::IDLE), lastError(ErrorCode::NONE) {
    config.enableHistory = true;
    config.maxHistorySize = 100;
    config.precision = 0.000001;
    operationHistory = "计算器初始化完成\n";
}

Calculator::Calculator(const CalculatorConfig& config) 
    : operationCount(0), currentState(CalculatorState::IDLE), lastError(ErrorCode::NONE), config(config) {
    operationHistory = "计算器初始化完成\n";
}

// 析构函数实现
Calculator::~Calculator() {
    // 清理资源
    detailedHistory.clear();
    memory.clear();
}

// 静态方法实现
bool Calculator::isValidNumber(double num) {
    return !std::isnan(num) && !std::isinf(num);
}

// 基本运算实现
double Calculator::add(double a, double b) {
    updateState(CalculatorState::CALCULATING);
    if (!isValidNumber(a) || !isValidNumber(b)) {
        handleError(ErrorCode::INVALID_INPUT);
        return 0.0;
    }
    
    double result = a + b;
    std::stringstream ss;
    ss << std::fixed << std::setprecision(6) << a << " + " << b << " = " << result;
    logOperation(ss.str());
    
    lastResult = result;
    updateState(CalculatorState::IDLE);
    return result;
}

double Calculator::subtract(double a, double b) {
    updateState(CalculatorState::CALCULATING);
    if (!isValidNumber(a) || !isValidNumber(b)) {
        handleError(ErrorCode::INVALID_INPUT);
        return 0.0;
    }
    
    double result = a - b;
    std::stringstream ss;
    ss << std::fixed << std::setprecision(6) << a << " - " << b << " = " << result;
    logOperation(ss.str());
    
    lastResult = result;
    updateState(CalculatorState::IDLE);
    return result;
}

double Calculator::multiply(double a, double b) {
    updateState(CalculatorState::CALCULATING);
    if (!isValidNumber(a) || !isValidNumber(b)) {
        handleError(ErrorCode::INVALID_INPUT);
        return 0.0;
    }
    
    double result = a * b;
    std::stringstream ss;
    ss << std::fixed << std::setprecision(6) << a << " * " << b << " = " << result;
    logOperation(ss.str());
    
    lastResult = result;
    updateState(CalculatorState::IDLE);
    return result;
}

double Calculator::divide(double a, double b) {
    updateState(CalculatorState::CALCULATING);
    if (!isValidNumber(a) || !isValidNumber(b)) {
        handleError(ErrorCode::INVALID_INPUT);
        return 0.0;
    }
    
    if (std::abs(b) < config.precision) {
        handleError(ErrorCode::DIVISION_BY_ZERO);
        throw std::runtime_error("除数不能为零！");
    }
    
    double result = a / b;
    std::stringstream ss;
    ss << std::fixed << std::setprecision(6) << a << " / " << b << " = " << result;
    logOperation(ss.str());
    
    lastResult = result;
    updateState(CalculatorState::IDLE);
    return result;
}

// Getter方法实现
std::string Calculator::getHistory() const {
    return operationHistory;
}

CalculatorState Calculator::getState() const {
    return currentState;
}

const CalculatorConfig& Calculator::getConfig() const {
    return config;
}

int Calculator::getOperationCount() const {
    return operationCount;
}

// Setter方法实现
void Calculator::setConfig(const CalculatorConfig& newConfig) {
    config = newConfig;
    if (config.maxHistorySize < detailedHistory.size()) {
        detailedHistory.resize(config.maxHistorySize);
    }
}

void Calculator::clearHistory() {
    operationHistory.clear();
    detailedHistory.clear();
    operationCount = 0;
}

// 私有辅助方法实现
void Calculator::logOperation(const std::string& operation) {
    if (!config.enableHistory) return;
    
    operationCount++;
    operationHistory += std::to_string(operationCount) + ". " + operation + "\n";
    
    // 记录详细历史
    OperationLog log;
    log.operation = operation;
    log.timestamp = getCurrentTimestamp();
    detailedHistory.push_back(log);
    
    // 限制历史记录大小
    if (detailedHistory.size() > config.maxHistorySize) {
        detailedHistory.erase(detailedHistory.begin());
    }
}

void Calculator::updateState(CalculatorState newState) {
    currentState = newState;
}

void Calculator::handleError(ErrorCode code) {
    lastError = code;
    updateState(CalculatorState::ERROR);
}

std::string Calculator::getCurrentTimestamp() const {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    return ss.str();
} 