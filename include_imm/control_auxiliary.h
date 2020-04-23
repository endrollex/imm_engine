////////////////
// control_auxiliary.h
// by Huang Yiting
////////////////
////////////////
#ifndef CONTROL_AUXILIARY_H
#define CONTROL_AUXILIARY_H
#include "control_xinput.h"
#include <DirectXCollision.h>
using namespace DirectX;
namespace imm
{
////////////////
// control_stop
////////////////
////////////////
template <typename T_app>
struct control_stop
{
	control_stop();
	void set_destination(const float &speed_in, CXMVECTOR &pos, const float &half_y);
	void set_aabb(CXMVECTOR &pos, const float &half_y);
	bool contains(const XMFLOAT3 &center);
	void update(T_app *app, const size_t &index, const float &dt);
	void interrupt(T_app *app, const size_t &index);
	bool is_stop;
	bool is_avoidance;
	bool is_lag;
	bool is_rot_y;
	BoundingBox bbox;
	XMFLOAT3 hit_pos;
	XMFLOAT3 avoidance;
	XMFLOAT3 lag_pos;
	float flsuh_time;
	float speed;
	float avoid_time;
	float last_time;
};
//
template <typename T_app>
control_stop<T_app>::control_stop():
	is_stop(true),
	is_avoidance(false),
	is_lag(false),
	is_rot_y(false),
	hit_pos(0.0f, 0.0f, 0.0f),
	avoidance(0.0f, 0.0f, 0.0f),
	lag_pos(0.0f, 0.0f, 0.0f),
	flsuh_time(0.0f),
	speed(0.0f),
	avoid_time(0.0f),
	last_time(0.0f)
{
	;
}
//
template <typename T_app>
void control_stop<T_app>::set_destination(const float &speed_in, CXMVECTOR &pos, const float &half_y)
{
	is_stop = false;
	is_lag = false;
	is_rot_y = false;
	speed = speed_in;
	set_aabb(pos, half_y);
	XMStoreFloat3(&lag_pos, pos);
	lag_pos.y = 0.0f;
}
//
template <typename T_app>
void control_stop<T_app>::set_aabb(CXMVECTOR &pos, const float &half_y)
{
	XMStoreFloat3(&bbox.Center, pos);
	bbox.Extents = XMFLOAT3(0.2f, half_y*2.0f, 0.2f);
	hit_pos = bbox.Center;
}
//
template <typename T_app>
bool control_stop<T_app>::contains(const XMFLOAT3 &center)
{
	XMVECTOR C = XMLoadFloat3(&center);
	if (bbox.Contains(C) == 2) return true;
	else return false;
}
//
template <typename T_app>
void control_stop<T_app>::update(T_app *app, const size_t &index, const float &dt)
{
	avoid_time -= dt;
	if (is_stop) return;
	// assume bound's center approximate instance's world
	bool is_inter = contains(app->m_Inst.m_BoundW.center(index));
	if (is_inter) {
		is_stop = true;
		last_time = 0.0f;
		app->m_Inst.m_Stat[index].phy.vel_indirect = XMFLOAT3(0.0f, 0.0f, 0.0f);
		app->m_Inst.m_Troll[index].order_ac |= ORDER_IDLE;
	}
	// adjust the direction
	else {
		last_time += dt;
		flsuh_time += dt;
		if (!app->m_Inst.m_Stat[index].phy.is_on_land) return;
		if (flsuh_time > AI_DT_PHY_2SLOW) flsuh_time = 0.0f;
		else return;
		XMVECTOR hit = XMLoadFloat3(&hit_pos);
		if (is_avoidance) {
			math2::mouse_move_toward_hit_ai_avoid(true, index, speed, is_rot_y);
		}
		else {
			if (avoid_time > 0.0f) {
				math2::mouse_move_toward_hit_ai_avoid(false, index, speed);
			}
			else {
				math2::mouse_move_toward_hit(hit, index, speed);
			}
		}
	}
	//
	bool test_short = false;
	if (is_avoidance && last_time > 2.0f) test_short = (PTR->m_Inst.m_Probe.candidate_vec.size() > 0);
	if (test_short) {
		if (!is_rot_y) {
			is_rot_y = true;
		}
		else {
			is_stop = true;
			last_time = 0.0f;
			app->m_Inst.m_Stat[index].phy.vel_indirect = XMFLOAT3(0.0f, 0.0f, 0.0f);
			app->m_Inst.m_Troll[index].order_ac |= ORDER_IDLE;
		}
	}
	return;
}
//
template <typename T_app>
void control_stop<T_app>::interrupt(T_app *app, const size_t &index)
{
	if (is_stop) return;
	is_stop = true;
	last_time = 0.0f;
	is_avoidance = false;
	app->m_Inst.m_Stat[index].phy.vel_indirect = XMFLOAT3(0.0f, 0.0f, 0.0f);
}
////////////////
// control_speed
////////////////
////////////////
template <typename T_app>
struct control_speed
{
	control_speed();
	void update(T_app *app, const size_t &index, const float &dt);
	void reset();
	void set(const size_t &index, const std::list<XMFLOAT2> speed_in);
	float count_d;
	float current_spped;
	std::list<XMFLOAT2> speed;
};
//
template <typename T_app>
control_speed<T_app>::control_speed():
	count_d(-1.0f),
	current_spped(0.0f)
{
	;
}
//
template <typename T_app>
void control_speed<T_app>::update(T_app *app, const size_t &index, const float &dt)
{
	app;
	if (speed.size() == 0) return;
	count_d += dt;
	if (count_d > speed.front().x) {
		math2::set_inst_speed(index, speed.front().y, false);
		current_spped = speed.front().y;
		speed.pop_front();
	}
}
//
template <typename T_app>
void control_speed<T_app>::reset()
{
	speed.clear();
	count_d = 0.0f;
	current_spped = 0.0f;
}
//
template <typename T_app>
void control_speed<T_app>::set(const size_t &index, const std::list<XMFLOAT2> speed_in)
{
	assert(speed_in.size() > 0);
	reset();
	speed = speed_in;
	if (speed.front().x < FPS_MIN_REQ_1DIV) {
		math2::set_inst_speed(index, speed.front().y, false);
		current_spped = speed.front().y;
		speed.pop_front();
	}
}
//
}
#endif