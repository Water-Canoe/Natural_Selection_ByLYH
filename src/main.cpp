#include "recognition.hpp"  //巡线识别
#include "control.hpp"      //运动控制
#include "common.hpp"       //通用库
#include "task.hpp"
#include "opencv2/opencv.hpp"
#include <thread>   //多线程库，提供多线程支持
#include <chrono>   //时间库，提供时间支持

using namespace common;
using namespace recognition;
using namespace std;
using namespace control;
using namespace task;
using namespace cv;

int main()
{
    // ========================================== 构造对象 ==========================================
    // 创建对象

    Tracking tracker;   //创建巡线对象
    Element element;    //创建元素对象
    Display display;    //创建显示对象
    ControlCenter control_center; //创建控制中心对象
    Motion motion;       //创建运动对象
    int motion_cnt = 0;  // 暂时注释掉未使用的变量
    
    shared_ptr<Uart> uart = nullptr;
    int ret;

    bool is_paused = false; //暂停状态
    string scene = "ZebraScene";
    float middle_error = 0;

    // ========================================== 初始化串口 ==========================================
    if(motion._motion_enable)
    {
        uart = make_shared<Uart>("/dev/ttyUSB0");
        ret = uart->open();
        if(ret != 0)
        {
            debug.force_outputln("串口打开失败");
            return -1;
        }
        uart->startReceive();   //启动串口接收线程
    }
    // ========================================== 初始化摄像头及窗口 ==========================================
    // 检查摄像头初始化状态
    if(!tracker._camera.Is_Initialized())
    {
        cerr<<"摄像头初始化失败"<<endl;
        return -1;
    }
    // 显示调试输出状态
    debug << "调试输出模式已" << (debug.is_print_enabled() ? "启用" : "禁用") << std::endl;
    // 显示摄像头信息
    tracker._camera.Print_Camera_Info();
    // 创建多窗口显示系统
    display.add_window(0, "原始图像");
    display.add_window(1, "二值化图像");
    display.add_window(2, "巡线路径");

    // ========================================== 运动控制初始化 ==========================================
    if(motion._motion_enable)
    {
        debug.force_outputln("运动控制已启用,等待按键发车");
        uart -> buzzerSound(uart -> BUZZER_OK);
        while(!uart -> keypress)
            waitKey(300);
        while(ret < 10)
        {
            uart -> carControl(0,PWMSERVOMID);
            waitKey(300);
            ret ++;
        }
        uart -> keypress = false;
        uart -> buzzerSound(uart -> BUZZER_START);
        debug.force_outputln("发车成功");
    }

    // ========================================== 主循环 ==========================================
    while(true)
    {
        // 开始新帧的处理
        debug.start_frame();
        if(!is_paused)        // 只有在非暂停状态下才进行图像处理
        {
            debug.start_processing();   // 开始图像处理计时
            if(!tracker._camera.Capture())   // 捕获图像
            {
                debug.force_outputln("图像捕获失败，跳过此帧");
                continue;
            }
            tracker.Picture_Process();   // 图像处理
            tracker._camera.Get_Frame();
            tracker._camera.Get_Binary_Frame();
            // ========================================== 赛道巡线获取控制点信息 ==========================================
            tracker.Track_Recognition(); // 巡线识别
            tracker.Edge_Extract(); // 边缘提取
            switch(element.Recognition_Element(tracker))    // 元素识别
            {
                case recognition::Scene::ZebraScene:
                    element._zebra_cnt ++;
                    if(element._zebra_cnt >= 5)//连着5帧为斑马线再确定
                    {
                        scene = "ZebraScene";
                        element._zebra_cnt = 0;
                        element._zebra_flag = true;
                        debug.force_outputln("检测到场景：斑马线 "+to_string(debug.get_frame_count()));
                    }
                    break;
                    scene = "RingScene";
                    element._ring_flag = true;
                    debug.force_outputln("检测到场景：环岛"+to_string(debug.get_frame_count()));
                    break;
                case recognition::Scene::ObstacleScene:
                    scene = "ObstacleScene";
                    element._obstaclee_flag = true;
                    debug.force_outputln("检测到场景：障碍物 "+to_string(debug.get_frame_count()));
                    break;
                case recognition::Scene::BridgeScene:
                case recognition::Scene::CateringScene:
                case recognition::Scene::LaybyScene:
                case recognition::Scene::ParkingScene:
                case recognition::Scene::NolmalScene:
                default: scene = "NormalScene";
                    // 这些场景暂时不处理
                    break;
            }
            middle_error = element.Get_Middle_Error(tracker);  // 补线及拟合中心线
            control_center.Fitting(tracker,element); // 拟合中心线
            Show_Draw_Line_Task(tracker,element,control_center);
            debug.end_processing();   // 结束图像处理计时
        }

        // ========================================== 运动控制 ==========================================
        if(motion_cnt > 30)
        {
            if(scene == "ZebraScene") motion._speed = 0;
            else if(scene == "RingScene") motion._speed = motion._speed_ring;
            else if(scene == "ObstacleScene") motion._speed = motion._speed_obstacle;
            else motion.Speed_Control(true,false,control_center,tracker);
            motion.Pose_Control(control_center._control_center,tracker);    // 姿态控制
            if(motion._motion_enable)
            {
                uart -> carControl(motion._speed,motion._servo_pwm);    // 发送速度和舵机PWM
            }
        }
        else
            motion_cnt ++;

        // ========================================== 图像显示 ==========================================
        char key = cv::waitKey(1);
        Show_Windows_Task(tracker, scene, motion, control_center, display, key, is_paused);
    }

    //释放资源
    cv::destroyAllWindows();
    return 0;
}


