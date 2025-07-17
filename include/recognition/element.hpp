#pragma once

#include "common.hpp"
#include "recognition/tracking.hpp"

namespace recognition
{

enum class Scene
{
    NolmalScene = 0,//基础赛道
    ZebraScene,     // 斑马线
    CrossScene,      // 十字道路
    RingScene,       // 环岛道路
    BridgeScene,     // 坡道区
    ObstacleScene,   // 障碍区
    CateringScene,   // 快餐店
    LaybyScene,      // 临时停车区
    ParkingScene,    // 停车区
};

class Element
{
public:
    uint8_t _crossroad_cnt;   // 十字帧计数  
    uint8_t _ring_cnt;        // 环岛帧计数
    uint8_t _bridge_cnt;      // 坡道帧计数
    uint8_t _obstaclee_cnt;   // 障碍帧计数
    uint8_t _zebra_cnt;       // 斑马线帧计数
    bool _crossroad_flag;   // 十字标志位  
    bool _ring_flag;        // 环岛标志位
    bool _bridge_flag;      // 坡道标志位
    bool _obstaclee_flag;   // 障碍标志位
    bool _zebra_flag;       // 斑马线标志位

    float _middle_error;      // 中间线误差
    
    std::vector<common::POINT> _left_line;
    std::vector<common::POINT> _right_line;
    std::vector<common::POINT> _middle_line;




    Element();
    ~Element();

    Scene Recognition_Element(Tracking &tracking);
    void Draw_Edge(Tracking &tracking);
    float Get_Middle_Error(Tracking &tracking);

private:
    /**
     * @brief 应用补线到边线中
     * @param current_y 当前边线点的Y坐标
     * @param supplement_line 补线点集
     * @param supplement_index 补线索引（引用，会被更新）
     * @return 是否成功应用补线
     */
    bool Apply_Supplement_Line(int current_y, const std::vector<common::POINT>& supplement_line, int& supplement_index);

    Scene scene;    // 场景
    std::vector<common::POINT> _crossroad_left_line;   // 十字左补线
    std::vector<common::POINT> _crossroad_right_line;  // 十字右补线
    std::vector<common::POINT> _obstacle_left_line;   // 障碍物左补线
    std::vector<common::POINT> _obstacle_right_line;  // 障碍物右补线
    std::vector<common::POINT> _ring_left_line_in;   // 环岛左补线入
    std::vector<common::POINT> _ring_right_line_in;  // 环岛右补线入
    std::vector<common::POINT> _ring_left_line_out;   // 环岛左补线出
    std::vector<common::POINT> _ring_right_line_out;  // 环岛右补线出



};

}