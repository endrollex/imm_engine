////////////////
// phy_motion.h
// by Huang Yiting
////////////////
////////////////
#ifndef PHY_MOTION_H
#define PHY_MOTION_H
#include <vector>
#include <DirectXMath.h>
namespace imm
{
////////////////
// moti_circle
// https://stackoverflow.com/questions/16448702/how-to-move-the-position-of-a-calculated-circle
////////////////
////////////////
struct moti_circle
{
	moti_circle();
	XMFLOAT3 edge;
	XMFLOAT3 target;
	XMFLOAT3 init_pos;
	XMFLOAT3 direction1;
	XMFLOAT3 center;
	float duration;
	float dt_path;
	size_t path_ix;
	size_t path_step;
	size_t path_ix_start;
	bool is_active;
	bool is_initialzed;
	std::vector<XMFLOAT3> path;
	std::vector<XMFLOAT3> history;
	void active(const XMFLOAT3 &pos);
	template <typename T_app, typename T_magic>
	void update(const float &dt, T_app app, T_magic magic, const size_t &circle_ix);
	template <typename T_app, typename T_magic>
	void up_p1(const float &dt, T_app app, T_magic magic, const size_t &circle_ix);
};
//
moti_circle::moti_circle():
	edge(0.0f, 0.0f, 0.0f),
	target(0.0f, 0.0f, 0.0f),
	init_pos(0.0f, 0.0f, 0.0f),
	direction1(0.0f, 0.0f, 0.0f),
	center(0.0f, 0.0f, 0.0f),
	duration(0.0f),
	dt_path(1.0f),
	path_ix(0),
	path_step(0),
	path_ix_start(0),
	is_active(false),
	is_initialzed(false),
	path(),
	history()
{
	;
}
//
void moti_circle::active(const XMFLOAT3 &pos)
{
	edge = pos;
	is_active = true;
	path.resize(3);
}
//
template <typename T_app, typename T_magic>
void moti_circle::update(const float &dt, T_app app, T_magic magic, const size_t &circle_ix)
{
	if (!is_active) return;
	if (!is_initialzed) {
		XMStoreFloat3(&direction1, app->m_Inst.m_BoundW.direction(magic->caster));
		center = PTR->m_Hit.center_box(magic->caster, magic->vec_bone_name[circle_ix]);
		PTR->m_Scene.plasma.push_back(PLASMA_MISSILE3, 45.0f, path[0], true);
		PTR->m_Scene.plasma.push_back(PLASMA_MISSILE3, 45.0f, path[1], true);
		PTR->m_Scene.plasma.push_back(PLASMA_MISSILE4, 45.0f, path[2], true);
		is_initialzed = true;
	}
	duration += dt;
	//
	if (duration < 20.0f) up_p1(dt, app, magic, circle_ix);
	//
	history.push_back(edge);
	if (path_step == 0) dt_path += dt;
	if (path_step == 0 &&dt_path > 0.01f) {
		dt_path = 0.0f;
		path_step = path_ix-path_ix_start;
		path_ix_start = path_ix;
	}
	if (history.size() > path_step*3) {
		path[0] = history[history.size()-path_step-1];
		path[1] = history[history.size()-(path_step*2)-1];
		path[2] = history[history.size()-(path_step*3)-1];
	}
	path_ix += 1;
	if (path_step != 0) {
		if (history.size() > path_step*100) {
			history.erase(history.begin()+(path_step*10));
		}
	}
}
//
template <typename T_app, typename T_magic>
void moti_circle::up_p1(const float &dt, T_app app, T_magic magic, const size_t &circle_ix)
{
	circle_ix;
	//
	float radius = 5.0f;
	float perimeter = 25.0f;
	float point_time = 1.0f;
	XMFLOAT3 point(0.0f, 0.0f, 0.0f);
	if (duration < point_time) {
		radius *= (point_time-duration)/point_time;
		point.x = radius * sin(duration*100.0f/perimeter);
		point.z = radius * cos(duration*100.0f/perimeter);
	}
	//
	XMVECTOR dir1(XMVectorZero());
	XMVECTOR P1 = XMLoadFloat3(&point);
	XMVECTOR E1 = XMLoadFloat3(&edge);
	XMVECTOR pos = XMLoadFloat3(&center);
	//
	if (duration > 0.5f) {
		XMVECTOR target_pos = XMLoadFloat3(&app->m_Inst.m_BoundW.center(magic->target));
		dir1 = XMVectorSubtract(target_pos, E1);
		dir1 = XMVector3Normalize(dir1);
		if (duration < 1.5f) {
			XMVECTOR caster_dir = XMLoadFloat3(&direction1);
			dir1 = XMVectorLerp(caster_dir, dir1, (duration-0.5f)/1.0f);
		}
	}
	else {
		dir1 = XMLoadFloat3(&direction1);
	}
	float vel = 0.2f;
	vel *= (duration+1.0f)/1.0f;
	vel = math::calc_clamp(vel, 0.2f, 0.6f);
	vel *= FPS60*dt*3.0f;
	dir1 = XMVectorScale(dir1, vel);
	dir1 = XMVectorAdd(pos, dir1);
	//
	if (duration > 0.2f) {
		pos = dir1;
	}
	else {
		XMFLOAT3 center_box = PTR->m_Hit.center_box(magic->caster, magic->vec_bone_name[circle_ix]);
		pos = XMLoadFloat3(&center_box);
		float rand_scale = 0.3f;
		XMVECTOR rand_vec3 = math::rand_unit_vec3();
		rand_vec3 = XMVectorScale(rand_vec3, rand_scale);
		pos = XMVectorAdd(pos, rand_vec3);
	}
	P1 = pos;
	XMStoreFloat3(&edge, P1);
	XMStoreFloat3(&center, pos);
}
//
}
#endif