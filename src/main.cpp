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
    int frame_count = 0; //帧计数器

    // 检查摄像头初始化状态
    if(!tracker._camera.Is_Initialized())
    {
        cerr<<"摄像头初始化失败"<<endl;
        return -1;
    }

    // 创建多窗口显示系统
    display.add_window(0, "原始图像");
    display.add_window(1, "二值化图像");
    display.add_window(2, "巡线路径");

    while(true)
    {
        // 捕获图像并检查是否成功
        // if(!)
        // {
        //     cout << "读取帧失败/播放结束" << endl;
        //     break;
        // }
        
        // 只有在非暂停状态下才进行图像处理
        if(!is_paused)
        {
            tracker._camera.Capture();
            // 图像处理
            tracker.Picture_Process();
            //tracker.Find_Start_Point(3,10);
            tracker.Track_Recognition();
            tracker.Edge_Extract();
            tracker.Draw_Edge();
            recognition::Scene scene = element.Recognition_Element(tracker);
            switch(scene)
            {
                case recognition::Scene::ZebraScene:
                    cout << "斑马线" << endl;
                    break;
            }
            //corner.Detect_Stable_Corners(tracker.Get_Maze_Edge_Left(), tracker.Get_Maze_Edge_Right(), tracker._draw_frame);
        }

        // 获取图像并检查是否为空
        cv::Mat original_frame = tracker._camera.Get_Frame();
        cv::Mat binary_frame = tracker._camera.Get_Binary_Frame();
        cv::Mat draw_frame = tracker._draw_frame;
        
        display.show_image(0, original_frame);
        display.show_image(1, binary_frame);
        display.show_image(2, draw_frame);
        
        // 根据暂停状态设置不同的延时
        if(!is_paused)
        {
            this_thread::sleep_for(chrono::milliseconds(33)); // 正常运行时的帧率控制
        }
        else
        {
            this_thread::sleep_for(chrono::milliseconds(100)); // 暂停时减少CPU占用
        }

        char key = cv::waitKey(1);
        if(key == 'q' || key == 'Q')
        {
            cout << "退出程序" << endl;
            break;
        }
        if(key == ' ')  //暂停/继续
        {
            is_paused = !is_paused;
            if(is_paused)
            {
                cout << "程序已暂停 - 按空格键继续" << endl;
            }
            else
            {
                cout << "程序已继续运行" << endl;
            }
        }
        if(key == 's' || key == 'S')
        {
            // 检查绘制图像是否为空再保存
            if(!draw_frame.empty()) {
                string filename = "frame_" + to_string(frame_count) + ".jpg";
                imwrite(filename, draw_frame);
                cout << "保存图像" << filename << endl;
            } else {
                cerr << "无法保存：绘制图像为空" << endl;
            }
        }
        frame_count++;
    }
    //释放资源
    cv::destroyAllWindows();
    return 0;
}
