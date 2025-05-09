////////////////
// phy_obstacle_avoid.h
// by Huang Yiting
// method:
// http://gamedevelopment.tutsplus.com/tutorials/understanding-steering-behaviors-collision-avoidance--gamedev-7777
////////////////
////////////////
#ifndef PHY_OBSTACLE_AVOID_H
#define PHY_OBSTACLE_AVOID_H
#include "phy_prepare.h"
namespace imm
{
////////////////
// phy_collision_avoidance
////////////////
////////////////
void phy_collision_avoidance(
	const phy_property &prop_A,
	CXMVECTOR &center_A,
	CXMVECTOR &center_B,
	const float max_see_ahead,
	XMFLOAT3 &avoidance_out
)
{
	XMVECTOR ahead = XMLoadFloat3(&prop_A.vel_indirect);
	ahead = XMVector3Normalize(ahead);
	ahead = XMVectorScale(ahead, max_see_ahead);
	ahead = XMVectorAdd(ahead, center_A);
	//
	XMVECTOR avoidance = XMVectorSubtract(ahead, center_B);
	avoidance = XMVector3Normalize(avoidance);
	avoidance = XMVectorScale(avoidance, max_see_ahead);
	XMStoreFloat3(&avoidance_out, avoidance);
}
//
}
#endif