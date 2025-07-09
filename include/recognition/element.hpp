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
    Element();
    ~Element();

    Scene Recognition_Element(Tracking &tracking);

private:
    Scene scene;    // 场景
    bool _crossroad_flag;   // 十字标志位  
    bool _ring_flag;        // 环岛标志位
    bool _bridge_flag;      // 坡道标志位
    bool _obstaclee_flag;   // 障碍标志位
    bool _zebra_flag;       // 斑马线标志位
};

}