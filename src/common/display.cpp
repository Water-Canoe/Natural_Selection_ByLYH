#include "common/display.hpp"
#include <unistd.h>
#include <opencv2/highgui.hpp>

using namespace std;
using namespace cv;

namespace common{

const int TARGET_FPS = 30;
const uint16_t COLSIMAGE = 320;
const uint16_t ROWSIMAGE = 240;
const uint16_t COLSIMAGEIPM = 320;
const uint16_t ROWSIMAGEIPM = 240;    


/**
 * @brief 创建一个空窗口
 * 
 * @param wnd_size 窗口大小
 * @param text 窗口标题
 * @return Mat 返回一个空窗口
 */
Mat empty_window(Size2i wnd_size,const string& text)
{
    Mat img = Mat::zeros(wnd_size.width,wnd_size.height,CV_8UC3);
    Display::putText(img,text,Point{20,20});
    return img;
}

void display_loop(Display::Frame_Queue *fq,Display::WndMap *wm,bool *flag)
{
    while(!*flag)
    {
        usleep(1e6);
    }
    while(*flag)
    {
        while(! fq->empty())
        {
            auto item = fq->front();
            fq->pop();
            auto wm_iter = wm->find(item.first);
            if(wm_iter == wm->end())
            {
                printf("错误的窗口序号%d\n",item.first);
                continue;
            }
            imshow(wm_iter->second,item.second);
        }
    }
    auto key = (1000/TARGET_FPS);
}


Display::Display()
    :loop{display_loop,&frame_queue,&wnd_map,&thread_flag}
{
    thread_flag = true;
}

Display::~Display()
{
    thread_flag = false;
    loop.join();
}

void Display::add_window(uint8_t series,const string& wnd_name)
{
    wnd_map[series] = wnd_name;
    auto mat = empty_window(wnd_size,wnd_name);
    frame_queue.push({series,mat});
}

void Display::sync()
{
    while(!frame_queue.empty())
        usleep(1);
}

void Display::putText(cv::Mat& img,const std::string& text,
    cv::Point org,int fontHeight,cv::Scalar color)
{
    cv::putText(img,text,org,cv::FONT_HERSHEY_SIMPLEX,fontHeight,color);
}

void Display::show_image(uint8_t series,cv::Mat frame)
{
    frame_queue.push({series,frame});
}

}