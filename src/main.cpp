#include "recognition.hpp"  //巡线识别
#include "control.hpp"      //运动控制
#include "common.hpp"       //通用库
#include "opencv2/opencv.hpp"
#include <thread>   //多线程库，提供多线程支持
#include <chrono>   //时间库，提供时间支持

using namespace common;
using namespace recognition;
using namespace std;
using namespace control;
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
    // int motion_cnt = 0;  // 暂时注释掉未使用的变量
    bool is_paused = false; //暂停状态
    string scene = "ZebraScene";
    float middle_error = 0;

    // ========================================== 初始化串口 ==========================================
    // shared_ptr<Uart> uart = make_shared<Uart>("/dev/ttyUSB0");
    // int ret = uart->open();
    // if(ret != 0)
    // {
    //     debug.force_outputln("串口打开失败");
    //     return -1;
    // }
    // uart->startReceive();   //启动串口接收线程

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
    // if(motion._motion_enable)
    // {
    //     debug.force_outputln("运动控制已启用,等待按键发车");
    //     uart -> buzzerSound(uart -> BUZZER_OK);
    //     while(!uart -> keypress)
    //         waitKey(300);
    //     while(ret < 10)
    //     {
    //         uart -> carControl(0,PWMSERVOMID);
    //         waitKey(300);
    //         ret ++;
    //     }
    //     uart -> keypress = false;
    //     uart -> buzzerSound(uart -> BUZZER_START);
    //     debug.force_outputln("发车成功");
    // }

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
                case recognition::Scene::CrossScene:
                    scene = "CrossScene";
                    element._crossroad_flag = true;
                    debug.force_outputln("检测到场景：十字路口"+to_string(debug.get_frame_count()));
                    break;
                case recognition::Scene::RingScene:
                    scene = "RingScene";
                    debug.force_outputln("检测到场景：环岛"+to_string(debug.get_frame_count()));
                    break;
                case recognition::Scene::ObstacleScene:
                    scene = "ObstacleScene";
                    element._obstaclee_flag = true;
                    debug.force_outputln("检测到场景：障碍物 "+to_string(debug.get_frame_count()));
                    break;
                case recognition::Scene::NolmalScene:
                case recognition::Scene::BridgeScene:
                case recognition::Scene::CateringScene:
                case recognition::Scene::LaybyScene:
                case recognition::Scene::ParkingScene:
                default: scene = "NormalScene";
                    // 这些场景暂时不处理
                    break;
            }
            middle_error = element.Get_Middle_Error(tracker);  // 获取中心点误差
            //control_center.Fitting(tracker); // 拟合中心线
            element.Draw_Edge(tracker);  // 绘制边缘
            debug.end_processing();   // 结束图像处理计时
        }

        // ========================================== 运动控制 ==========================================
        // if(motion._motion_enable && motion_cnt > 30)
        // {
        //     if(scene == recognition::Scene::ZebraScene) motion._speed = 0;
        //     else if(scene == recognition::Scene::RingScene) motion._speed = motion._speed_ring;
        //     else if(scene == recognition::Scene::ObstacleScene) motion._speed = motion._speed_obstacle;
        //     else motion.Speed_Control(true,false,control_center,tracker);

        //     motion.Pose_Control(control_center._control_center,tracker);    // 姿态控制
        //     uart -> carControl(motion._speed,motion._servo_pwm);    // 发送速度和舵机PWM
        // }
        // else
        //     motion_cnt ++;
        // ========================================== 图像显示 ==========================================
        // 获取图像并检查是否为空
        cv::Mat original_frame = tracker._camera.Get_Frame();
        cv::Mat binary_frame = tracker._camera.Get_Binary_Frame();
        cv::Mat draw_frame = tracker._draw_frame;

        // 在图像上显示帧率信息
        if (!draw_frame.empty()) {
            // 设置文本参数
            int font_face = cv::FONT_HERSHEY_SIMPLEX;
            double font_scale = 0.4;
            int thickness = 1;
            cv::Scalar text_color(0, 255, 0);  // 绿色
            // 准备显示的文本
            string scene_text = "Scene: " + scene;
            string avg_fps_text = "Avg FPS: " + to_string(static_cast<int>(debug.get_average_fps()));
            string frame_text = "Frame: " + to_string(debug.get_frame_count());
            // 计算文本位置
            int y_offset = 30;
            cv::Point text_pos(10, y_offset);
            
            // 绘制文本
            cv::putText(draw_frame, scene_text, text_pos, font_face, font_scale, text_color, thickness);
            cv::putText(draw_frame, avg_fps_text, cv::Point(10, y_offset + 25), font_face, font_scale, text_color, thickness);
            cv::putText(draw_frame, frame_text, cv::Point(10, y_offset + 50), font_face, font_scale, text_color, thickness);
        }
        
        display.show_image(0, original_frame);
        display.show_image(1, binary_frame);
        display.show_image(2, draw_frame);
        
        // 根据暂停状态设置不同的延时
        if(!is_paused)
        {
            // 获取摄像头实际帧率
            double camera_fps = tracker._camera.Get_Actual_FPS();
            if(camera_fps <= 0) {
                camera_fps = 10.0; // 如果获取失败，使用默认值
            }
            
            // 动态计算延时，使用摄像头实际帧率
            int processing_time_ms = debug.get_last_processing_time_ms();
            int target_fps = static_cast<int>(camera_fps);
            int delay_ms = max(1, 1000 / target_fps - processing_time_ms);
            
            // 限制延时范围，避免负值或过大值
            delay_ms = max(1, min(delay_ms, 50));
            
            // 输出调试信息（可选）
            static int frame_counter = 0;
            if(frame_counter++ % 100 == 0) { // 每100帧输出一次
                debug << "摄像头帧率: " << camera_fps << " FPS, 处理时间: " 
                      << processing_time_ms << " ms, 延时: " << delay_ms << " ms" << std::endl;
            }
            
            this_thread::sleep_for(chrono::milliseconds(delay_ms));
        }
        else
        {
            this_thread::sleep_for(chrono::milliseconds(100)); // 暂停时减少CPU占用
        }

        char key = cv::waitKey(1);
        if(key == 'q' || key == 'Q')
        {
            debug << "用户退出程序" << std::endl;
            break;
        }
        if(key == ' ')  //暂停/继续
        {
            is_paused = !is_paused;
            if(is_paused)
            {
                debug << "程序已暂停 - 按空格键继续" << std::endl;
            }
            else
            {
                debug << "程序已继续运行" << std::endl;
            }
        }
        if(key == 's' || key == 'S')
        {
            // 检查绘制图像是否为空再保存
            if(!draw_frame.empty()) {
                string filename = "frame_" + to_string(debug.get_frame_count()) + ".jpg";
                imwrite(filename, draw_frame);
                debug << "保存图像：" << filename << std::endl;
            } else {
                debug.force_outputln("错误：无法保存图像，绘制图像为空");
            }
        }
        
        // 检查是否需要输出性能统计
        if(debug.should_log_performance()) {
            debug.log_performance();
        }
    }
    
    //释放资源
    cv::destroyAllWindows();
    return 0;
}


