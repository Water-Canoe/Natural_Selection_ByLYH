#pragma once

#include <opencv2/opencv.hpp>
#include <queue>
#include <thread>
#include <unordered_map>

namespace common{

extern const int TARGET_FPS;
extern const uint16_t COLSIMAGE;
extern const uint16_t ROWSIMAGE;
extern const uint16_t COLSIMAGEIPM;
extern const uint16_t ROWSIMAGEIPM;   


class Display{

public:
    using Frame = std::pair<uint8_t,cv::Mat>;   //显示帧数据结构
    using Frame_Queue = std::queue<Frame>;      //显示帧队列类型
    using WndMap = std::unordered_map<uint8_t,std::string>;  //窗口映射表类型

    Display();
    ~Display();

    static void putText(
        cv::Mat& img,const std::string& text, cv::Point org,
        int fontHeight = 1,cv::Scalar color = cv::Scalar{0,255,0}
    );
    cv::Size2i wnd_size {COLSIMAGE,ROWSIMAGE};
    cv::Size2i display_size {COLSIMAGE,ROWSIMAGE};

    int sizeWindow = 1; //窗口数量
    bool save = false;  //图像保存标志

    void add_window(uint8_t series, const std::string& wnd_name);
    void sync();
    void show_image(uint8_t series,cv::Mat frame);


private:

    const int max_show = 4; //最大同时显示窗口数
    cv::Mat imgShow;        //显示图像缓冲区
    bool realShow = false;  //是否实时显示标志
    bool thread_flag = false;  //线程运行标志
    Frame_Queue frame_queue;  //显示帧队列
    WndMap wnd_map;         //窗口映射表
    std::thread loop;       //显示线程

};

}
