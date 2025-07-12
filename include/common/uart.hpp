#pragma once
/**
 ********************************************************************************************************
 *                                               示例代码
 *                                             EXAMPLE  CODE
 *
 *                      (c) Copyright 2024; SaiShu.Lcc.; Leo;
 *https://bjsstech.com 版权所属[SASU-北京赛曙科技有限公司]
 *
 *            The code is for internal use only, not for commercial
 *transactions(开源学习,请勿商用). The code ADAPTS the corresponding hardware
 *circuit board(代码适配百度Edgeboard-智能汽车赛事版), The specific details
 *consult the professional(欢迎联系我们,代码持续更正，敬请关注相关开源渠道).
 *********************************************************************************************************
 * @file uart.hpp
 *
 * @author Leo
 * @brief 上下位机串口通信协议
 * @version 0.1
 * @date 2023-12-26
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "common.hpp"
#include <iostream>               // 输入输出类
#include <libserial/SerialPort.h> // 串口通信
#include <math.h>                 // 数学函数类
#include <stdint.h>               // 整型数据类
#include <string.h>
#include <thread>
#include <memory>

using namespace LibSerial;
using namespace std;

// USB通信帧
#define USB_FRAME_HEAD 0x42 // USB通信帧头
#define USB_FRAME_LENMIN 4  // USB通信帧最短字节长度
#define USB_FRAME_LENMAX 12 // USB通信帧最长字节长度

// USB通信地址
#define USB_ADDR_CARCTRL 1 // 智能车速度+方向控制
#define USB_ADDR_BUZZER 4  // 蜂鸣器音效控制
#define USB_ADDR_LED 5     // LED灯效控制
#define USB_ADDR_KEY 6     // 按键信息

// PWM舵机相关常量
#define PWMSERVOMID 1500   // 舵机中位值

class Uart {
private:
  /**
   * @brief 串口通信结构体
   *
   */
  typedef struct {
    bool start;                           // 开始接收标志
    uint8_t index;                        // 接收序列
    uint8_t buffRead[USB_FRAME_LENMAX];   // 临时缓冲数据
    uint8_t buffFinish[USB_FRAME_LENMAX]; // 校验成功数据
  } SerialStruct;

  std::unique_ptr<std::thread> threadRec; // 串口接收子线程
  std::shared_ptr<SerialPort> serialPort = nullptr;
  std::string portName; // 端口名字
  bool isOpen = false;
  SerialStruct serialStr; // 串口通信数据结构体

  /**
   * @brief 32位数据内存对齐/联合体
   *
   */
  typedef union {
    uint8_t buff[4];
    float float32;
    int int32;
  } Bit32Union;

  /**
   * @brief 16位数据内存对齐/联合体
   *
   */
  typedef union {
    uint8_t buff[2];
    int int16;
    uint16_t uint16;
  } Bit16Union;

  /**
   * @brief 串口接收字节数据
   *
   * @param charBuffer
   * @param msTimeout
   * @return int
   */
  int receiveBytes(unsigned char &charBuffer, size_t msTimeout = 0);

  /**
   * @brief 串口发送字节数据
   *
   * @param data
   * @return int
   */
  int transmitByte(unsigned char data);

public:
  // 定义构造函数
  Uart(const std::string &port);
  // 定义析构函数
  ~Uart();
  
  bool keypress = false; // 按键

  /**
   * @brief 蜂鸣器音效
   *
   */
  enum Buzzer {
    BUZZER_OK = 0,   // 确认
    BUZZER_WARNNING, // 报警
    BUZZER_FINISH,   // 完成
    BUZZER_DING,     // 提示
    BUZZER_START,    // 开机
  };

  /**
   * @brief 启动串口通信
   *
   * @param port 串口号
   * @return int
   */
  int open(void);

  /**
   * @brief 启动接收子线程
   *
   */
  void startReceive(void);

  /**
   * @brief 关闭串口通信
   *
   */
  void close(void);

  /**
   * @brief 串口接收校验
   *
   */
  void receiveCheck(void);

  /**
   * @brief 串口通信协议数据转换
   */
  void dataTransform(void);

  /**
   * @brief 速度+方向控制
   *
   * @param speed 速度：m/s
   * @param servo 方向：PWM（500~2500）
   */
  void carControl(float speed, uint16_t servo);

  /**
   * @brief 蜂鸣器音效控制
   *
   * @param sound
   */
  void buzzerSound(Buzzer sound);
};
