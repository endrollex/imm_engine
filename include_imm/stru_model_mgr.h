////////////////
// stru_model_mgr.h
// by Huang Yiting
////////////////
////////////////
#ifndef STRU_MODEL_MGR_H
#define STRU_MODEL_MGR_H
#include "phy_position_logic.h"
#include "stru_load_help.h"
#include "mesh_basic_model.h"
#include "stru_scene_swatch.h"
#include <thread>
namespace imm
{
////////////////
// MODEL_TYPE
////////////////
////////////////
enum MODEL_TYPE
{
	MODEL_BASIC,
	MODEL_SKINNED,
	MODEL_SIMPLE_P,
};
////////////////
// INST_PROPERTY
////////////////
////////////////
enum INST_PROPERTY
{
	INST_IS_NONE         = 0x0,
	INST_IS_ATTACH       = 0x1,
	INST_IS_LAND         = 0x2,
	INST_IS_CONTROLLABLE = 0x4,
	INST_IS_EFFECT       = 0x8,
};
////////////////
// instance_stat
////////////////
////////////////
struct instance_stat
{
	instance_stat();
	const void *p_inst;
	phy_property phy;
	MODEL_TYPE type;
	int property;
	size_t index; // index for std::vector<instance_type>
	ex_texture_info ex_tex_info;
	XMFLOAT4X4 *get_World();
	XMFLOAT4X4 *get_RotFront();
	XMFLOAT4X4 *get_FinalTransform(size_t ix);
	std::string *get_ModelName();
	std::string *get_ClipName();
	PHY_BOUND_TYPE get_BoundType();
	void set_World(const XMFLOAT4X4 &world);
	void set_World(const XMMATRIX &world);
	void set_WorldPos(const XMFLOAT3 &pos);
	void set_IsInFrustum(const bool &is_in_frustum);
	void set_IsOffline(const bool &is_offline);
	void set_IsTransparent(const bool &is_transparent);
	void set_ClipName(const std::string &clip_name, const bool &is_reset_time);
	void check_set_ClipName(const std::string &clip_name, const bool &is_reset_time, const float &time_scale_in);
	void set_switch_ClipName(
		const std::string &clip_first,
		const std::string &clip_second,
		const size_t &last_frame);
	void set_switch_current_ClipName(const std::string &clip_second, const size_t &last_frame);
	void set_sequence_ClipName(const std::string &clip_second);
	void set_SwatchTex(const SWATCH_TEXTURE &tex_type_in, const float &duration, ID3D11ShaderResourceView* resource);
	bool is_alpha();
	bool is_invoke_physics();
	bool is_special_physics();
	bool is_in_switch_clip();
	bool is_in_frustum();
	bool get_IsInFrustum();
	bool get_IsOffline();
};
//
instance_stat::instance_stat():
	p_inst(nullptr),
	phy(),
	type(MODEL_BASIC),
	property(INST_IS_NONE),
	index(0),
	ex_tex_info()
{
	;
}
//
XMFLOAT4X4 *instance_stat::get_World()
{
	switch(type) {
	case MODEL_BASIC: return &(((basic_model_instance*)p_inst)->world);
	case MODEL_SKINNED: return &(((skinned_model_instance*)p_inst)->world);
	case MODEL_SIMPLE_P: return &(((simple_model_instance<vertex::pntt>*)p_inst)->world);
	}
	assert(false);
	return &(((basic_model_instance*)p_inst)->world);
}
//
XMFLOAT4X4 *instance_stat::get_RotFront()
{
	switch(type) {
	case MODEL_BASIC: return &(((basic_model_instance*)p_inst)->rot_front);
	case MODEL_SKINNED: return &(((skinned_model_instance*)p_inst)->rot_front);
	case MODEL_SIMPLE_P: return &(((simple_model_instance<vertex::pntt>*)p_inst)->rot_front);
	}
	assert(false);
	return &(((basic_model_instance*)p_inst)->rot_front);
}
//
XMFLOAT4X4 *instance_stat::get_FinalTransform(size_t ix)
{
	assert(type == MODEL_SKINNED);
	return &((skinned_model_instance*)p_inst)->final_transforms[ix];
}
//
std::string *instance_stat::get_ModelName()
{
	switch(type) {
	case MODEL_BASIC: return &(((basic_model_instance*)p_inst)->model_name);
	case MODEL_SKINNED: return &(((skinned_model_instance*)p_inst)->model_name);
	case MODEL_SIMPLE_P: return &(((simple_model_instance<vertex::pntt>*)p_inst)->model_name);
	}
	assert(false);
	return &(((basic_model_instance*)p_inst)->model_name);
}
//
std::string *instance_stat::get_ClipName()
{
	switch(type) {
	case MODEL_SKINNED: return &(((skinned_model_instance*)p_inst)->clip_name);
	}
	return &EMPTY_STRING;
}
//
PHY_BOUND_TYPE instance_stat::get_BoundType()
{
	int bound_t = 0;
	switch(type) {
	case MODEL_BASIC:
		bound_t = ((basic_model_instance*)p_inst)->model->m_BoundType;
		break;
	case MODEL_SKINNED:
		bound_t = ((skinned_model_instance*)p_inst)->model->m_BoundType;
		break;
	case MODEL_SIMPLE_P:
		bound_t = ((simple_model_instance<vertex::pntt>*)p_inst)->model->m_NameBoundType.at(
			((simple_model_instance<vertex::pntt>*)p_inst)->model_name);
		break;
	}
	assert(bound_t <= 2 && bound_t >= 0);
	return static_cast<PHY_BOUND_TYPE>(bound_t);
}
//
void instance_stat::set_World(const XMFLOAT4X4 &world)
{
	switch(type) {
	case MODEL_BASIC: ((basic_model_instance*)p_inst)->world = world; return;
	case MODEL_SKINNED: ((skinned_model_instance*)p_inst)->world = world; return;
	case MODEL_SIMPLE_P: ((simple_model_instance<vertex::pntt>*)p_inst)->world = world; return;
	}
	assert(false);
}
//
void instance_stat::set_World(const XMMATRIX &world)
{
	switch(type) {
	case MODEL_BASIC: XMStoreFloat4x4(&((basic_model_instance*)p_inst)->world, world); return;
	case MODEL_SKINNED: XMStoreFloat4x4(&((skinned_model_instance*)p_inst)->world, world); return;
	case MODEL_SIMPLE_P: XMStoreFloat4x4(&((simple_model_instance<vertex::pntt>*)p_inst)->world, world); return;
	}
	assert(false);
}
//
void instance_stat::set_WorldPos(const XMFLOAT3 &pos)
{
	switch(type) {
	case MODEL_BASIC:
		((basic_model_instance*)p_inst)->world._41 = pos.x;
		((basic_model_instance*)p_inst)->world._42 = pos.y;
		((basic_model_instance*)p_inst)->world._43 = pos.z;
		return;
	case MODEL_SKINNED:
		((skinned_model_instance*)p_inst)->world._41 = pos.x;
		((skinned_model_instance*)p_inst)->world._42 = pos.y;
		((skinned_model_instance*)p_inst)->world._43 = pos.z;
		return;
	case MODEL_SIMPLE_P:
		((simple_model_instance<vertex::pntt>*)p_inst)->world._41 = pos.x;
		((simple_model_instance<vertex::pntt>*)p_inst)->world._42 = pos.y;
		((simple_model_instance<vertex::pntt>*)p_inst)->world._43 = pos.z;
		return;
	}
	assert(false);
}
//
void instance_stat::set_IsInFrustum(const bool &is_in_frustum)
{
	switch(type) {
	case MODEL_BASIC: ((basic_model_instance*)p_inst)->is_in_frustum = is_in_frustum; return;
	case MODEL_SKINNED: ((skinned_model_instance*)p_inst)->is_in_frustum = is_in_frustum; return;
	case MODEL_SIMPLE_P: ((simple_model_instance<vertex::pntt>*)p_inst)->is_in_frustum = is_in_frustum; return;
	}
	assert(false);
}
//
void instance_stat::set_IsOffline(const bool &is_offline)
{
	switch(type) {
	case MODEL_BASIC: ((basic_model_instance*)p_inst)->is_offline = is_offline; return;
	case MODEL_SKINNED: ((skinned_model_instance*)p_inst)->is_offline = is_offline; return;
	case MODEL_SIMPLE_P: ((simple_model_instance<vertex::pntt>*)p_inst)->is_offline = is_offline; return;
	}
	assert(false);
}
//
void instance_stat::set_IsTransparent(const bool &is_transparent)
{
	switch(type) {
	case MODEL_BASIC: ((basic_model_instance*)p_inst)->is_transparent = is_transparent; return;
	case MODEL_SKINNED: ((skinned_model_instance*)p_inst)->is_transparent = is_transparent; return;
	case MODEL_SIMPLE_P: ((simple_model_instance<vertex::pntt>*)p_inst)->is_transparent = is_transparent; return;
	}
	assert(false);
}
//
void instance_stat::set_ClipName(const std::string &clip_name, const bool &is_reset_time = false)
{
	if (type != MODEL_SKINNED) return;
	((skinned_model_instance*)p_inst)->set_ClipName(clip_name, is_reset_time);
}
//
void instance_stat::check_set_ClipName(const std::string &clip_name, const bool &is_reset_time = false, const float &time_scale_in = 1.0f)
{
	if (type != MODEL_SKINNED) return;
	((skinned_model_instance*)p_inst)->check_set_ClipName(clip_name, is_reset_time, time_scale_in);
}
//
void instance_stat::set_switch_ClipName(
	const std::string &clip_first,
	const std::string &clip_second,
	const size_t &last_frame)
{
	if (type != MODEL_SKINNED) return;
	((skinned_model_instance*)p_inst)->set_switch_ClipName(clip_first, clip_second, last_frame);
}
//
void instance_stat::set_switch_current_ClipName(const std::string &clip_second,	const size_t &last_frame)
{
	if (type != MODEL_SKINNED) return;
	((skinned_model_instance*)p_inst)->set_switch_ClipName(*get_ClipName(), clip_second, last_frame);
}
//
void instance_stat::set_sequence_ClipName(const std::string &clip_second)
{
	if (type != MODEL_SKINNED) return;
	((skinned_model_instance*)p_inst)->set_sequence_ClipName(clip_second);
}
//
void instance_stat::set_SwatchTex(const SWATCH_TEXTURE &tex_type_in, const float &duration, ID3D11ShaderResourceView* resource)
{
	ex_tex_info.resource = resource;
	ex_tex_info.swatch = tex_type_in;
	ex_tex_info.duration = duration;
	ex_tex_info.is_twinkle = false;
	ex_tex_info.is_change_tex = true;
	if (tex_type_in == SWATCH_TEX_TWINKLE) {
		ex_tex_info.is_twinkle = true;
		ex_tex_info.is_change_tex = false;
	}
}
//
bool instance_stat::is_alpha()
{
	switch(type) {
	case MODEL_BASIC: return ((basic_model_instance*)p_inst)->model->m_IsAlpha;
	case MODEL_SKINNED: return ((skinned_model_instance*)p_inst)->model->m_IsAlpha;
	case MODEL_SIMPLE_P: return ((simple_model_instance<vertex::pntt>*)p_inst)->model->m_IsAlpha;
	}
	assert(false);
	return false;
}
//
bool instance_stat::is_invoke_physics()
{
	if (property & INST_IS_ATTACH) return false;
	if (property & INST_IS_EFFECT) return false;
	if (get_IsOffline()) return false;
	return true;
}
//
bool instance_stat::is_special_physics()
{
	if (property & INST_IS_LAND) return true;
	if (phy.intera_tp & PHY_INTERA_FIXED_LIMIT) return true;
	return false;
}
//
bool instance_stat::is_in_switch_clip()
{
	if (type != MODEL_SKINNED) return false;
	if (((skinned_model_instance*)p_inst)->time_switch > 0.0f) return true;
	return false;
}
//
bool instance_stat::is_in_frustum()
{
	switch(type) {
	case MODEL_BASIC: return ((basic_model_instance*)p_inst)->is_in_frustum;
	case MODEL_SKINNED: return ((skinned_model_instance*)p_inst)->is_in_frustum;
	case MODEL_SIMPLE_P: return ((simple_model_instance<vertex::pntt>*)p_inst)->is_in_frustum;
	}
	assert(false);
	return false;
}
//
bool instance_stat::get_IsInFrustum()
{
	switch(type) {
	case MODEL_BASIC: return ((basic_model_instance*)p_inst)->is_in_frustum;
	case MODEL_SKINNED: return ((skinned_model_instance*)p_inst)->is_in_frustum;
	case MODEL_SIMPLE_P: return ((simple_model_instance<vertex::pntt>*)p_inst)->is_in_frustum;
	}
	assert(false);
	return false;
}
//
bool instance_stat::get_IsOffline()
{
	switch(type) {
	case MODEL_BASIC: return ((basic_model_instance*)p_inst)->is_offline;
	case MODEL_SKINNED: return ((skinned_model_instance*)p_inst)->is_offline;
	case MODEL_SIMPLE_P: return ((simple_model_instance<vertex::pntt>*)p_inst)->is_offline;
	}
	assert(false);
	return false;
}
////////////////
// model_mgr
////////////////
////////////////
struct model_mgr
{
	model_mgr();
	void init(ID3D11Device *device);
	bool load(ID3D11Device *device, const std::string& scene_ix);
	void pntt_init(ID3D11Device *device, lua_reader &l_reader);
	void skinned_init(ID3D11Device *device, lua_reader &l_reader);
	void basic_init(ID3D11Device *device, lua_reader &l_reader);
	void model_mgr::copy_instance(
		std::vector<instance_stat> &stat,
		const size_t &ix,
		const std::string &new_name);
	void remove_all();
	texture_mgr m_TexMgr;
	bool m_IsInitialized;
	std::map<std::string, simple_model<vertex::pntt>> m_PNTT;
	std::map<std::string, skinned_model> m_Skinned;
	std::map<std::string, basic_model> m_Basic;
	std::vector<simple_model_instance<vertex::pntt>> m_InstPNTT;
	std::vector<skinned_model_instance> m_InstSkinnedAlpha;
	std::vector<skinned_model_instance> m_InstSkinned;
	std::vector<basic_model_instance> m_InstBasicAlpha;
	std::vector<basic_model_instance> m_InstBasic;
	std::map<size_t, std::string> m_NamePNTT;
	std::map<size_t, std::string> m_NameSkinnedAlpha;
	std::map<size_t, std::string> m_NameSkinned;
	std::map<size_t, std::string> m_NameBasicAlpha;
	std::map<size_t, std::string> m_NameBasic;
};
model_mgr::model_mgr():
	m_TexMgr(),
	m_IsInitialized(false)
{
	;
}
//
void model_mgr::init(ID3D11Device *device)
{
	if (!m_IsInitialized) {
		m_TexMgr.init(device);
		m_IsInitialized = true;
	}
}
//
bool model_mgr::load(ID3D11Device *device, const std::string& scene_ix)
{
	init(device);
	std::string concrete = IMM_PATH["script"]+"scene"+scene_ix+"\\concrete_instance.lua";
	if (!data_is_file_exist(concrete)) return false;
	remove_all();
	lua_reader l_reader;
	l_reader.loadfile(concrete);
	std::thread(&model_mgr::pntt_init, this, device, std::ref(l_reader)).join();
	std::thread(&model_mgr::skinned_init, this, device, std::ref(l_reader)).join();
	std::thread(&model_mgr::basic_init, this, device, std::ref(l_reader)).join();
	return true;
}
//
//
void model_mgr::pntt_init(ID3D11Device *device, lua_reader &l_reader)
{
	std::vector<std::vector<std::string>> vec2d_model;
	std::vector<std::vector<std::string>> vec2d_instance;
	std::map<std::string, rotation_xyz> model_rot_front;
	std::map<std::string, bool> model_is_tex;
	std::map<std::string, std::string> model_tex_trans;
	// build model
	l_reader.vec2d_str_from_table("csv_model_geometry", vec2d_model);
	std::vector<geometry::mesh_data> geo_mesh;
	geometry geo_gen;
	m_PNTT["default"];
	std::wstring path_tex(txtutil::str_to_wstr(IMM_PATH["texture"]));
	for (size_t ix = 1; ix < vec2d_model.size(); ++ix) {
		std::string model_name = vec2d_model[ix][0];
		m_PNTT["default"].m_NameSubid[model_name] = static_cast<UINT>(ix-1);
		geo_mesh.push_back(geometry::mesh_data());
		std::vector<float> para;
		if (vec2d_model[ix][1] == "box") {
			para = csv_string_to_float(vec2d_model[ix][2], 3, "#");
			geo_gen.create_box(para[0], para[1], para[2], geo_mesh.back());
		}
		else if (vec2d_model[ix][1] == "grid") {
			para = csv_string_to_float(vec2d_model[ix][2], 4, "#");
			geo_gen.create_grid(para[0], para[1],
				static_cast<int>(para[2]), static_cast<int>(para[3]), geo_mesh.back());
		}
		else if (vec2d_model[ix][1] == "sphere") {
			para = csv_string_to_float(vec2d_model[ix][2], 3, "#");
			geo_gen.create_sphere(para[0],
				static_cast<int>(para[1]), static_cast<int>(para[2]), geo_mesh.back());
		}
		else if (vec2d_model[ix][1] == "cylinder") {
			para = csv_string_to_float(vec2d_model[ix][2], 5, "#");
			geo_gen.create_cylinder(para[0], para[1], para[2],
				static_cast<int>(para[3]), static_cast<int>(para[4]), geo_mesh.back());
		}
		else ERROR_MESA("model_geometry.csv load type error.");
		//
		m_PNTT["default"].m_Mat.push_back(material());
		para = csv_string_to_float(vec2d_model[ix][4], 2, "#");
		model_material_swatch(
			m_PNTT["default"].m_Mat.back(), para[0], static_cast<int>(para[1]));
		model_rot_front[model_name] = rotation_xyz(vec2d_model[ix][3]);
		std::wstring tex_subpath = txtutil::str_to_wstr(vec2d_model[ix][5]);
		std::wstring tex_diffuse(vec2d_model[ix][6].begin(), vec2d_model[ix][6].end());
		std::wstring tex_location = path_tex + tex_subpath;
		std::wstring tex_normal(vec2d_model[ix][7].begin(), vec2d_model[ix][7].end());
		model_is_tex[model_name] = !csv_value_is_empty(vec2d_model[ix][6]);
		m_PNTT["default"].set_MapSRV(m_TexMgr, tex_location, tex_diffuse, tex_normal, model_is_tex[model_name]);
		model_tex_trans[model_name] = vec2d_model[ix][8];
		m_PNTT["default"].m_NameBoundType[model_name] = phy_bound_type_str(vec2d_model[ix][9]);
		m_PNTT["default"].m_NameInteractiveType[model_name] = phy_interactive_type_str(vec2d_model[ix][10]);
	}
	model_load_geo_mesh(device, m_PNTT["default"], geo_mesh);
	// build instance
	l_reader.vec2d_str_from_table("csv_instance_geometry", vec2d_instance);
	for (size_t ix = 1; ix < vec2d_instance.size(); ++ix) {
		std::string model_name = vec2d_instance[ix][1];
		m_NamePNTT[m_InstPNTT.size()] = vec2d_instance[ix][0];
		assert(m_PNTT["default"].m_NameSubid.count(model_name));
		m_InstPNTT.push_back(simple_model_instance<vertex::pntt>());
		std::vector<simple_model_instance<vertex::pntt>>::iterator it;
		it = m_InstPNTT.end();
		// iterator to the last instance
		--it;
		it->model = &m_PNTT["default"];
		it->model_name = model_name;
		it->subid = m_PNTT["default"].m_NameSubid[model_name];
		instance_assign_csv_basic(
			it,
			ix,
			model_name,
			model_rot_front,
			vec2d_instance);
		it->is_textrue = model_is_tex[model_name];
		if (model_tex_trans[model_name].size() > 4) {
			std::vector<float> para = csv_string_to_float(model_tex_trans[model_name], 3, "#");
			XMStoreFloat4x4(&it->tex_transform, XMMatrixScaling(para[0], para[1], para[2]));
		}
	}
}
//
void model_mgr::skinned_init(ID3D11Device *device, lua_reader &l_reader)
{
	std::vector<std::vector<std::string>> vec2d_model;
	std::vector<std::vector<std::string>> vec2d_instance;
	std::map<std::string, rotation_xyz> model_rot_front;
	// build model
	model_load_csv_basic(
		device,
		m_Skinned,
		vec2d_model,
		model_rot_front,
		l_reader,
		m_TexMgr,
		"csv_model_skinned",
		IMM_PATH["model"],
		txtutil::str_to_wstr(IMM_PATH["texture"])
	);
	// build instance
	l_reader.vec2d_str_from_table("csv_instance_skinned", vec2d_instance);
	for (size_t ix = 1; ix < vec2d_instance.size(); ++ix) {
		std::string model_name = vec2d_instance[ix][1];
		assert(m_Skinned.count(model_name));
		std::vector<skinned_model_instance>::iterator it;
		if (m_Skinned[model_name].m_IsAlpha) {
			m_NameSkinnedAlpha[m_InstSkinnedAlpha.size()] = vec2d_instance[ix][0];
			m_InstSkinnedAlpha.push_back(skinned_model_instance());
			it = m_InstSkinnedAlpha.end();
		}
		else {
			m_NameSkinned[m_InstSkinned.size()] = vec2d_instance[ix][0];
			m_InstSkinned.push_back(skinned_model_instance());
			it = m_InstSkinned.end();
		}
		// iterator to the last instance
		--it;
		it->model = &m_Skinned[model_name];
		it->model_name = model_name;
		instance_assign_csv_basic(
			it,
			ix,
			model_name,
			model_rot_front,
			vec2d_instance);
		//
		it->time_pos = stof(vec2d_instance[ix][7]);
		it->set_ClipName(vec2d_instance[ix][8]);
		it->final_transforms.resize(m_Skinned[model_name].m_SkinnedData.m_BoneCount());
		// avoid all zero matrix
		for (auto &final: it->final_transforms) {
			XMStoreFloat4x4(&final, XMMatrixIdentity());
		}
	}
}
//
void model_mgr::basic_init(ID3D11Device *device, lua_reader &l_reader)
{
	std::vector<std::vector<std::string>> vec2d_model;
	std::vector<std::vector<std::string>> vec2d_instance;
	std::vector<std::vector<std::string>> vec2d_instance_prep;
	std::map<std::string, rotation_xyz> model_rot_front;
	// build model
	model_load_csv_basic(
		device,
		m_Basic,
		vec2d_model,
		model_rot_front,
		l_reader,
		m_TexMgr,
		"csv_model_basic",
		IMM_PATH["model"],
		txtutil::str_to_wstr(IMM_PATH["texture"])
	);
	// build instance
	l_reader.vec2d_str_from_table("csv_instance_basic", vec2d_instance);
	//
	if (!l_reader.is_nil("csv_prep_instance_basic")) {
		assert(!l_reader.is_nil("csv_prep_model_basic"));
		model_load_csv_basic(
			device,
			m_Basic,
			vec2d_model,
			model_rot_front,
			l_reader,
			m_TexMgr,
			"csv_prep_model_basic",
			IMM_PATH["model"],
			txtutil::str_to_wstr(IMM_PATH["texture"])
		);
		// build instance
		l_reader.vec2d_str_from_table("csv_prep_instance_basic", vec2d_instance_prep);
		vec2d_instance.insert(vec2d_instance.end(), vec2d_instance_prep.begin()+1, vec2d_instance_prep.end());
	}
	//
	for (size_t ix = 1; ix < vec2d_instance.size(); ++ix) {
		std::string model_name = vec2d_instance[ix][1];
		assert(m_Basic.count(model_name));
		std::vector<basic_model_instance>::iterator it;
		if (m_Basic[model_name].m_IsAlpha) {
			m_NameBasicAlpha[m_InstBasicAlpha.size()] = vec2d_instance[ix][0];
			m_InstBasicAlpha.push_back(basic_model_instance());
			it = m_InstBasicAlpha.end();
		}
		else {
			m_NameBasic[m_InstBasic.size()] = vec2d_instance[ix][0];
			m_InstBasic.push_back(basic_model_instance());
			it = m_InstBasic.end();
		}
		// iterator to the last instance
		--it;
		it->model = &m_Basic[model_name];
		it->model_name = model_name;
		instance_assign_csv_basic(
			it,
			ix,
			model_name,
			model_rot_front,
			vec2d_instance
		);
	}
}
//
void model_mgr::copy_instance(
	std::vector<instance_stat> &stat,
	const size_t &ix,
	const std::string &new_name)
{
	switch(stat[ix].type) {
	case MODEL_BASIC:
		stat.emplace_back(stat[ix]);
		if (stat[ix].is_alpha()) {
			m_NameBasicAlpha[m_InstBasicAlpha.size()] = new_name;
			m_InstBasicAlpha.push_back(m_InstBasicAlpha.at(stat[ix].index));
			stat.back().p_inst = &m_InstBasicAlpha.back();
			stat.back().index = m_InstBasicAlpha.size()-1;
			m_InstBasicAlpha.back().stat_ix = stat.size()-1;
		}
		else {
			m_NameBasic[m_InstBasic.size()] = new_name;
			m_InstBasic.push_back(m_InstBasic.at(stat[ix].index));
			stat.back().p_inst = &m_InstBasic.back();
			stat.back().index = m_InstBasic.size()-1;
			m_InstBasic.back().stat_ix = stat.size()-1;
		}
		break;
	case MODEL_SKINNED:
		assert(false);
		return;
		break;
	case MODEL_SIMPLE_P:
		stat.emplace_back(stat[ix]);
		m_NamePNTT[m_InstPNTT.size()] = new_name;
		m_InstPNTT.push_back(m_InstPNTT.at(stat[ix].index));
		stat.back().p_inst = &m_InstPNTT.back();
		stat.back().index = m_InstPNTT.size()-1;
		m_InstPNTT.back().stat_ix = stat.size()-1;
		return;
		break;
	}
}
//
void model_mgr::remove_all()
{
	m_PNTT.clear();
	// continue to have m_Skinned m_Basic model
	m_InstPNTT.clear();
	m_InstSkinnedAlpha.clear();
	m_InstSkinned.clear();
	m_InstBasicAlpha.clear();
	m_InstBasic.clear();
	m_NamePNTT.clear();
	m_NameSkinnedAlpha.clear();
	m_NameSkinned.clear();
	m_NameBasicAlpha.clear();
	m_NameBasic.clear();
	m_InstPNTT.reserve(VECTOR_RESERVE);
	m_InstSkinnedAlpha.reserve(VECTOR_RESERVE);
	m_InstSkinned.reserve(VECTOR_RESERVE);
	m_InstBasicAlpha.reserve(VECTOR_RESERVE);
	m_InstBasic.reserve(VECTOR_RESERVE);
}
//
}
#endif