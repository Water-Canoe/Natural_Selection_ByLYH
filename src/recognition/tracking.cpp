#include "recognition/tracking.hpp"
#include <iostream>
#include <algorithm>
#include "common.hpp"

using namespace common;
using namespace std;

/*
 * 图像坐标系说明：
 * (0,0) -------- (width-1,0)x
 *   |               |
 *   |               |
 *   |               |
 *   |               |
 * (0,height-1) -- (width-1,height-1)
 *   y
 */

namespace recognition{

/**
 * @brief 构造函数：初始化跟踪器
 * 
 * 功能说明：
 * 1. 加载配置文件中的参数
 * 2. 初始化摄像头
 * 3. 获取图像尺寸和边框参数
 * 4. 获取二值化图像用于后续处理
 */
Tracking::Tracking()
{
    _width = _parameter.Get_Parameter("Image_Width").get<int>();    // 获取图像宽度
    _height = _parameter.Get_Parameter("Image_Height").get<int>();  // 获取图像高度
    _border = _parameter.Get_Parameter("Border").get<int>();        // 获取边框宽度
}

/**
 * @brief 析构函数：清理资源
 */
Tracking::~Tracking()
{

}

/**
 * @brief 图像预处理函数
 * 
 * 算法流程：
 * 1. 捕获原始图像
 * 2. 图像尺寸调整（压缩）
 * 3. 高斯滤波去噪
 * 4. 灰度化处理
 * 5. 二值化处理
 * 6. 添加黑色边框
 * 
 * 处理目的：
 * - 减少噪声干扰
 * - 突出赛道边缘特征
 * - 为后续边缘检测做准备
 */
void Tracking::Picture_Process()
{
    // 步骤1：捕获图像
    if(!_camera.Capture())
    {
        std::cerr << "Failed to capture frame" << std::endl;
        return;
    }
    
    // 步骤2：图像处理
    _camera.Resize_Frame(_width,_height);
    
    // 检查图像处理是否成功
    if(!_camera.Frame_Process())
    {
        std::cerr << "图像处理失败" << std::endl;
        return;
    }
    
    // 获取原始帧并检查是否为空
    cv::Mat original_frame = _camera.Get_Frame();
    if(original_frame.empty())
    {
        std::cerr << "原始图像为空，无法创建绘制帧" << std::endl;
        return;
    }
    
    _draw_frame = original_frame.clone();
    
    // 步骤3：添加边框(_border个像素的黑色边框)
    // 边框的作用：防止边缘检测时越界，同时提供边界参考
    cv::Mat binary_frame = _camera.Get_Binary_Frame();
    if(!binary_frame.empty())
    {
        cv::rectangle(binary_frame,
            cv::Point(0,0),
            cv::Point(binary_frame.cols, binary_frame.rows),
            cv::Scalar(0,0,0), _border);
    }
    else
    {
        std::cerr << "二值化图像为空，无法添加边框" << std::endl;
    }
}


/**
 * @brief 赛道起点检测
 * @param scan_start_y 从底部向上第几行开始扫描
 * @param scan_height 扫描多少行（建议5~10）
 * @return true 找到起点，false 未找到
 */
bool Tracking::Find_Start_Point(int scan_start_y, int scan_height)
{
    // 检查二值化图像是否可用
    cv::Mat binary_frame = _camera.Get_Binary_Frame();
    if(binary_frame.empty())
    {
        debug << "二值化图像为空，无法寻找起点" << std::endl;
        return false;
    }
    
    // 检查图像尺寸
    if(binary_frame.cols <= 0 || binary_frame.rows <= 0)
    {
        debug << "二值化图像尺寸无效" << std::endl;
        return false;
    }
    
    std::vector<int> lefts, rights;
    int y_base = _height - scan_start_y; // 从底部向上
    
    // 检查扫描范围是否有效
    if(y_base < 0 || y_base >= binary_frame.rows)
    {
        debug << "扫描起始位置无效: y_base=" << y_base << ", rows=" << binary_frame.rows << std::endl;
        return false;
    }

    for(int offset = 0; offset < scan_height; ++offset) 
    {
        int y = y_base - offset;
        
        // 检查y坐标是否在有效范围内
        if(y < 0 || y >= binary_frame.rows)
        {
            continue; // 跳过无效行
        }
        
        int left = -1, right = -1;
        // bool in_white = false;  // 暂时注释掉未使用的变量

        // 从左到右扫描，找到第一个白色像素
        for(int x = 0; x < binary_frame.cols; ++x) {
            if(binary_frame.at<uchar>(y, x) == WHITE) {
                left = x;
                break;
            }
        }
        // 从右到左扫描，找到第一个白色像素
        for(int x = binary_frame.cols - 1; x >= 0; --x) {
            if(binary_frame.at<uchar>(y, x) == WHITE) {
                right = x;
                break;
            }
        }
        // 记录有效边界
        if(left > 0 && right > left && (right - left) > _width * 0.5) { // 宽度阈值可调
            lefts.push_back(left);
            rights.push_back(right);
        }
    }

    // 统计多行结果，取中位数/均值
    if(lefts.size() >= static_cast<size_t>(scan_height / 2) && rights.size() >= static_cast<size_t>(scan_height / 2)) {
        std::sort(lefts.begin(), lefts.end());
        std::sort(rights.begin(), rights.end());
        int left_pt = lefts[lefts.size()/2];
        int right_pt = rights[rights.size()/2];
        int y_pt = _height - scan_start_y - scan_height/2;

        _maze_edge_left.push_back({left_pt, y_pt});
        _maze_edge_right.push_back({right_pt, y_pt});

        // 检查绘制帧是否可用
        if(!_draw_frame.empty())
        {
            cv::circle(_draw_frame,cv::Point(_maze_edge_left[0].x,_maze_edge_left[0].y),5,cv::Scalar(0,0,255),-1);
            cv::circle(_draw_frame,cv::Point(_maze_edge_right[0].x,_maze_edge_right[0].y),5,cv::Scalar(255,0,0),-1);
        }

        return true;
    } else {
        debug << "未找到有效的赛道起点！" << std::endl;
        return false;
    }
}

/**
 * @brief 方向数组定义
 * 
 * 迷宫算法的核心：定义四个基本方向（上、右、下、左）
 * 用于控制巡线机器人的移动方向
 */
const POINT dir_front[4] = {
    {0,-1},  // 上：y坐标减少
    {1,0},   // 右：x坐标增加
    {0,1},   // 下：y坐标增加
    {-1,0}   // 左：x坐标减少
};

/**
 * @brief 左前方方向数组
 * 
 * 用于检测左前方的像素值，判断是否需要左转
 */
const POINT dir_frontleft[4] = {
    {-1,-1},  // 左上
    {1,-1},   // 右上
    {1,1},    // 右下
    {-1,1}    // 左下
};

/**
 * @brief 右前方方向数组
 * 
 * 用于检测右前方的像素值，判断是否需要右转
 */
const POINT dir_frontright[4] = {
    {1,-1},   // 右上
    {1,1},    // 右下
    {-1,1},   // 左下
    {-1,-1}   // 左上
};

// 最大步数限制，防止无限循环
#define STEP_MAX 1500

/**
 * @brief 基于迷宫法的赛道识别算法
 * 
 * 算法原理：
 * 使用双线并行巡线技术，同时跟踪赛道的左右边界。
 * 通过分析前方、左前方、右前方的像素值来决定移动方向。
 * 
 * 巡线策略：
 * 1. 左线巡线：优先保持左前方为黑色（贴近左边界）
 * 2. 右线巡线：优先保持右前方为黑色（贴近右边界）
 * 3. 当遇到障碍时，通过转向来寻找新的路径
 * 
 * 移动规则：
 * - 前方为黑色：转向（左线右转，右线左转）
 * - 前方为白色且侧前方为黑色：直行
 * - 前方和侧前方都为白色：向侧前方移动并转向
 */
void Tracking::Track_Recognition()
{
    _maze_edge_left.clear();
    _maze_edge_right.clear();
    // 获取起始行参数
    int start_line = _parameter.Get_Parameter("Start_Line").get<int>();
    // 寻找起点，如果失败则退出
    while(!Find_Start_Point(start_line))
    {
        debug << "未找到起点， 重新寻找" << endl;
        start_line += 5;
        if(start_line > _height / 2)
            return;
    }
    
    // 初始化巡线参数
    int l_step = 0, l_dir = 0, l_turn = 0;    // 左边步数、方向、转向次数
    int r_step = 0, r_dir = 0, r_turn = 0;    // 右边步数、方向、转向次数
    
    // 获取起始点
    POINT l_point = _maze_edge_left[0];  // 左线起始点
    POINT r_point = _maze_edge_right[0]; // 右线起始点
    
    // 添加起始点到路径中
    _maze_edge_left.push_back(l_point);
    _maze_edge_right.push_back(r_point);

    //历史方向记录
    vector<pair<POINT,int>> l_history_dir;
    vector<pair<POINT,int>> r_history_dir;
    
    // 双线并行巡线主循环
    while(true)
    {
        // 边界检查：确保不会越界
        if(l_point.x <= 0 || l_point.x >= _camera.Get_Binary_Frame().cols - 1 || 
           l_point.y <= 0 || l_point.y >= _camera.Get_Binary_Frame().rows - 1 ||
           r_point.x <= 0 || r_point.x >= _camera.Get_Binary_Frame().cols - 1 || 
           r_point.y <= 0 || r_point.y >= _camera.Get_Binary_Frame().rows - 1) {
            debug << "越界，退出循环" << endl;
            break;
        }
        
        // 获取前方和侧前方的像素值
        uchar l_front_value = _camera.Get_Binary_Frame().at<uchar>(l_point.y + dir_front[l_dir].y, 
                                                     l_point.x + dir_front[l_dir].x);
        uchar l_frontleft_value = _camera.Get_Binary_Frame().at<uchar>(l_point.y + dir_frontleft[l_dir].y, 
                                                         l_point.x + dir_frontleft[l_dir].x);
        uchar r_front_value = _camera.Get_Binary_Frame().at<uchar>(r_point.y + dir_front[r_dir].y, 
                                                     r_point.x + dir_front[r_dir].x);
        uchar r_frontright_value = _camera.Get_Binary_Frame().at<uchar>(r_point.y + dir_frontright[r_dir].y, 
                                                          r_point.x + dir_frontright[r_dir].x);

        // ===================================== 左边线巡线逻辑 =======================================
        if(l_front_value == BLACK)  
        {
            // 情况1：前方为黑色 -> 右转
            // 说明前方是障碍物，需要改变方向
            l_dir = (l_dir + 1) % 4;  // 顺时针旋转90度
            l_turn++;
            l_history_dir.push_back(make_pair(l_point,l_dir));
        }
        else if(l_front_value == WHITE && l_frontleft_value == BLACK)
        {
            // 情况2：前方为白色，左前方黑色 -> 直行
            // 这是理想的巡线状态，保持当前方向前进
            l_point.x += dir_front[l_dir].x;
            l_point.y += dir_front[l_dir].y;
            _maze_edge_left.push_back(l_point);
            l_step++;
            l_turn = 0;  // 重置转向计数
            l_history_dir.push_back(make_pair(l_point,l_dir));
        }
        else
        {
            // 情况3：前方和左前方都是白色 -> 左前方移动
            // 说明需要向左调整位置，贴近左边界
            l_point.x += dir_frontleft[l_dir].x;
            l_point.y += dir_frontleft[l_dir].y;
            l_dir = (l_dir + 3) % 4;  // 逆时针旋转90度（左转）
            _maze_edge_left.push_back(l_point);
            l_step++;
            l_turn = 0;  // 重置转向计数
            l_history_dir.push_back(make_pair(l_point,l_dir));
        }
        // if(l_step > 5)
        // {
        //     if(l_history_dir[l_step].second == 0 
        //         && l_history_dir[l_step - 1].second == 3
        //         && l_history_dir[l_step - 2].second == 0
        //         && l_history_dir[l_step - 3].second == 0)
        //     {
        //         if(_corner_right_down.x == 0)
        //         {
        //             _corner_right_down = l_history_dir[l_step].first;
        //         }
        //     }
        //     if(l_history_dir[l_step].second == 0 
        //         && l_history_dir[l_step - 1].second == 1
        //         && l_history_dir[l_step - 2].second == 0
        //         && l_history_dir[l_step - 3].second == 0)
        //     {
        //         if(_corner_right_up.x == 0)
        //         {
        //             _corner_right_up = l_history_dir[l_step].first;
        //         }
        //     }
        // }
        // =========================================== 右边线巡线逻辑 =======================================
        if(r_front_value == BLACK)
        {
            // 情况1：前方为黑色 -> 左转
            // 与左线相反，右线遇到障碍时左转
            r_dir = (r_dir + 3) % 4;  // 逆时针旋转90度
            r_turn++;
            r_history_dir.push_back(make_pair(r_point,r_dir));
        }
        else if(r_front_value == WHITE && r_frontright_value == BLACK)
        {
            // 情况2：前方为白色，右前方黑色 -> 直行
            // 理想的巡线状态
            r_point.x += dir_front[r_dir].x;
            r_point.y += dir_front[r_dir].y;
            _maze_edge_right.push_back(r_point);
            r_step++;
            r_turn = 0;  // 重置转向计数
            r_history_dir.push_back(make_pair(r_point,r_dir));
        }
        else
        {
            // 情况3：前方和右前方都是白色 -> 右前方移动
            // 向右调整位置，贴近右边界
            r_point.x += dir_frontright[r_dir].x;
            r_point.y += dir_frontright[r_dir].y;
            r_dir = (r_dir + 1) % 4;  // 顺时针旋转90度（右转）
            _maze_edge_right.push_back(r_point);
            r_step++;
            r_turn = 0;  // 重置转向计数
            r_history_dir.push_back(make_pair(r_point,r_dir));
        }
        // if(r_step > 5)
        // {
        //     if(r_history_dir[r_step].second == 0 
        //         && r_history_dir[r_step - 1].second == 1
        //         && r_history_dir[r_step - 2].second == 0
        //         && r_history_dir[r_step - 3].second == 0)
        //     {
        //         if(_corner_right_down.x == 0)
        //         {
        //             _corner_right_down = r_history_dir[r_step].first;
        //         }
        //     }
        //     if(r_history_dir[r_step].second == 0 
        //         && r_history_dir[r_step - 1].second == 3
        //         && r_history_dir[r_step - 2].second == 0
        //         && r_history_dir[r_step - 3].second == 0)
        //     {
        //         if(_corner_right_up.x == 0)
        //         {
        //             _corner_right_up = r_history_dir[r_step].first;
        //         }
        //     }
        // }

        //=========================================限制条件=====================================
        // 转向次数限制：防止在复杂路径中陷入循环
        if(l_turn > 3 || r_turn > 3)
        {
            debug << "转向次数过多，退出循环" << endl;
            break;
        }
        // 到达图像顶部，退出循环
        if(l_point.y <= _border && r_point.y <= _border)
        {
            if(l_point.x == _width / 2 || r_point.x == _width / 2)
            {
                debug << "中线，退出循环" << endl;
                break;
            }
        }
        // 两点相遇，退出循环
        if(l_point.x == r_point.x && l_point.y == r_point.y) 
        {
            debug << "两点相遇，退出循环" << endl;
            break;
        }
        if(l_step > STEP_MAX || r_step > STEP_MAX)
        {
            debug << "步数过多，退出循环" << endl;
            break;
        }
    }
} 


/**
 * @brief 提取赛道边缘点、斜率、宽度
 */
void Tracking::Edge_Extract()
{
    // 获取左右线的步数
    int l_step = _maze_edge_left.size();
    int r_step = _maze_edge_right.size();
    _valid_row = min(l_step,r_step);  // 取较小值，确保同步处理

    // 清空之前的边缘点记录
    _edge_left.clear();
    _edge_right.clear();
    _width_block.clear();
    _corner_left_up = {0,0};
    _corner_left_down = {0,0};
    _corner_right_up = {0,0};
    _corner_right_down = {0,0};

    // 初始化最高点记录（用于按行提取）
    POINT l_heighest_point{0,_width-_border};  // 左线最高点
    POINT r_heighest_point{0,_width-_border};  // 右线最高点
    int l_height = 0;  // 左线高度计数
    int r_height = 0;  // 右线高度计数
    float temp_slope1 = 0.0f,temp_slope2 = 0.0f;

    vector<double> l_slope;
    vector<double> r_slope;
    
    // 遍历巡线路径，按行提取边缘点
    for(int i = 0;i < _valid_row;i++)
    {
        // ========== 左线边缘点提取 ==========
        // 检查当前点是否比已记录的最高点更高（y坐标更小）
        if(_maze_edge_left[i].y < l_heighest_point.y)
        {
            l_heighest_point = _maze_edge_left[i];
            _edge_left.push_back(l_heighest_point);
            if(_edge_left[i].x == _border)
                _lost_left.push_back(_edge_left[i]);
            l_height++;
            
            // 计算斜率、角点（从第8个点开始）
            if(l_height > 8)
            {
                int current_idx = l_height - 1;
                // ============================================斜率计算========================================
                // 计算当前点与前三个点之间的斜率
                temp_slope1 = Slope_Point_To_Point(_edge_left[current_idx],_edge_left[current_idx-2]);
                temp_slope2 = Slope_Point_To_Point(_edge_left[current_idx],_edge_left[current_idx-4]);
                
                if(abs(temp_slope1) != 255 && abs(temp_slope2) != 255)
                {
                    _edge_left[current_idx].slope = (temp_slope1 + temp_slope2)*1.0f / 2;
                    l_slope.push_back(_edge_left[current_idx].slope);
                }
                else if(abs(temp_slope1) != 255)
                {
                    _edge_left[current_idx].slope = temp_slope1;
                    l_slope.push_back(_edge_left[current_idx].slope);
                } else 
                {
                    _edge_left[current_idx].slope = temp_slope2;
                    l_slope.push_back(_edge_left[current_idx].slope);
                }
                // ============================================角点记录========================================
                // 角点计算方法一——基于斜率：
                // 记录左下角点
                if(_edge_left[current_idx].slope > 0 
                    && abs(_edge_left[current_idx].slope) != 255 
                    && _corner_left_down.x == 0)
                {
                    _corner_left_down = _edge_left[current_idx - 1];
                }
            }
            else
            {
                _edge_left[l_height-1].slope = 0.0f;
            }
        }
        // =========================================== 右线边缘点提取 =========================================
        // 检查当前点是否比已记录的最高点更高
        if(_maze_edge_right[i].y < r_heighest_point.y)
        {
            r_heighest_point = _maze_edge_right[i];
            _edge_right.push_back(r_heighest_point);
            if(_edge_right[i].x == _width - _border)
                _lost_right.push_back(_edge_right[i]);
            r_height++;
            
            // 计算斜率、角点
            if(r_height > 8)            {
                int current_idx = r_height - 1;
                // ============================================斜率计算========================================
                temp_slope1 = Slope_Point_To_Point(_edge_right[current_idx],_edge_right[current_idx-2]);
                temp_slope2 = Slope_Point_To_Point(_edge_right[current_idx],_edge_right[current_idx-4]);
                if(abs(temp_slope1) != 255 && abs(temp_slope2) != 255)
                {
                    _edge_right[current_idx].slope = (temp_slope1 + temp_slope2)*1.0f / 2;
                    r_slope.push_back(_edge_right[current_idx].slope);
                }
                else if(abs(temp_slope1) != 255)
                {
                    _edge_right[current_idx].slope = temp_slope1;
                    r_slope.push_back(_edge_right[current_idx].slope);
                } else {
                    _edge_right[current_idx].slope = temp_slope2;
                    r_slope.push_back(_edge_right[current_idx].slope);
                }
                // ============================================角点记录========================================
                // 记录右下角点
                if(_edge_right[current_idx].slope < 0 
                    && abs(_edge_right[current_idx].slope) != 255 
                    && _corner_right_down.x == 0)
                {
                    _corner_right_down = _edge_right[current_idx - 1];
                }
                // 注意：右上角点检测需要宽度信息，将在后面进行
            }
            else
            {
                _edge_right[r_height-1].slope = 0.0f;
            }
        }
    }
    _valid_row = min(l_height,r_height);
    for(int i = 0;i < _valid_row;i++)
    {
        _width_block.push_back(_edge_right[i].x - _edge_left[i].x);
    }

    // ============================================ 角点检测（需要宽度信息）========================================
    // 检测左上角点
    for(int i = 8; i < l_height && i < static_cast<int>(_width_block.size()) && i >= 2; i++)
    {
        if(_corner_left_up.x == 0) // 如果还没有找到左上角点
        {
            bool width_condition = (_width_block[i] <= _width_block[i - 2]*0.6);
            if((_edge_left[i].slope < _parameter.Get_Parameter("Corner_Left_Up_Slope1_Min").get<double>() 
                && _edge_left[i].slope > _parameter.Get_Parameter("Corner_Left_Up_Slope1_Max").get<double>())
                && (_edge_left[i - 2].slope > _parameter.Get_Parameter("Corner_Left_Up_Slope2").get<double>() 
                && (width_condition || abs(_edge_left[i].slope) != 255)
                && (width_condition || abs(_edge_left[i - 2].slope) != 255)
                && _edge_left[i - 2].slope != 0 
                && _edge_left[i - 2].y < _height - 50))
            {
                debug << "左上斜率" << to_string(_edge_left[i].slope) << endl;
                debug << "左上-2斜率" << to_string(_edge_left[i - 2].slope) << endl;
                _corner_left_up = _edge_left[i - 2];
                break;
            }
        }
    }
    
    // 检测右上角点
    for(int i = 8; i < r_height && i < static_cast<int>(_width_block.size()) && i >= 2; i++)
    {
        if(_corner_right_up.x == 0) // 如果还没有找到右上角点
        {
            bool width_condition_right = (_width_block[i] <= _width_block[i - 2]*0.6);
            if(_edge_right[i].slope > _parameter.Get_Parameter("Corner_Right_Up_Slope1_Min").get<double>() 
                && _edge_right[i].slope < _parameter.Get_Parameter("Corner_Right_Up_Slope1_Max").get<double>()
                && (_edge_right[i - 2].slope < _parameter.Get_Parameter("Corner_Right_Up_Slope2").get<double>())
                && _edge_right[i - 2].slope != 0
                && (width_condition_right || abs(_edge_right[i].slope) != 255)
                && (width_condition_right || abs(_edge_right[i - 2].slope) != 255)
                && _edge_right[i - 2].y < _height - 50)
            {
                debug << "右上斜率" << to_string(_edge_right[i].slope) << endl;
                debug << "右上-2斜率" << to_string(_edge_right[i - 2].slope) << endl;
                _corner_right_up = _edge_right[i - 2];
                break;
            }
        }
    }
    stdev_edge_left = Variance<double, vector<double>>(l_slope);
    stdev_edge_right = Variance<double, vector<double>>(r_slope);
    debug << "左边缘点方差：" << stdev_edge_left << std::endl;
    debug << "右边缘点方差：" << stdev_edge_right << std::endl;
    debug << "宽度：" << _width_block[_height/2] << std::endl;
}


// /**
//  * @brief 绘制边缘点
//  * 
//  * 功能说明：
//  * 在图像上绘制提取到的边缘点，用于可视化调试。
//  * 左边缘点用蓝色圆圈表示，右边缘点用红色圆圈表示。
//  * 
//  * 调试用途：
//  * - 验证边缘检测的准确性
//  * - 观察赛道识别的效果
//  * - 分析算法性能
//  */
// void Tracking::Draw_Edge()
// {
//     // 绘制左边缘点（蓝色）
//     for(size_t i = 0; i < _edge_left.size();i++)
//     {
//         cv::circle(_draw_frame,
//                    cv::Point(_edge_left[i].x,_edge_left[i].y),
//                    1,  // 圆圈半径
//                    cv::Scalar(255,0,0),  // 蓝色 (B,G,R)
//                    -1);  // 实心圆
//     }
//     // 绘制右边缘点（红色）
//     for(size_t i = 0; i < _edge_right.size();i++)
//     {
//         cv::circle(_draw_frame,
//                    cv::Point(_edge_right[i].x,_edge_right[i].y),
//                    1,  // 圆圈半径
//                    cv::Scalar(0,0,255),  // 红色 (B,G,R)
//                    -1);  // 实心圆
//     }
//     // 绘制十字补线
//     for(size_t i = 0; i < _crossroad_left_line.size();i++)
//     {
//         cv::line(_draw_frame,
//                    cv::Point(_crossroad_left_line[i].x,_crossroad_left_line[i].y),
//                    cv::Point(_crossroad_right_line[i].x,_crossroad_right_line[i].y),
//                    cv::Scalar(0,255,255),  // 绿色 (B,G,R)
//                    -1);  // 实心圆
//     }
//     // 绘制角点
//     cv::circle(_draw_frame,
//                cv::Point(_corner_left_down.x,_corner_left_down.y),
//                2,  // 圆圈半径
//                cv::Scalar(0,255,0),  // 绿色 (B,G,R)
//                -1);  // 实心圆
//     cv::circle(_draw_frame,
//                cv::Point(_corner_right_down.x,_corner_right_down.y),
//                2,  // 圆圈半径
//                cv::Scalar(0,255,0),  // 绿色 (B,G,R)
//                -1);  // 实心圆
//     cv::circle(_draw_frame,
//                cv::Point(_corner_left_up.x,_corner_left_up.y),
//                2,  // 圆圈半径
//                cv::Scalar(0,255,255),  // 绿色 (B,G,R)
//                -1);  // 实心圆
//     cv::circle(_draw_frame,
//                cv::Point(_corner_right_up.x,_corner_right_up.y),
//                2,  // 圆圈半径
//                cv::Scalar(0,255,255),  // 绿色 (B,G,R)
//                -1);  // 实心圆
//     debug << "左边斜率：" << _edge_left[_height/2].slope << std::endl;
//     debug << "右边斜率：" << _edge_right[_height/2].slope << std::endl;
// }


}