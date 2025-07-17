#include "common.hpp"
#include "recognition.hpp"
#include "control.hpp"


using namespace std;
using namespace control;
using namespace recognition;
using namespace common;

namespace task{



void Show_Draw_Line_Task(Tracking& tracking, Element& element, ControlCenter& control_center)
{
    
    // 绘制中线、左右边线
    for(size_t i = 0;i < element._middle_line.size();i++)
    {
        cv::circle(tracking._draw_frame,
                   cv::Point(element._middle_line[i].x,element._middle_line[i].y),
                   1,  // 圆圈半径
                   cv::Scalar(0,255,255),  // 绿色 (B,G,R)
                   -1);  // 实心圆
        cv::circle(tracking._draw_frame,
                   cv::Point(element._left_line[i].x,element._left_line[i].y),
                   1,  // 圆圈半径
                   cv::Scalar(255,0,0),  // 蓝色 (B,G,R)
                   -1);  // 实心圆
        cv::circle(tracking._draw_frame,
                   cv::Point(element._right_line[i].x,element._right_line[i].y),
                   1,  // 圆圈半径
                   cv::Scalar(0,0,255),  // 红色 (B,G,R)
                   -1);  // 实心圆
        // 绘制贝塞尔中心线
        if(i < control_center._center_edge.size())
        {
            cv::circle(tracking._draw_frame,
                cv::Point(control_center._center_edge[i].x,control_center._center_edge[i].y),
                1,
                cv::Scalar(0,0,0),
                -1);
        }
    }

    // 绘制角点
    cv::circle(tracking._draw_frame,
               cv::Point(tracking.Get_Corner(LEFT_DOWN).x,tracking.Get_Corner(LEFT_DOWN).y),
               2,  // 圆圈半径
               cv::Scalar(0,255,0),  // 绿色 (B,G,R)
               -1);  // 实心圆
    cv::circle(tracking._draw_frame,
               cv::Point(tracking.Get_Corner(RIGHT_DOWN).x,tracking.Get_Corner(RIGHT_DOWN).y),
               2,  // 圆圈半径
               cv::Scalar(0,255,0),  // 绿色 (B,G,R)
               -1);  // 实心圆
    cv::circle(tracking._draw_frame,
               cv::Point(tracking.Get_Corner(LEFT_UP).x,tracking.Get_Corner(LEFT_UP).y),
               2,  // 圆圈半径
               cv::Scalar(0,255,255),  // 绿色 (B,G,R)
               -1);  // 实心圆
    cv::circle(tracking._draw_frame,
               cv::Point(tracking.Get_Corner(RIGHT_UP).x,tracking.Get_Corner(RIGHT_UP).y),
               2,  // 圆圈半径
               cv::Scalar(0,255,255),  // 绿色 (B,G,R)
               -1);  // 实心圆
    // 绘制控制中心
    cv::circle(tracking._draw_frame,
               cv::Point(control_center._control_center,tracking.Get_Height()/2),
               4,       
               cv::Scalar(0,0,255),
               -1);
}



void Show_Windows_Task(Tracking& tracking,string scene, Motion& motion, ControlCenter& control_center, Display& display, char& key,bool& is_paused)
{
    int control_point = control_center._control_center; 
    double sigma_center = control_center._sigma_center;
    float speed = motion._speed;
    uint16_t pwm = motion._servo_pwm;
    string debug_mode = tracking._camera._debug_mode;
    // 获取图像并检查是否为空
    cv::Mat original_frame = tracking._camera.Get_Frame();
    cv::Mat binary_frame = tracking._camera.Get_Binary_Frame();
    cv::Mat draw_frame = tracking._draw_frame;

    // 在图像上显示帧率信息
    if (!draw_frame.empty()) {
        // 设置文本参数
        int font_face = cv::FONT_HERSHEY_SIMPLEX;
        double font_scale = 0.5;
        int thickness = 1;
        cv::Scalar text_color(255, 255, 0); 
        // 准备显示的文本
        string scene_text = "Scene: " + scene;
        string avg_fps_text = "Avg FPS: " + to_string(static_cast<int>(debug.get_average_fps()));
        string frame_text = "Frame: " + to_string(debug.get_frame_count());
        string control_center = "Control: " + to_string(static_cast<int>(control_point)) + " " + to_string(static_cast<int>(sigma_center));
        string uart = "Speed: " + to_string(static_cast<float>(speed)) + " PWM: " + to_string(static_cast<uint16_t>(pwm));
        // 计算文本位置
        int y_offset = 30;
        cv::Point text_pos(10, y_offset);
        // 绘制文本
        cv::putText(draw_frame, scene_text, text_pos, font_face, font_scale, text_color, thickness);
        cv::putText(draw_frame, avg_fps_text, cv::Point(10, y_offset + 25), font_face, font_scale, text_color, thickness);
        cv::putText(draw_frame, frame_text, cv::Point(10, y_offset + 50), font_face, font_scale, text_color, thickness);
        cv::putText(draw_frame, control_center, cv::Point(250, y_offset + 25), font_face, font_scale, text_color, thickness);
        cv::putText(draw_frame, uart, cv::Point(250, y_offset + 50), font_face, font_scale, text_color, thickness);
    }
    
    display.show_image(0, original_frame);
    display.show_image(1, binary_frame);
    display.show_image(2, draw_frame);
    
    // 根据暂停状态设置不同的延时
    if(!is_paused)
    {
        // 获取摄像头实际帧率
        double camera_fps = tracking._camera.Get_Actual_FPS();
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
        
        if(debug_mode == "video" || debug_mode == "picture")
        {
            this_thread::sleep_for(chrono::milliseconds(tracking._camera._video_delay));   //video、picture模式使用，越小播放视频越快
        }
        else
            this_thread::sleep_for(chrono::milliseconds(delay_ms));  //camera模式使用，根据帧率动态调整播放速度
    }
    else
    {
        this_thread::sleep_for(chrono::milliseconds(100)); // 暂停时减少CPU占用
    }

    if(key == 'q' || key == 'Q')
    {
        debug << "用户退出程序" << std::endl;
        return;
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



}