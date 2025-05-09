////////////////
// stru_inst_adapter.h
// by Huang Yiting
////////////////
////////////////
#ifndef STRU_INST_ADAPTER_H
#define STRU_INST_ADAPTER_H
#include "stru_model_mgr.h"
namespace imm
{
////////////////
// inst_attachment
////////////////
////////////////
struct inst_attachment
{
	inst_attachment();
	std::string name;
	std::string owner_name;
	size_t ix;
	size_t owner_ix;
	XMFLOAT4X4 to_bone;
	size_t bone_ix;
	bool is_enable;
};
//
inst_attachment::inst_attachment():
	ix(0),
	owner_ix(0),
	bone_ix(0),
	is_enable(false)
{
	XMStoreFloat4x4(&to_bone, XMMatrixIdentity());
}
////////////////
// inst_solid_effect
////////////////
////////////////
struct inst_solid_effect
{
	inst_solid_effect();
	size_t inst_ix;
	size_t caster;
	bool is_loaded;
	bool is_active;
	float time_pos;
	bone_animation animation;
};
//
inst_solid_effect::inst_solid_effect():
	inst_ix(0),
	caster(0),
	is_loaded(false),
	is_active(false),
	time_pos(0.0f),
	animation()
{
	;
}
////////////////
// inst_adapter
////////////////
////////////////
template <typename T_app>
struct inst_adapter
{
	inst_adapter();
	void init_load(T_app *app_in);
	void read_lua();
	void rebuild();
	void attach_flush();
	void effect_rebuild();
	void update_world();
	void update_attach();
	void update_magic_text();
	void call_magic_text(const size_t caster, const float &duration);
	T_app *app;
	std::vector<inst_attachment> attach;
	std::vector<inst_solid_effect> magic_text;
	std::string magic_text_name;
};
//
template <typename T_app>
inst_adapter<T_app>::inst_adapter():
	app(nullptr),
	attach(),
	magic_text(),
	magic_text_name("magic_text")
{
	;
}
//
template <typename T_app>
void inst_adapter<T_app>::init_load(T_app *app_in)
{
	app = app_in;
	read_lua();
}
//
template <typename T_app>
void inst_adapter<T_app>::read_lua()
{
	std::string concrete = IMM_PATH["script"]+"concrete_common.lua";
	lua_reader l_reader;
	l_reader.loadfile(concrete);
	std::vector<std::vector<std::string>> vec2d;
	l_reader.vec2d_str_from_table("csv_weapon_map", vec2d);
	for (size_t ix = 1; ix < vec2d.size(); ++ix) {
		attach.emplace_back();
		attach.back().name = vec2d[ix][0];
		attach.back().owner_name = vec2d[ix][1];
		attach.back().bone_ix = stoi(vec2d[ix][2]);
		XMMATRIX offset = XMMatrixTranslation(
			stof(vec2d[ix][3]),
			stof(vec2d[ix][4]),
			stof(vec2d[ix][5]));
		XMMATRIX rot = rotation_xyz(vec2d[ix][6]).get_Matrix();
		XMMATRIX to_bone = XMMatrixMultiply(rot, offset);
		XMStoreFloat4x4(&attach.back().to_bone, to_bone);
	}
}
//
template <typename T_app>
void inst_adapter<T_app>::rebuild()
{
	effect_rebuild();
}
//
template <typename T_app>
void inst_adapter<T_app>::effect_rebuild()
{
	magic_text.clear();
	magic_text.resize(1);
	magic_text[0].is_loaded = (app->m_Inst.m_NameMap.count(magic_text_name) != 0);
	if (magic_text[0].is_loaded) {
		// test
		app->m_Inst.copy_instance(magic_text_name, magic_text_name+"1");
		app->m_Inst.m_Stat[app->m_Inst.m_NameMap[magic_text_name+"1"]].property = INST_IS_EFFECT;
		app->m_Inst.m_Stat[app->m_Inst.m_NameMap[magic_text_name+"1"]].set_IsOffline(true);
		//
		magic_text[0].inst_ix = app->m_Inst.m_NameMap.at(magic_text_name);
		app->m_Inst.m_Stat[magic_text[0].inst_ix].property = INST_IS_EFFECT;
		app->m_Inst.m_Stat[magic_text[0].inst_ix].set_IsOffline(true);
		//
		XMVECTOR q0 = XMQuaternionRotationAxis(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), XMConvertToRadians(30.0f));
		XMVECTOR q1 = XMQuaternionRotationAxis(XMVectorSet(0.5f, 1.0f, 2.0f, 0.0f), XMConvertToRadians(45.0f));
		XMVECTOR q2 = XMQuaternionRotationAxis(XMVectorSet(0.3f, 1.0f, 4.0f, 0.0f), XMConvertToRadians(-30.0f));
		XMVECTOR q3 = XMQuaternionRotationAxis(XMVectorSet(0.0f, 1.0f, 6.0f, 0.0f), XMConvertToRadians(70.0f));
		XMMATRIX mrf = XMLoadFloat4x4(app->m_Inst.m_Stat[magic_text[0].inst_ix].get_RotFront());
		XMVECTOR qrf = XMQuaternionRotationMatrix(mrf);
		q0 = XMQuaternionMultiply(q0, qrf);
		q1 = XMQuaternionMultiply(q1, qrf);
		q2 = XMQuaternionMultiply(q2, qrf);
		q3 = XMQuaternionMultiply(q3, qrf);
		magic_text[0].animation.keyframes.resize(5);
		//
		magic_text[0].animation.keyframes[0].scale = XMFLOAT3(1.5f, 1.5f, 1.5f);
		XMStoreFloat4(&magic_text[0].animation.keyframes[0].rotation_quat, q0);
		magic_text[0].animation.keyframes[1].scale = XMFLOAT3(1.5f, 1.5f, 1.5f);
		XMStoreFloat4(&magic_text[0].animation.keyframes[1].rotation_quat, q1);
		magic_text[0].animation.keyframes[2].scale = XMFLOAT3(1.5f, 1.5f, 1.5f);
		XMStoreFloat4(&magic_text[0].animation.keyframes[2].rotation_quat, q2);
		magic_text[0].animation.keyframes[3].scale = XMFLOAT3(1.2f, 1.2f, 1.2f);
		XMStoreFloat4(&magic_text[0].animation.keyframes[3].rotation_quat, q3);
		magic_text[0].animation.keyframes[4].scale = XMFLOAT3(0.1f, 0.1f, 0.1f);
		XMStoreFloat4(&magic_text[0].animation.keyframes[4].rotation_quat, q0);
	}
}
//
template <typename T_app>
void inst_adapter<T_app>::attach_flush()
{
	for (auto &att: attach) {
		if (app->m_Inst.m_NameMap.count(att.name) && app->m_Inst.m_NameMap.count(att.owner_name)) {
			att.is_enable = true;
			att.ix = app->m_Inst.m_NameMap[att.name];
			att.owner_ix = app->m_Inst.m_NameMap[att.owner_name];
			app->m_Inst.m_Stat[att.ix].property |= INST_IS_ATTACH;
		}
		else {
			att.is_enable = false;
		}
	}
}
//
template <typename T_app>
void inst_adapter<T_app>::update_world()
{
	update_attach();
	update_magic_text();
}
//
template <typename T_app>
void inst_adapter<T_app>::update_attach()
{
	for (auto &att: attach) {
		if (!att.is_enable) continue;
		if (app->m_Inst.m_Stat[att.ix].get_IsOffline()) continue;
		app->m_Inst.m_Stat[att.ix].set_IsOffline(app->m_Inst.m_Stat[att.owner_ix].get_IsOffline());
		XMMATRIX world = XMLoadFloat4x4(app->m_Inst.m_Stat[att.owner_ix].get_World());
		XMMATRIX to_bone = XMLoadFloat4x4(&att.to_bone);
		XMMATRIX trans_bone = XMLoadFloat4x4(
			app->m_Inst.m_Stat[att.owner_ix].get_FinalTransform(att.bone_ix)
		);
		XMMATRIX att_world = XMMatrixMultiply(to_bone, trans_bone);
		att_world = XMMatrixMultiply(att_world, world);
		app->m_Inst.m_Stat[att.ix].set_World(att_world);
	}
}
//
template <typename T_app>
void inst_adapter<T_app>::update_magic_text()
{
	if (!magic_text[0].is_active) return;
	if(magic_text[0].time_pos >= magic_text[0].animation.get_end_time()) {
		magic_text[0].is_active = false;
		app->m_Inst.m_Stat[magic_text[0].inst_ix].set_IsOffline(true);
		return;
	}
	magic_text[0].time_pos += app->m_Timer.delta_time();
	magic_text[0].animation.interpolate(magic_text[0].time_pos, *app->m_Inst.m_Stat[magic_text[0].inst_ix].get_World());
	app->m_Inst.m_Stat[magic_text[0].inst_ix].set_WorldPos(
		app->m_Inst.m_BoundW.center(magic_text[0].caster)
	);
}
//
template <typename T_app>
void inst_adapter<T_app>::call_magic_text(const size_t caster, const float &duration)
{
	if (!magic_text[0].is_loaded) return;
	app->m_Inst.m_Stat[magic_text[0].inst_ix].set_IsOffline(false);
	magic_text[0].is_active = true;
	magic_text[0].time_pos = 0.0f;
	magic_text[0].caster = caster;
	assert(magic_text[0].animation.keyframes.size() > 4);
	magic_text[0].animation.keyframes[0].time_pos = 0.0f;
	magic_text[0].animation.keyframes[1].time_pos = duration/4.0f;
	magic_text[0].animation.keyframes[2].time_pos = duration/4.0f*2.0f;
	magic_text[0].animation.keyframes[3].time_pos = duration/4.0f*3.0f;
	magic_text[0].animation.keyframes[4].time_pos = duration;
}
//
}
#endif