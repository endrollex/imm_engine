////////////////
// stru_instance_mgr.h
// by Huang Yiting
////////////////
////////////////
#ifndef STRU_INSTANCE_MGR_H
#define STRU_INSTANCE_MGR_H
#include "ai_steering.h"
#include "phy_ai_probe.h"
#include "stru_inst_adapter.h"
#include "stru_build_inst.h"
#include "stru_model_mgr.h"
#include "control_state.h"
namespace imm
{
////////////////
// instance_mgr
////////////////
////////////////
template <typename T_app>
struct instance_mgr
{
	instance_mgr();
	void init(T_app *app_in);
	void reload();
	void reload_scene_instance_relate();
	template <typename instance, typename get_pos>
	void push_back_basic(
		std::vector<instance> &v_inst,
		instance_stat &inst_stat,
		size_t &k,
		const get_pos &get_pos_f,
		const std::map<size_t, std::string> &name);
	template <typename instance, typename get_pos>
	void push_back_pntt(
		std::vector<instance> &v_inst,
		instance_stat &inst_stat,
		size_t &k,
		const get_pos &get_pos_f,
		const std::map<size_t, std::string> &name);
	void copy_instance(const std::string &inst_name, const std::string &new_name);
	int get_index(const std::string &name);
	void on_resize();
	void update_all_physics1(const float &dt);
	void update_all_physics2(const float &dt);
	void update_bound();
	void update_phy_misc();
	void update_collision(float dt);
	void update_collision_impulse(float dt);
	void update_collision_plane(float dt);
	void update_collision_terrain(float dt);
	void update_collision_terrain_height(const size_t &ix, float &get_height);
	void update_collision_liquid(float dt);
	void update_skinned(const float &dt);
	void update_frustum_culling();
	void remove_all();
	model_mgr m_Model;
	phy_bound_mgr<T_app> m_BoundL;
	phy_bound_mgr<T_app> m_BoundW;
	inst_adapter<T_app> m_Adapter;
	inst_build<T_app> m_Build;
	ai_probe<T_app> m_Probe;
	BoundingFrustum m_CamFrustumL;
	BoundingFrustum m_CamFrustumW;
	std::vector<instance_stat> m_Stat;
	std::vector<troll> m_Troll;
	std::map<size_t, steering> m_Steering;
	std::map<std::string, size_t> m_NameMap;
	std::map<size_t, std::string> m_IndexMap;
	bool m_IsLoading;
	bool m_IsTerrainUse;
	int m_PlaneLandIx;
	T_app *m_App;
};
//
template <typename T_app>
instance_mgr<T_app>::instance_mgr():
	m_Model(),
	m_BoundL(),
	m_BoundW(),
	m_Adapter(),
	m_Build(),
	m_Probe(),
	m_IsLoading(false),
	m_IsTerrainUse(false),
	m_PlaneLandIx(-1),
	m_App(nullptr)
{
	;
}
//
template <typename T_app>
void instance_mgr<T_app>::init(T_app *app_in)
{
	m_App = app_in;
	m_BoundL.init(m_App);
	m_BoundW.init(m_App);
	m_Adapter.init_load(m_App);
	m_Build.init(m_App);
	m_Probe.init(m_App);
}
//
template <typename T_app>
void instance_mgr<T_app>::reload()
{
	std::string scene_ix(m_App->m_Scene.scene_ix);
	std::wstring load_done(txtutil::str_to_wstr(scene_ix));
	if (!m_Model.load(m_App->m_D3DDevice, scene_ix)) {
		load_done = L"> Scene "+load_done+L" not found.\n";
		m_App->m_Cmd.input += load_done;
		return;
	}
	remove_all();
	instance_stat inst_stat;
	size_t k = 0;
	inst_stat.type = MODEL_BASIC;
	push_back_basic(
		m_Model.m_InstBasic,
		inst_stat,
		k,
		[](const vertex::pntt2 &x) {return &x.pos;},
		m_Model.m_NameBasic);
	push_back_basic(
		m_Model.m_InstBasicAlpha,
		inst_stat,
		k,
		[](const vertex::pntt2 &x) {return &x.pos;},
		m_Model.m_NameBasicAlpha);
	inst_stat.type = MODEL_SKINNED;
	push_back_basic(
		m_Model.m_InstSkinned,
		inst_stat,
		k,
		[](const vertex::pntt_skinned &x) {return &x.pos;},
		m_Model.m_NameSkinned);
	push_back_basic(
		m_Model.m_InstSkinnedAlpha,
		inst_stat,
		k,
		[](const vertex::pntt_skinned &x) {return &x.pos;},
		m_Model.m_NameSkinnedAlpha);
	inst_stat.type = MODEL_SIMPLE_P;
	push_back_pntt(
		m_Model.m_InstPNTT,
		inst_stat,
		k,
		[](const vertex::pntt &x) {return &x.pos;},
		m_Model.m_NamePNTT);
	//
	// controllable
	m_Troll.resize(m_Stat.size());
	for (size_t ix = 0; ix != m_Troll.size(); ++ix) {
		m_Troll[ix].init(ix);
		if (m_App->m_Control.atk.ski_data.count(*m_Stat[ix].get_ModelName())) {
			m_Stat[ix].property |= INST_IS_CONTROLLABLE;
			m_Steering[ix].init(ix);
		}
	}
	assert(m_NameMap.size() == m_Stat.size());
	for (auto &name: m_NameMap) {
		m_IndexMap[name.second] = name.first;
	}
	//
	reload_scene_instance_relate();
	on_resize();
	m_IsLoading = false;
}
//
template <typename T_app>
void instance_mgr<T_app>::reload_scene_instance_relate()
{
	if (csv_value_is_empty(m_App->m_Scene.get_misc["terrain_info"])) {
		m_PlaneLandIx = get_index(m_App->m_Scene.get_misc["plane_land"]);
		m_Stat[m_PlaneLandIx].property |= INST_IS_LAND;
		m_IsTerrainUse = false;
	}
	else {
		m_PlaneLandIx = -1;
		m_IsTerrainUse = true;
	}
	// after instance load over
	m_App->m_Control.rebuild();
	m_App->m_Hit.rebuild();
	m_Adapter.rebuild();
	m_App->m_AiAttr.rebuild();
	m_App->m_AiInterf.rebuild();
	m_App->m_AiInfo.rebuild();
	m_Probe.rebuild();
	m_App->m_Scene.phy_wire.rebuild_buffer();
}
//
template <typename T_app>
template <typename instance, typename get_pos>
void instance_mgr<T_app>::push_back_basic(
	std::vector<instance> &v_inst,
	instance_stat &inst_stat,
	size_t &k,
	const get_pos &get_pos_f,
	const std::map<size_t, std::string> &name)
{
	for (size_t ix = 0; ix != v_inst.size(); ++ix) {
		m_NameMap[name.at(ix)] = k++;
		inst_stat.p_inst = &v_inst[ix];
		inst_stat.index = ix;
		m_Stat.push_back(inst_stat);
		v_inst[ix].stat_ix = m_Stat.size()-1;
		m_BoundL.push_back_empty(m_Stat.back().get_BoundType());
		m_BoundW.push_back_empty(m_Stat.back().get_BoundType());
		switch(m_Stat.back().get_BoundType()) {
		case PHY_BOUND_BOX:
		case PHY_BOUND_ORI_BOX:
			m_BoundL.phy_set_box_back(
				m_Stat.back().get_BoundType(),
				v_inst[ix].model->m_Vertices,
				get_pos_f
			);
			if (m_App->m_Hit.model_bound_offset.count(*inst_stat.get_ModelName())) {
				m_BoundL.set_box_offset(k-1, m_App->m_Hit.model_bound_offset[*inst_stat.get_ModelName()]);
			}
			break;
		case PHY_BOUND_SPHERE:
			// not implement
			assert(false);
			break;
		}
		XMMATRIX world = XMLoadFloat4x4(inst_stat.get_World());
		m_BoundL.transform(k-1, m_BoundW, world);
		m_BoundW.set_phy_value(k-1);
	}
}
//
template <typename T_app>
template <typename instance, typename get_pos>
void instance_mgr<T_app>::push_back_pntt(
	std::vector<instance> &v_inst,
	instance_stat &inst_stat,
	size_t &k,
	const get_pos &get_pos_f,
	const std::map<size_t, std::string> &name)
{
	for (size_t ix = 0; ix != v_inst.size(); ++ix) {
		m_NameMap[name.at(ix)] = k++;
		inst_stat.p_inst = &v_inst[ix];
		inst_stat.index = ix;
		m_Stat.push_back(inst_stat);
		v_inst[ix].stat_ix = m_Stat.size()-1;
		m_BoundL.push_back_empty(m_Stat.back().get_BoundType());
		m_BoundW.push_back_empty(m_Stat.back().get_BoundType());
		auto vert_range = v_inst[ix].model->get_VertexRange(v_inst[ix].subid);
		switch(m_Stat.back().get_BoundType()) {
		case PHY_BOUND_BOX:
		case PHY_BOUND_ORI_BOX:
			m_BoundL.phy_set_box_back(
				m_Stat.back().get_BoundType(),
				v_inst[ix].model->m_Vertices,
				get_pos_f,
				vert_range.first,
				vert_range.second
			);
			if (m_App->m_Hit.model_bound_offset.count(*inst_stat.get_ModelName())) {
				assert(false);;
			}
			break;
		case PHY_BOUND_SPHERE:
			// not implement
			assert(false);
			break;
		}
		XMMATRIX world = XMLoadFloat4x4(inst_stat.get_World());
		m_BoundL.transform(k-1, m_BoundW, world);
		m_BoundW.set_phy_value(k-1);
	}
}
//
template <typename T_app>
void instance_mgr<T_app>::copy_instance(const std::string &inst_name, const std::string &new_name)
{
	if (!m_NameMap.count(inst_name)) assert(false);
	size_t inst_ix = m_NameMap[inst_name];
	size_t size_hold = m_Stat.size();
	m_Model.copy_instance(m_Stat, inst_ix, new_name);
	if (size_hold == m_Stat.size()-1) {
		m_NameMap[new_name] = size_hold;
		m_IndexMap[size_hold] = new_name;
		assert(m_Troll.size() == size_hold);
		m_Troll.emplace_back();
		m_Troll.at(size_hold).index = size_hold;
		m_BoundL.push_back_empty(m_Stat.back().get_BoundType());
		m_BoundW.push_back_empty(m_Stat.back().get_BoundType());
		XMMATRIX world = XMLoadFloat4x4(m_Stat[size_hold].get_World());
		switch(m_Stat.back().get_BoundType()) {
		case PHY_BOUND_BOX:
			m_BoundL.bd0.back() = m_BoundL.bd0[m_BoundL.map.at(inst_ix).second];
			break;
		case PHY_BOUND_ORI_BOX:
			m_BoundL.bd1.back() = m_BoundL.bd1[m_BoundL.map.at(inst_ix).second];
			m_BoundL.bd1.back().Orientation = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
			break;
		case PHY_BOUND_SPHERE:
			// not implement
			assert(false);
			break;
		}
		m_BoundL.transform(size_hold, m_BoundW, world);
		m_BoundW.set_phy_value(size_hold);
		m_Stat[inst_ix].phy.clear_indirect();
		m_Stat[inst_ix].phy.init(static_cast<int>(inst_ix));
	}
	else {
		assert(false);
	}
}
//
template <typename T_app>
int instance_mgr<T_app>::get_index(const std::string &name)
{
	if (!m_NameMap.count(name)) return -1;
	return static_cast<int>(m_NameMap[name]);
}
//
template <typename T_app>
void instance_mgr<T_app>::on_resize()
{
	if (m_App == nullptr) return;
	XMMATRIX proj = m_App->m_Cam.get_Proj();
	BoundingFrustum::CreateFromMatrix(m_CamFrustumL, proj);
}
//
template <typename T_app>
void instance_mgr<T_app>::update_all_physics1(const float &dt)
{
	float delta = dt;
	if (delta > PHY_DELTA_TIME_MAX) delta = PHY_DELTA_TIME_MAX;
	update_bound();
	update_phy_misc();
	update_collision(delta);
	update_frustum_culling();
	update_collision_liquid(delta);
	m_Adapter.update_world();
	m_Probe.update(delta);
	m_App->m_Hit.update();
	m_App->m_Magic.update(delta);
}
//
template <typename T_app>
void instance_mgr<T_app>::update_all_physics2(const float &dt)
{
	float delta = dt;
	if (delta > PHY_DELTA_TIME_MAX) delta = PHY_DELTA_TIME_MAX;
	update_skinned(delta);
}
//
template <typename T_app>
void instance_mgr<T_app>::update_bound()
{
	XMMATRIX world;
	for (size_t ix = 0; ix != m_Stat.size(); ++ix) {
		world = XMLoadFloat4x4(m_Stat[ix].get_World());
		m_BoundL.transform(ix, m_BoundW, world);
	}
}
//
template <typename T_app>
void instance_mgr<T_app>::update_phy_misc()
{
	for (size_t ix = 0; ix != m_Stat.size(); ++ix) {
		if (m_Stat[ix].phy.is_drop_fix) {
			if (m_Stat[ix].phy.is_vel_zero())  {
				m_Stat[ix].phy.intera_tp = PHY_INTERA_FIXED;
				m_Stat[ix].phy.is_drop_fix = false;
			}
		}
	}
}
// should use octree, temporary not implement
template <typename T_app>
void instance_mgr<T_app>::update_collision(float dt)
{
	// if runtime stun
	if (dt > PHY_MAX_DELTA_TIME) dt = PHY_MAX_DELTA_TIME;
	update_collision_impulse(dt);
	if (m_IsTerrainUse) update_collision_terrain(dt);
	else update_collision_plane(dt);
}
//
template <typename T_app>
void instance_mgr<T_app>::update_collision_impulse(float dt)
{
	//
	for (int ix = 0; ix < static_cast<int>(m_Stat.size()-1); ++ix) {
	for (size_t ix2 = ix+1; ix2 != m_Stat.size(); ++ix2) {
		if (m_Stat[ix].property & INST_IS_LAND || m_Stat[ix2].property & INST_IS_LAND) continue;
		if (!m_Stat[ix].is_invoke_physics() || !m_Stat[ix2].is_invoke_physics()) continue;
		// record sensor
		bool is_touch = m_BoundW.intersects(ix, ix2);
		if (m_Stat[ix].phy.intera_tp == PHY_INTERA_FIXED_LIMIT ||
			m_Stat[ix2].phy.intera_tp == PHY_INTERA_FIXED_LIMIT) is_touch = !is_touch;
		// if instance stand on instance, continue;
		if (m_Stat[ix].phy.stand_on == ix2 || m_Stat[ix2].phy.stand_on == ix) continue;
		//
		m_App->m_PhyPos.impulse_casual(
			dt,
			m_BoundW.extents_y(ix),
			m_BoundW.extents_y(ix2),
			*(m_Stat[ix].get_World()),
			*(m_Stat[ix2].get_World()),
			m_Stat[ix].phy,
			m_Stat[ix2].phy,
			m_BoundW.center(ix),
			m_BoundW.center(ix2),
			is_touch
		);
	}}
}
//
template <typename T_app>
void instance_mgr<T_app>::update_collision_plane(float dt)
{
	if (m_PlaneLandIx < 0) return;
	for (size_t ix = 0; ix != m_Stat.size(); ++ix) {
		//
		if (m_Stat[ix].is_special_physics()) continue;
		if (!m_Stat[ix].is_invoke_physics()) continue;
		// physcis logic
		int ix_land;
		float land_y;
		if (m_Stat[ix].phy.stand_on >= 0) {
			ix_land = m_Stat[ix].phy.stand_on;
			m_Stat[ix].phy.is_on_land = m_BoundW.intersects(ix_land, ix);
			if (m_Stat[ix].phy.is_on_land) {
				land_y = m_BoundW.center(ix_land).y + m_BoundW.extents_y(ix_land);
			}
			else {
				ix_land = m_PlaneLandIx;
				m_Stat[ix].phy.is_on_land = m_BoundW.intersects(ix_land, ix);
				land_y = m_BoundW.center(ix_land).y + m_BoundW.extents_y(ix_land);
				if (m_Stat[ix].phy.is_on_land) {
					m_Stat[ix].phy.stand_on = -1;
				}
			}
		}
		else {
			ix_land = m_PlaneLandIx;
			m_Stat[ix].phy.is_on_land = m_BoundW.intersects(ix_land, ix);
			land_y = m_BoundW.center(ix_land).y + m_BoundW.extents_y(ix_land);
		}
		//
		m_App->m_PhyPos.update(
			dt,
			*(m_Stat[ix].get_World()),
			m_Stat[ix].phy,
			m_Stat[ix_land].phy,
			m_BoundW.center(ix),
			m_BoundW.extents_y(ix),
			land_y);
		//
	}
}
//
template <typename T_app>
void instance_mgr<T_app>::update_collision_terrain(float dt)
{
	for (size_t ix = 0; ix != m_Stat.size(); ++ix) {
		//
		if (!m_Stat[ix].is_invoke_physics()) continue;
		// physcis logic
		int ix_land;
		float height;
		phy_property *phy_gro;
		if (m_Stat[ix].phy.stand_on >= 0) {
			ix_land = m_Stat[ix].phy.stand_on;
			m_Stat[ix].phy.is_on_land = m_BoundW.intersects(ix_land, ix);
			if (m_Stat[ix].phy.is_on_land) {
				height = m_BoundW.center(ix_land).y + m_BoundW.extents_y(ix_land);
			}
			else {
				ix_land = -1;
				update_collision_terrain_height(ix, height);
				if (m_Stat[ix].phy.is_on_land) {
					m_Stat[ix].phy.stand_on = -1;
				}
			}
		}
		else {
			ix_land = -1;
			update_collision_terrain_height(ix, height);
		}
		if (ix_land < 0) phy_gro = &m_App->m_Scene.terrain1_phy;
		else phy_gro = &m_Stat[ix_land].phy;
		//
		m_App->m_PhyPos.update(
			dt,
			*(m_Stat[ix].get_World()),
			m_Stat[ix].phy,
			*phy_gro,
			m_BoundW.center(ix),
			m_BoundW.extents_y(ix),
			height);
		// inaccuracy touch Land, logically assume it touches gourond, because terrain is not flat
		// this is not for physics
		if (ix_land < 0) {
			float extents_y = m_BoundW.extents_y(ix);
			if (m_BoundW.center(ix).y - extents_y - height < 0.3f) m_Stat[ix].phy.is_on_land = true;
		}
	}
}
//
template <typename T_app>
void instance_mgr<T_app>::update_collision_terrain_height(const size_t &ix, float &get_height)
{
	XMFLOAT3 center = m_BoundW.center(ix);
	get_height = m_App->m_Scene.terrain1.get_Height(center.x, center.z);
	XMVECTOR terrain_point = XMVectorSet(center.x, get_height, center.z, 0.0f);
	int contains = m_BoundW.contains(ix, terrain_point);
	m_Stat[ix].phy.is_on_land = (contains != 0);
	float ext_y = m_BoundW.extents_y(ix);
	if (center.y+ext_y < get_height) m_Stat[ix].phy.is_on_land = true;
}
//
template <typename T_app>
void instance_mgr<T_app>::update_collision_liquid(float dt)
{
	for (size_t ix = 0; ix != m_Stat.size(); ++ix) {
		if (m_Stat[ix].is_special_physics()) continue;
		if (!m_Stat[ix].is_invoke_physics()) continue;
		if (m_Stat[ix].phy.intera_tp & PHY_INTERA_FIXED) continue;
		switch(m_BoundW.map[ix].first) {
		case PHY_BOUND_BOX:
			m_App->m_Scene.liquid.intersects(m_BoundW.bd0[m_BoundW.map[ix].second], dt, ix);
			break;
		case PHY_BOUND_ORI_BOX:
			m_App->m_Scene.liquid.intersects(m_BoundW.bd1[m_BoundW.map[ix].second], dt, ix);
			break;
		case PHY_BOUND_SPHERE:
			m_App->m_Scene.liquid.intersects(m_BoundW.bd2[m_BoundW.map[ix].second], dt, ix);
			break;
		}
	}
	m_App->m_Scene.liquid.frustum_culling(m_CamFrustumW);
}
//
template <typename T_app>
void instance_mgr<T_app>::update_skinned(const float &dt)
{
	if (m_App->m_Cmd.is_preparing) return;
	// Troll
	assert(m_Troll.capacity() < VECTOR_RESERVE+1);
	for (auto &troll: m_Troll) troll.update();
	for (auto &ste: m_Steering) ste.second.update(dt);
	// should be multithread?
	for (auto &skinned: m_Model.m_InstSkinned) skinned.update(dt);
	for (auto &skinned: m_Model.m_InstSkinnedAlpha) skinned.update(dt);
}
//
template <typename T_app>
void instance_mgr<T_app>::update_frustum_culling()
{
	XMVECTOR det_view = XMMatrixDeterminant(m_App->m_Cam.get_View());
	XMMATRIX inv_view = XMMatrixInverse(&det_view, m_App->m_Cam.get_View());
	m_CamFrustumL.Transform(m_CamFrustumW, inv_view);
	// disappear if not in frustum
	// remove bounding offset
	m_BoundW.switch_box_alter_offset(false, m_BoundL);
	for (size_t ix = 0; ix != m_Stat.size(); ++ix) {
		switch(m_BoundW.map[ix].first) {
		case PHY_BOUND_BOX:
			m_Stat[ix].set_IsInFrustum(m_CamFrustumW.Intersects(m_BoundW.bd0[m_BoundW.map[ix].second]));
			break;
		case PHY_BOUND_ORI_BOX:
			m_Stat[ix].set_IsInFrustum(m_CamFrustumW.Intersects(m_BoundW.bd1[m_BoundW.map[ix].second]));
			break;
		case PHY_BOUND_SPHERE:
			m_Stat[ix].set_IsInFrustum(m_CamFrustumW.Intersects(m_BoundW.bd2[m_BoundW.map[ix].second]));
			break;
		}
		m_Stat[ix].phy.is_on_screen = m_Stat[ix].is_in_frustum();
	}
	m_BoundW.switch_box_alter_offset(true, m_BoundL);
	// shpere test
	BoundingSphere p1shpere;
	p1shpere.Center = m_BoundW.center(m_App->m_Control.player1);
	p1shpere.Radius = 70.0f;
	for (size_t ix = 0; ix != m_Stat.size(); ++ix) {
		if (m_Stat[ix].get_IsInFrustum()) continue;
		// big object always display, because of small scene
		if (m_Stat[ix].phy.avg_extent > 20.0f) {m_Stat[ix].set_IsInFrustum(true); continue;}
		XMVECTOR obj_center = XMLoadFloat3(&m_BoundW.center(ix));
		ContainmentType test = p1shpere.Contains(obj_center);
		if (test != DISJOINT) m_Stat[ix].set_IsInFrustum(true);
	}
}
//
template <typename T_app>
void instance_mgr<T_app>::remove_all()
{
	m_BoundL.remove_all();
	m_BoundW.remove_all();
	m_Stat.clear();
	m_Troll.clear();
	m_Steering.clear();
	m_NameMap.clear();
	m_IndexMap.clear();
	m_Stat.reserve(VECTOR_RESERVE);
	m_Troll.reserve(VECTOR_RESERVE);
	m_Build.remove_all();
}
//
}
#endif