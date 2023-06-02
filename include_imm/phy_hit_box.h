////////////////
// phy_hit_box.h
// by Huang Yiting
////////////////
////////////////
#ifndef PHY_HIT_BOX_H
#define PHY_HIT_BOX_H
#include "phy_prepare.h"
#include "stru_inst_adapter.h"
#include "phy_magic.h"
#include <map>
namespace imm
{
////////////////
// phy_hit_box
////////////////
////////////////
struct phy_hit_box
{
	phy_hit_box();
	size_t bone_ix;
	XMFLOAT4X4 to_bone;
	XMFLOAT3 extents;
};
//
phy_hit_box::phy_hit_box():
	bone_ix(0),
	extents(0.5f, 0.5f, 0.5f)
{
	XMStoreFloat4x4(&to_bone, XMMatrixIdentity());
}
////////////////
// phy_hit_model
////////////////
////////////////
struct phy_hit_model
{
	std::map<std::string, phy_hit_box> box;
};
////////////////
// phy_hit_arrange
////////////////
////////////////
template <typename T_app>
struct phy_hit_arrange
{
	phy_hit_arrange();
	~phy_hit_arrange();
	void remove_all();
	void init_load(T_app *app_in);
	void read_lua();
	void read_lua_bound_offset();
	void rebuild();
	void rebuild_bbox_from_instance();
	void rebuild_info_from_attachment();
	void update();
	void update_world();
	void update_collision();
	void set_active_box(
		const size_t &inst_ix,
		const std::vector<std::string> &box_name,
		const bool &active = true);
	void deactive_box(const size_t &inst_ix);
	const XMFLOAT3& center_box(const size_t &inst_ix, const std::string &box_name);
	const XMFLOAT4X4& fin_trans_box(const size_t &inst_ix, const std::string &box_name);
	std::vector<BoundingBox> bbox_l;
	std::vector<BoundingBox> bbox_w;
	std::vector<bool> is_active_box;
	std::vector<bool> is_active_att;
	// unarmed map:
	// # atk_model[model_name] = phy_hit_model
	// # map_box_active[instance_ix][box_name] = bbox_ix = is_active_box_ix
	// # map_box_owner[bbox_ix] = instance_ix
	std::map<std::string, phy_hit_model> atk_model;
	std::map<size_t, std::map<std::string, size_t>> map_box_active;
	std::map<size_t, std::map<std::string, XMFLOAT4X4>> map_box_fin_trans;
	std::map<size_t, size_t> map_box_owner;
	// bound correction:
	std::map<std::string, std::vector<float>> model_bound_offset;
	// weapon map:
	// # map_att_active[instance_ix][weapon_name] = is_active_att_ix
	// # map_att_ix[is_active_att_ix] = weapon_ix
	std::map<size_t, std::map<std::string, size_t>> map_att_active;
	std::map<size_t, size_t> map_att_ix;
	std::map<size_t, std::map<size_t, bool> > map_touch;
	std::map<size_t, std::map<size_t, float> > map_touch_cd;
	T_app *app;
	float cd_touch;
};
//
template <typename T_app>
phy_hit_arrange<T_app>::phy_hit_arrange():
	app(nullptr),
	cd_touch(0.8f)
{
	;
}
//
template <typename T_app>
phy_hit_arrange<T_app>::~phy_hit_arrange()
{
	;
}
//
template <typename T_app>
void phy_hit_arrange<T_app>::remove_all()
{
	map_box_active.clear();
	map_box_fin_trans.clear();
	map_box_owner.clear();
	bbox_l.clear();
	bbox_w.clear();
	is_active_box.clear();
	is_active_att.clear();
	map_att_active.clear();
	map_att_ix.clear();
}
//
template <typename T_app>
void phy_hit_arrange<T_app>::init_load(T_app *app_in)
{
	app = app_in;
	read_lua();
	read_lua_bound_offset();
}
//
template <typename T_app>
void phy_hit_arrange<T_app>::read_lua()
{
	std::string concrete = IMM_PATH["script"]+"concrete_common.lua";
	lua_reader l_reader;
	l_reader.loadfile(concrete);
	std::vector<std::vector<std::string>> vec2d;
	l_reader.vec2d_str_from_table("csv_hit_box", vec2d);
	for (size_t ix = 1; ix < vec2d.size(); ++ix) {
		atk_model[vec2d[ix][0]].box[vec2d[ix][1]].bone_ix = stoi(vec2d[ix][2]);
		XMMATRIX offset = XMMatrixTranslation(
			stof(vec2d[ix][3]),
			stof(vec2d[ix][4]),
			stof(vec2d[ix][5]));
		XMMATRIX rot = rotation_xyz(vec2d[ix][6]).get_Matrix();
		XMMATRIX to_bone = XMMatrixMultiply(rot, offset);
		XMStoreFloat4x4(&atk_model[vec2d[ix][0]].box[vec2d[ix][1]].to_bone, to_bone);
		//
		if (vec2d[ix][7].size() > 4) {
			std::vector<float> para = csv_string_to_float(vec2d[ix][7], 3, "#");
			atk_model[vec2d[ix][0]].box[vec2d[ix][1]].extents.x = para[0];
			atk_model[vec2d[ix][0]].box[vec2d[ix][1]].extents.y = para[1];
			atk_model[vec2d[ix][0]].box[vec2d[ix][1]].extents.z = para[2];
		}
	}
}
//
template <typename T_app>
void phy_hit_arrange<T_app>::read_lua_bound_offset()
{
	std::string concrete = IMM_PATH["script"]+"concrete_common.lua";
	lua_reader l_reader;
	l_reader.loadfile(concrete);
	std::vector<std::vector<std::string>> vec2d;
	l_reader.vec2d_str_from_table("csv_bound_offset", vec2d);
	for (size_t ix = 1; ix < vec2d.size(); ++ix) {
		std::vector<float> offset;
		offset.push_back(std::stof(vec2d[ix][1]));
		offset.push_back(std::stof(vec2d[ix][2]));
		offset.push_back(std::stof(vec2d[ix][3]));
		offset.push_back(std::stof(vec2d[ix][4]));
		offset.push_back(std::stof(vec2d[ix][5]));
		offset.push_back(std::stof(vec2d[ix][6]));
		model_bound_offset[vec2d[ix][0]] = offset;
	}
}
//
template <typename T_app>
void phy_hit_arrange<T_app>::rebuild()
{
	rebuild_bbox_from_instance();
	rebuild_info_from_attachment();
}
//
template <typename T_app>
void phy_hit_arrange<T_app>::rebuild_bbox_from_instance()
{
	remove_all();
	for (size_t ix = 0; ix != app->m_Inst.m_Stat.size(); ++ix) {
		if (!(app->m_Inst.m_Stat[ix].property & INST_IS_CONTROLLABLE)) continue;
		std::string *model_name = app->m_Inst.m_Stat[ix].get_ModelName();
		if (atk_model.count(*model_name)) {
			for (auto it = atk_model[*model_name].box.begin(); it != atk_model[*model_name].box.end(); ++it) {
				BoundingBox out;
				BoundingBox bbox(XMFLOAT3(0.0f, 0.0f, 0.0f), it->second.extents);
				XMMATRIX to_bone = XMLoadFloat4x4(&it->second.to_bone);
				bbox.Transform(out, to_bone);
				bbox_l.push_back(out);
				bbox_w.push_back(out);
				// build map
				map_box_active[ix][it->first] = bbox_l.size()-1;
				map_box_owner[bbox_l.size()-1] = ix;
			}
		}
	}
	is_active_box.resize(bbox_w.size(), false);
}
//
template <typename T_app>
void phy_hit_arrange<T_app>::rebuild_info_from_attachment()
{
	app->m_Inst.m_Adapter.attach_flush();
	std::vector<inst_attachment> (*att) = &app->m_Inst.m_Adapter.attach;
	for (size_t ix = 0; ix != (*att).size(); ++ix) {
		if ((*att)[ix].is_enable) {
			// attachment's name may be different form ski_data's box name, let them be the same
			auto *ski_data = &app->m_Control.atk.ski_data[*app->m_Inst.m_Stat[(*att)[ix].owner_ix].get_ModelName()];
			std::string weapon_name = (*att)[ix].name;
			for (auto &vec_box: ski_data->atk_box) {
				for (auto &box: vec_box) {
					if (weapon_name.find(box) != std::string::npos) weapon_name = box;
				}
			}
			is_active_att.push_back(false);
			map_att_active[(*att)[ix].owner_ix][weapon_name] = is_active_att.size()-1;
			map_att_ix[is_active_att.size()-1] = ix;
		}
	}
}
//
template <typename T_app>
void phy_hit_arrange<T_app>::update()
{
	update_world();
	update_collision();
	//
	float dt = app->m_Timer.delta_time();
	for (auto &cdix: map_touch_cd) {
	for (auto &cd: cdix.second) {
		if (cd.second > 0.0f) {
			cd.second -= dt;
		}
		else {
			if (map_touch[cdix.first][cd.first]) {
				map_touch[cdix.first][cd.first] = false;
			}
		}
	}}
}
//
template <typename T_app>
void phy_hit_arrange<T_app>::update_world()
{
	assert(!app->m_Cmd.is_waiting_for_something());
	for (auto it_map = map_box_active.begin(); it_map != map_box_active.end(); ++it_map) {
		XMMATRIX world = XMLoadFloat4x4(app->m_Inst.m_Stat[it_map->first].get_World());
		for (auto it_box = it_map->second.begin(); it_box != it_map->second.end(); ++it_box) {
			size_t bone_ix =
				app->m_Hit.atk_model[*app->m_Inst.m_Stat[it_map->first].get_ModelName()].box[it_box->first].bone_ix;
			XMMATRIX fin_trans = XMLoadFloat4x4(
				app->m_Inst.m_Stat[it_map->first].get_FinalTransform(bone_ix)
			);
			fin_trans = XMMatrixMultiply(fin_trans, world);
			bbox_l[it_box->second].Transform(bbox_w[it_box->second], fin_trans);
			XMStoreFloat4x4(&map_box_fin_trans[it_map->first][it_box->first], fin_trans);
		}
	}
}
//
template <typename T_app>
void phy_hit_arrange<T_app>::update_collision()
{
	// unarmed
	for (size_t ix = 0; ix != is_active_box.size(); ++ix) {
		if (!is_active_box[ix]) continue;
		for (size_t ix_inst = 0; ix_inst != app->m_Inst.m_Stat.size(); ++ix_inst) {
			if (ix_inst == map_box_owner[ix]) continue;
			if (!app->m_Inst.m_Stat[ix_inst].is_invoke_physics()) continue;
			if (static_cast<int>(ix_inst) == app->m_Inst.m_PlaneLandIx) continue;
			if (app->m_Inst.m_Troll[ix_inst].battle_stat & BATTLE_STAT_DODGE) continue;
			if (map_touch[map_box_owner[ix]][ix_inst]) continue;
			bool is_touch = app->m_Inst.m_BoundW.intersects(ix_inst, bbox_w[ix]);
			if (is_touch) {
				map_touch[map_box_owner[ix]][ix_inst] = true;
				map_touch_cd[map_box_owner[ix]][ix_inst] = cd_touch;
				app->m_Control.atk.cause_damage(
					map_box_owner[ix],
					ix_inst,
					bbox_w[ix].Center,
					SKILL_MELEE_STANDARD
				);
			}
			app->m_PhyPos.attack_impulse(
				app->m_Inst.m_Stat[ix_inst].phy,
				bbox_w[ix].Center,
				app->m_Inst.m_BoundW.center(ix_inst),
				app->m_Inst.m_BoundW.center(map_box_owner[ix]),
				is_touch,
				app->m_Control.atk.current_impulse(map_box_owner[ix]),
				ix_inst
			);
		}
	}
	// weapon
	std::vector<inst_attachment> (*att) = &app->m_Inst.m_Adapter.attach;
	for (size_t ix = 0; ix != is_active_att.size(); ++ix) {
		if (!is_active_att[ix]) continue;
		for (size_t ix_inst = 0; ix_inst != app->m_Inst.m_Stat.size(); ++ix_inst) {
			size_t owner_ix = (*att)[map_att_ix[ix]].owner_ix;
			size_t weapon_ix = (*att)[map_att_ix[ix]].ix;
			if (ix_inst == owner_ix) continue;
			if (!app->m_Inst.m_Stat[ix_inst].is_invoke_physics()) continue;
			if (static_cast<int>(ix_inst) == app->m_Inst.m_PlaneLandIx) continue;
			if (app->m_Inst.m_Troll[ix_inst].battle_stat & BATTLE_STAT_DODGE) continue;
			if (map_touch[owner_ix][ix_inst]) continue;
			bool is_touch = app->m_Inst.m_BoundW.intersects(ix_inst, weapon_ix);
			map_touch[owner_ix][ix_inst] &= is_touch;
			if (is_touch) {
				map_touch[owner_ix][ix_inst] = true;				
				map_touch_cd[owner_ix][ix_inst] = cd_touch;
				app->m_Control.atk.cause_damage(
					owner_ix,
					ix_inst,
					app->m_Inst.m_BoundW.center(weapon_ix),
					SKILL_MELEE_STANDARD
				);
			}
			app->m_PhyPos.attack_impulse(
				app->m_Inst.m_Stat[ix_inst].phy,
				app->m_Inst.m_BoundW.center(ix),
				app->m_Inst.m_BoundW.center(ix_inst),
				app->m_Inst.m_BoundW.center(owner_ix),
				is_touch,
				app->m_Control.atk.current_impulse(owner_ix),
				ix_inst
			);
		}
	}
}
//
template <typename T_app>
void phy_hit_arrange<T_app>::set_active_box(
	const size_t &inst_ix,
	const std::vector<std::string> &box_name,
	const bool &active)
{
	map_touch.clear();
	assert(map_box_active.count(inst_ix) || map_att_active.count(inst_ix));
	// unarmed
	if (map_box_active.count(inst_ix)) {
		for (auto &name: box_name) {
			if (!map_box_active[inst_ix].count(name)) continue;
			is_active_box[map_box_active[inst_ix][name]] = active;
		}
	}
	// weapon
	if (map_att_active.count(inst_ix)) {
		for (auto &name: box_name) {
			if (!map_att_active[inst_ix].count(name)) continue;
			is_active_att[map_att_active[inst_ix][name]] = active;
		}
	}
}
//
template <typename T_app>
void phy_hit_arrange<T_app>::deactive_box(const size_t &inst_ix)
{
	map_touch.clear();
	assert(map_box_active.count(inst_ix) || map_att_active.count(inst_ix));
	// unarmed
	if (map_box_active.count(inst_ix)) {
		for (auto &box: map_box_active[inst_ix]) {
			is_active_box[box.second] = false;
		}
	}
	// weapon
	if (map_att_active.count(inst_ix)) {
		for (auto &box: map_att_active[inst_ix]) {
			is_active_att[box.second] = false;
		}
	}
}
//
template <typename T_app>
const XMFLOAT3& phy_hit_arrange<T_app>::center_box(const size_t &inst_ix, const std::string &box_name)
{
	return bbox_w.at(map_box_active.at(inst_ix).at(box_name)).Center;
}
//
template <typename T_app>
const XMFLOAT4X4& phy_hit_arrange<T_app>::fin_trans_box(const size_t &inst_ix, const std::string &box_name)
{
	return map_box_fin_trans.at(inst_ix).at(box_name);
}
//
}
#endif