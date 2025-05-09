////////////////
// phy_ai_probe.h
// by Huang Yiting
////////////////
////////////////
#ifndef PHY_AI_PROBE_H
#define PHY_AI_PROBE_H
#include "phy_obstacle_avoid.h"
namespace imm
{
////////////////
// ai_bound
////////////////
////////////////
struct ai_bound
{
	ai_bound();
	void init(
		CXMMATRIX &rot_inv,
		const float &radius_inst_in,
		const float &radius_close_in,
		const float &radius_alert_in,
		const float &extents_z_oblong_in);
	void transform_close(CXMMATRIX &world);
	void transform_alert(CXMMATRIX &world);
	void transform_oblong(CXMMATRIX &world);
	void transform_obshort(CXMMATRIX &world);
	BoundingSphere CloseL;
	BoundingSphere CloseW;
	BoundingOrientedBox AlertL;
	BoundingOrientedBox AlertW;
	BoundingOrientedBox OblongL;
	BoundingOrientedBox OblongW;
	BoundingOrientedBox ObshortL;
	BoundingOrientedBox ObshortW;
	bool is_active;
	float radius_inst;
	float radius_close;
	float extents_z_oblong;
	float radius_alert;
	float dt_close;
	float dt_alert;
	float dt_oblong;
	float dt_oblong_trans;
	float dt_obshort_trans;
	float max_see_ahead;
};
//
ai_bound::ai_bound():
	is_active(false),
	radius_inst(0.0f),
	radius_close(3.0f),
	radius_alert(30.0f),
	extents_z_oblong(5.0f),
	dt_close(0.0f),
	dt_oblong(0.0f),
	dt_oblong_trans(0.0f),
	dt_obshort_trans(0.0f),
	max_see_ahead(15.0f)
{
	;
}
//
void ai_bound::init(
	CXMMATRIX &rot_inv,
	const float &radius_inst_in,
	const float &radius_close_in,
	const float &radius_alert_in,
	const float &extents_z_oblong_in)
{
	radius_inst = radius_inst_in;
	radius_close = radius_close_in;
	radius_alert = radius_alert_in;
	extents_z_oblong = extents_z_oblong_in;
	max_see_ahead = extents_z_oblong_in;
	CloseL.Radius = radius_inst+radius_close;
	CloseL.Center = XMFLOAT3(0.0f, 0.0f, 0.0f);
	CloseW = CloseL;
	AlertL.Center = XMFLOAT3(0.0f, 0.0f, -radius_alert+radius_inst);
	AlertL.Extents = XMFLOAT3(radius_alert, radius_inst, radius_inst+radius_alert);
	AlertL.Orientation = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	AlertL.Transform(AlertW, rot_inv);
	AlertL = AlertW;
	OblongL.Center = XMFLOAT3(0.0f, 0.0f, -extents_z_oblong-radius_inst);
	OblongL.Extents = XMFLOAT3(radius_inst*0.3f, radius_inst*3.0f, radius_inst+extents_z_oblong);
	OblongL.Orientation = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	// Blender's mesh is right-hander, after export to left-handed,
	// mesh will face toward ground
	OblongL.Transform(OblongW, rot_inv);
	OblongL = OblongW;
	//
	float short_adj = 0.3f;
	ObshortL.Center = XMFLOAT3(0.0f, 0.0f, (-extents_z_oblong)*short_adj-radius_inst);
	ObshortL.Extents = XMFLOAT3(radius_inst*0.3f, radius_inst*3.0f, radius_inst+(extents_z_oblong*short_adj));
	ObshortL.Orientation = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	ObshortL.Transform(ObshortW, rot_inv);
	ObshortL = ObshortW;
	dt_close = math::calc_randf(-AI_DT_PHY_FAST, AI_DT_PHY_FAST);
	dt_alert = math::calc_randf(-AI_DT_PHY_FAST, AI_DT_PHY_FAST);
	dt_oblong = math::calc_randf(-AI_DT_PHY_SLOW, AI_DT_PHY_SLOW);
	dt_oblong_trans = math::calc_randf(-AI_DT_PHY_FAST, AI_DT_PHY_FAST);
	dt_obshort_trans = math::calc_randf(-AI_DT_PHY_FAST, AI_DT_PHY_FAST);
}
//
void ai_bound::transform_close(CXMMATRIX &world) {CloseL.Transform(CloseW, world);}
void ai_bound::transform_alert(CXMMATRIX &world) {AlertL.Transform(AlertW, world);}
void ai_bound::transform_oblong(CXMMATRIX &world) {OblongL.Transform(OblongW, world);}
void ai_bound::transform_obshort(CXMMATRIX &world) {ObshortL.Transform(ObshortW, world);}
////////////////
// ai_probe
////////////////
////////////////
template <typename T_app>
struct ai_probe
{
	ai_probe();
	void init(T_app *app_in);
	void reset();
	void rebuild();
	void update(const float &dt);
	void close_test(const size_t &ix_probe, const size_t &ix_object);
	void alert_test(const size_t &ix_probe, const size_t &ix_object);
	void obstacle_avoid_candidate(
		const size_t &ix_probe,
		const size_t &ix_object,
		int &candidate_ix,
		const XMVECTOR &center_inst,
		float &distance_near,
		float &radius_obj);
	void set_active(const size_t &ix);
	bool intersects_oblong(const size_t &ix_inst, const size_t &ix_tar);
	bool intersects_obshort_obj(const size_t &ix_inst, const size_t &ix_obj);
	std::map<size_t, ai_bound> geometry;
	std::vector<int> candidate_vec;
	T_app *app;
};
//
template <typename T_app>
ai_probe<T_app>::ai_probe():
	app(nullptr)
{
	;
}
//
template <typename T_app>
void ai_probe<T_app>::init(T_app *app_in)
{
	app = app_in;
}
//
template <typename T_app>
void ai_probe<T_app>::reset()
{
	geometry.clear();
}
//
template <typename T_app>
void ai_probe<T_app>::rebuild()
{
	reset();
	for (auto &stat: app->m_Inst.m_Stat) {
		if (stat.property & INST_IS_CONTROLLABLE) {
			size_t ix = &stat - &app->m_Inst.m_Stat[0];
			XMMATRIX rot_front = XMLoadFloat4x4(app->m_Inst.m_Stat[ix].get_RotFront());
			XMVECTOR out_scale, out_trans, out_rot;
			assert(XMMatrixDecompose(&out_scale, &out_rot, &out_trans, rot_front));
			out_rot = XMQuaternionInverse(out_rot);
			XMMATRIX rot_inv = XMMatrixRotationQuaternion(out_rot);
			//
			float radius_inst = (app->m_Inst.m_BoundL.extents_x(ix)+app->m_Inst.m_BoundL.extents_z(ix))*0.5f;
			geometry[ix].init(rot_inv, radius_inst, 4.0f, 10.0f, 5.0f);
		}
	}
}
//
template <typename T_app>
void ai_probe<T_app>::update(const float &dt)
{
	for (auto &geo: geometry) {
	if (ALWAYS_TRUE) {
		geo.second.dt_oblong_trans += dt;
		geo.second.dt_obshort_trans += dt;
	}
	if (!geo.second.is_active) continue;
	// separate update
	if (app->m_AiInterf.get_current_tactics(geo.first) & AI_TAC_SEEK ||
		app->m_AiInterf.get_current_tactics(geo.first) & AI_TAC_ATK) {
		geo.second.dt_close += dt;
	}
	if (app->m_AiInterf.get_current_tactics(geo.first) & AI_TAC_PATROL ||
		app->m_AiInterf.get_current_tactics(geo.first) & AI_TAC_FOLLOW) {
		geo.second.dt_alert += dt;
	}
	if (!app->m_Control.map_stop[geo.first].is_stop) {
		geo.second.dt_oblong += dt;
	}
	//
	if (geo.second.dt_close > AI_DT_PHY_FAST) {
		geo.second.dt_close -= AI_DT_PHY_FAST;
		geo.second.transform_close(XMLoadFloat4x4(app->m_Inst.m_Stat[geo.first].get_World()));
		for (size_t ix = 0; ix != app->m_Inst.m_Stat.size(); ++ix) {
			close_test(geo.first, ix);
		}
	}
	if (geo.second.dt_alert > AI_DT_PHY_FAST) {
		geo.second.dt_alert -= AI_DT_PHY_FAST;
		geo.second.transform_alert(XMLoadFloat4x4(app->m_Inst.m_Stat[geo.first].get_World()));
		app->m_Inst.m_Steering[geo.first].alert.clear();
		for (size_t ix = 0; ix != app->m_Inst.m_Stat.size(); ++ix) {
			alert_test(geo.first, ix);
		}
	}
	if (geo.second.dt_oblong > AI_DT_PHY_2SLOW) {
		geo.second.dt_oblong -= AI_DT_PHY_2SLOW;
		//
		geo.second.dt_oblong_trans = 0.0f;
		geo.second.transform_oblong(XMLoadFloat4x4(app->m_Inst.m_Stat[geo.first].get_World()));
		//
		XMVECTOR center_inst = XMLoadFloat3(&app->m_Inst.m_BoundW.center(geo.first));
		float distance_near = FLT_MAX;
		float radius_obj = 0.0f;
		int candidate_ix = -1;
		//
		candidate_vec.clear();
		app->m_Control.map_stop[geo.first].is_avoidance = false;
		for (size_t ix = 0; ix != app->m_Inst.m_Stat.size(); ++ix) {
			int temp = candidate_ix;
			obstacle_avoid_candidate(geo.first, ix, candidate_ix, center_inst, distance_near, radius_obj);
			if (candidate_ix == -1) continue;
			if (candidate_ix != temp) candidate_vec.push_back(candidate_ix);
		}
		if (candidate_ix == -1) return;
		XMFLOAT3 avoidance_force;
		bool test_short_obj = false;
		for (auto &can_ix : candidate_vec) {
			test_short_obj = intersects_obshort_obj(geo.first, can_ix);
			if (test_short_obj) break;
		}
		//
		phy_collision_avoidance(
			app->m_Inst.m_Stat[geo.first].phy,
			center_inst,
			XMLoadFloat3(&app->m_Inst.m_BoundW.center(candidate_ix)),
			geo.second.max_see_ahead,
			avoidance_force
		);		
		// estimate path length
		float path_len_est = geometry[geo.first].OblongW.Extents.z*2.0f;
		path_len_est += radius_obj*2.0f+math::calc_randf(-1.0f, 1.0f);
		if (distance_near < radius_obj*2.0f) {
			path_len_est -= distance_near;
		}
		float avoid_time = path_len_est/app->m_Inst.m_Troll[geo.first].speed_move();
		if (test_short_obj) avoid_time *= 0.5f;
		app->m_Control.map_stop[geo.first].avoid_time = avoid_time;
		app->m_Control.map_stop[geo.first].avoidance = avoidance_force;
		app->m_Control.map_stop[geo.first].is_avoidance = true;
	}
	}
}
//
template <typename T_app>
void ai_probe<T_app>::close_test(const size_t &ix_probe, const size_t &ix_object)
{
	if (app->m_Inst.m_Stat[ix_object].is_special_physics()) return;
	if (!app->m_Inst.m_Stat[ix_object].is_invoke_physics()) return;
	bool is_close = app->m_Inst.m_BoundW.intersects(ix_object, geometry[ix_probe].CloseW);
	app->m_Inst.m_Steering[ix_probe].close[ix_object] = is_close;
}
//
template <typename T_app>
void ai_probe<T_app>::alert_test(const size_t &ix_probe, const size_t &ix_object)
{
	if (app->m_Inst.m_Stat[ix_object].is_special_physics()) return;
	if (!app->m_Inst.m_Stat[ix_object].is_invoke_physics()) return;
	if (~app->m_Inst.m_Stat[ix_object].property & INST_IS_CONTROLLABLE) return;
	bool is_alert = app->m_Inst.m_BoundW.intersects(ix_object, geometry[ix_probe].AlertW);
	if (is_alert) app->m_Inst.m_Steering[ix_probe].alert[ix_object] = is_alert;
}
//
template <typename T_app>
void ai_probe<T_app>::obstacle_avoid_candidate(
		const size_t &ix_probe,
		const size_t &ix_object,
		int &candidate_ix,
		const XMVECTOR &center_inst,
		float &distance_near,
		float &radius_obj)
{
	// phy_obstacle_avoid
	if (app->m_Inst.m_Stat[ix_object].is_special_physics()) return;
	if (!app->m_Inst.m_Stat[ix_object].is_invoke_physics()) return;
	if (app->m_Inst.m_Stat[ix_object].property & INST_IS_CONTROLLABLE) return;
	float radius_test = (app->m_Inst.m_BoundW.extents_x(ix_object)+app->m_Inst.m_BoundW.extents_z(ix_object))*0.5f;
	if (radius_test < geometry[ix_probe].radius_inst*0.5f) return;
	bool is_intersect = app->m_Inst.m_BoundW.intersects(ix_object, geometry[ix_probe].OblongW);
	if (!is_intersect) return;
	//
	XMVECTOR center_obj = XMLoadFloat3(&app->m_Inst.m_BoundW.center(ix_object));
	XMVECTOR difference = XMVectorSubtract(center_obj, center_inst);
	difference = XMVector3LengthEst(difference);
	float distance = XMVectorGetX(difference);
	if (distance_near > distance) {
		distance_near = distance;
		candidate_ix = static_cast<int>(ix_object);
		radius_obj = radius_test;
	}
}
//
template <typename T_app>
void ai_probe<T_app>::set_active(const size_t &ix)
{
	geometry[ix].is_active = true;
}
//
template <typename T_app>
bool ai_probe<T_app>::intersects_oblong(const size_t &ix_inst, const size_t &ix_tar)
{
	if (geometry[ix_inst].dt_oblong_trans > AI_DT_PHY_SLOW) {
		geometry[ix_inst].transform_oblong(XMLoadFloat4x4(app->m_Inst.m_Stat[ix_inst].get_World()));
		geometry[ix_inst].dt_oblong_trans = 0.0f;
	}
	return app->m_Inst.m_BoundW.intersects(ix_tar, geometry[ix_inst].OblongW);
}
//
template <typename T_app>
bool ai_probe<T_app>::intersects_obshort_obj(const size_t &ix_inst, const size_t &ix_obj)
{
	if (geometry[ix_inst].dt_obshort_trans > AI_DT_PHY_SLOW) {
		geometry[ix_inst].transform_obshort(XMLoadFloat4x4(app->m_Inst.m_Stat[ix_inst].get_World()));
		geometry[ix_inst].dt_obshort_trans = 0.0f;
	}
	bool is_intersect = app->m_Inst.m_BoundW.intersects(ix_obj, geometry[ix_inst].ObshortW);
	return is_intersect;
}
//
}
#endif