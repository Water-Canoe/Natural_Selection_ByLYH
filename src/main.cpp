#include "recognition/tracking.hpp"
#include "recognition/element.hpp"
#include "common.hpp"
#include "opencv2/opencv.hpp"
#include <thread>   //多线程库，提供多线程支持
#include <chrono>   //时间库，提供时间支持

using namespace common;
using namespace recognition;
using namespace std;

int main()
{
    // 创建对象
    Tracking tracker;   //创建巡线对象
    Element element;    //创建元素对象
    Display display;    //创建显示对象

    bool is_paused = false;

    // 检查摄像头初始化状态
    if(!tracker._camera.Is_Initialized())
    {
        cerr<<"摄像头初始化失败"<<endl;
        return -1;
    }

    // 显示调试输出状态
    debug << "调试输出模式已" << (debug.is_print_enabled() ? "启用" : "禁用") << std::endl;

    // 创建多窗口显示系统
    display.add_window(0, "原始图像");
    display.add_window(1, "二值化图像");
    display.add_window(2, "巡线路径");

    while(true)
    {
        // 开始新帧的处理
        debug.start_frame();
        
        // 只有在非暂停状态下才进行图像处理
        if(!is_paused)
        {
            // 开始图像处理计时
            debug.start_processing();
            
            tracker._camera.Capture();
            // 图像处理
            tracker.Picture_Process();
            tracker.Track_Recognition(); // 巡线识别
            tracker.Edge_Extract(); // 边缘提取
            //元素识别只能用这个结构，别改
            recognition::Scene scene = element.Recognition_Element(tracker);           
            switch(scene)
            {
                case recognition::Scene::ZebraScene:
                    element._zebra_cnt ++;
                    if(element._zebra_cnt >= 5)//连着5帧为斑马线再确定
                    {
                        element._zebra_cnt = 0;
                        element._zebra_flag = true;
                        debug.force_outputln("检测到场景：斑马线 "+to_string(debug.get_frame_count()));
                    }
                    break;
                case recognition::Scene::CrossScene:
                    debug.force_outputln("检测到场景：十字路口");
                    element._crossroad_flag = true;
                    break;
                case recognition::Scene::RingScene:
                    debug.force_outputln("检测到场景：环岛");
                    break;
                case recognition::Scene::BridgeScene:
                    debug.force_outputln("检测到场景：坡道");
                    break;
                case recognition::Scene::ObstacleScene:
                    debug.force_outputln("检测到场景：障碍物");
                    break;
            }
            element.Draw_Edge(tracker);
            
            // 结束图像处理计时
            debug.end_processing();
        }
        
        // 获取图像并检查是否为空
        cv::Mat original_frame = tracker._camera.Get_Frame();
        cv::Mat binary_frame = tracker._camera.Get_Binary_Frame();
        cv::Mat draw_frame = tracker._draw_frame;
        
        // // 在图像上显示帧率信息
        // if (!draw_frame.empty()) {
        //     // 设置文本参数
        //     int font_face = cv::FONT_HERSHEY_SIMPLEX;
        //     double font_scale = 0.6;
        //     int thickness = 2;
        //     cv::Scalar text_color(0, 255, 0);  // 绿色
        //     cv::Scalar bg_color(0, 0, 0);      // 黑色背景
            
        //     // 准备显示的文本
        //     string fps_text = "FPS: " + to_string(static_cast<int>(debug.get_current_fps()));
        //     string avg_fps_text = "Avg FPS: " + to_string(static_cast<int>(debug.get_average_fps()));
        //     string frame_text = "Frame: " + to_string(debug.get_frame_count());
            
        //     // 计算文本位置
        //     int y_offset = 30;
        //     cv::Point text_pos(10, y_offset);
            
        //     // 绘制背景矩形
        //     cv::rectangle(draw_frame, cv::Point(5, 5), cv::Point(200, 80), bg_color, -1);
            
        //     // 绘制文本
        //     cv::putText(draw_frame, fps_text, text_pos, font_face, font_scale, text_color, thickness);
        //     cv::putText(draw_frame, avg_fps_text, cv::Point(10, y_offset + 25), font_face, font_scale, text_color, thickness);
        //     cv::putText(draw_frame, frame_text, cv::Point(10, y_offset + 50), font_face, font_scale, text_color, thickness);
        // }
        
        display.show_image(0, original_frame);
        display.show_image(1, binary_frame);
        display.show_image(2, draw_frame);
        
        // 根据暂停状态设置不同的延时
        if(!is_paused)
        {
            this_thread::sleep_for(chrono::milliseconds(50)); // 正常运行时的帧率控制
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


