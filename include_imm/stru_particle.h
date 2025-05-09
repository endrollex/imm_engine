////////////////
// stru_particle.h
// by Huang Yiting
////////////////
////////////////
#ifndef STRU_PARTICLE_H
#define STRU_PARTICLE_H
#include "render_particle.h"
#include "mesh_d3d_util.h"
#include "stru_lua_help.h"
#include <list>
namespace imm
{
////////////////
// PLASMA_TYPE
////////////////
////////////////
enum PLASMA_TYPE
{
	PLASMA_FIRE,
	PLASMA_STRIKE,
	PLASMA_STRIKE2,
	PLASMA_CHARGE,
	PLASMA_LIGHTNING,
	PLASMA_BROKEN,
	PLASMA_BLOCK,
	PLASMA_MISSILE1,
	PLASMA_MISSILE2,
	PLASMA_MISSILE3,
	PLASMA_MISSILE4,
};
////////////////
// plasma_pile
////////////////
////////////////
struct plasma_pile
{
	plasma_pile();
	~plasma_pile();
	void clear();
	particle pt;
	std::list<inst_plasam> list_inst;
	size_t index;
	ID3D11ShaderResourceView *tex;
};
//
plasma_pile::plasma_pile():
	index(0),
	tex(nullptr)
{
	;
}
//
plasma_pile::~plasma_pile()
{
	RELEASE_COM(tex);
}
//
void plasma_pile::clear()
{
	list_inst.clear();
}
////////////////
// state_plasma
////////////////
////////////////
struct state_plasma
{
	state_plasma();
	~state_plasma();
	void init_load(ID3D11Device *device, ID3D11DeviceContext *context);
	void update(const float &dt, const float &total_time);
	void draw(ID3D11DeviceContext *context, const camera &cam);
	void remove_all();
	void push_back(PLASMA_TYPE type, const float &count_down, const XMFLOAT3 &pos, bool is_ptr = false);
	std::map<PLASMA_TYPE, plasma_pile> pile;
	bool is_active;
	ID3D11ShaderResourceView *tex_random_srv;
private:
	state_plasma(const state_plasma &rhs);
	state_plasma &operator=(const state_plasma &rhs);
};
//
state_plasma::state_plasma():
	is_active(false),
	tex_random_srv(nullptr)
{
	;
}
//
state_plasma::~state_plasma()
{
	RELEASE_COM(tex_random_srv);
}
//
void state_plasma::init_load(ID3D11Device *device, ID3D11DeviceContext *context)
{
	std::string concrete = IMM_PATH["script"]+"scene_common\\state_plasma.lua";
	std::map<std::string, std::string> get_dds;
	get_dds["plasma_fire_dds"] = "";
	get_dds["plasma_strike_dds"] = "";
	get_dds["plasma_strike2_dds"] = "";
	get_dds["plasma_charge_dds"] = "";
	get_dds["plasma_lightning_dds"] = "";
	get_dds["plasma_broken_dds"] = "";
	get_dds["plasma_block_dds"] = "";
	get_dds["plasma_missile1_dds"] = "";
	lua_reader l_reader;
	l_reader.loadfile(concrete);
	l_reader.map_from_string(get_dds);
	is_active = true;
	for (auto &get: get_dds) {
		if (csv_value_is_empty(get.second)) {
			is_active = false;
			return;
		}
	}
	std::wstring path_tex = txtutil::str_to_wstr(IMM_PATH["texture"]+"particle\\");
	tex_random_srv = create_RandomTexture1DSRV(device);
	std::vector<std::wstring> file_names;
	// Fire
	file_names.push_back(path_tex+txtutil::str_to_wstr(get_dds["plasma_fire_dds"]));
	pile[PLASMA_FIRE].tex = create_Texture2DArraySRV(device, context, file_names);
	pile[PLASMA_FIRE].pt.init(device, effects::m_PtFireFX, pile[PLASMA_FIRE].tex, tex_random_srv, 512, 16);
	// Strike
	file_names.clear();
	file_names.push_back(path_tex+txtutil::str_to_wstr(get_dds["plasma_strike_dds"]));
	pile[PLASMA_STRIKE].tex = create_Texture2DArraySRV(device, context, file_names);
	pile[PLASMA_STRIKE].pt.init(device, effects::m_PtStrikeFX, pile[PLASMA_STRIKE].tex, tex_random_srv, 512, 16);
	// Strike2
	file_names.clear();
	file_names.push_back(path_tex+txtutil::str_to_wstr(get_dds["plasma_strike2_dds"]));
	pile[PLASMA_STRIKE2].tex = create_Texture2DArraySRV(device, context, file_names);
	pile[PLASMA_STRIKE2].pt.init(device, effects::m_PtStrike2FX, pile[PLASMA_STRIKE2].tex, tex_random_srv, 512, 16);
	// Charge
	file_names.clear();
	file_names.push_back(path_tex+txtutil::str_to_wstr(get_dds["plasma_charge_dds"]));
	pile[PLASMA_CHARGE].tex = create_Texture2DArraySRV(device, context, file_names);
	pile[PLASMA_CHARGE].pt.init(device, effects::m_PtChargeFX, pile[PLASMA_CHARGE].tex, tex_random_srv, 512, 16);
	// Lightning
	file_names.clear();
	file_names.push_back(path_tex+txtutil::str_to_wstr(get_dds["plasma_lightning_dds"]));
	pile[PLASMA_LIGHTNING].tex = create_Texture2DArraySRV(device, context, file_names);
	pile[PLASMA_LIGHTNING].pt.init(device, effects::m_PtLightningFX, pile[PLASMA_LIGHTNING].tex, tex_random_srv, 1, 16);
	// Broken
	file_names.clear();
	file_names.push_back(path_tex+txtutil::str_to_wstr(get_dds["plasma_broken_dds"]));
	pile[PLASMA_BROKEN].tex = create_Texture2DArraySRV(device, context, file_names);
	pile[PLASMA_BROKEN].pt.init(device, effects::m_PtBrokenFX, pile[PLASMA_BROKEN].tex, tex_random_srv, 32, 16);
	// Block
	file_names.clear();
	file_names.push_back(path_tex+txtutil::str_to_wstr(get_dds["plasma_block_dds"]));
	pile[PLASMA_BLOCK].tex = create_Texture2DArraySRV(device, context, file_names);
	pile[PLASMA_BLOCK].pt.init(device, effects::m_PtBlockFX, pile[PLASMA_BLOCK].tex, tex_random_srv, 32, 16);
	// Missile1
	file_names.clear();
	file_names.push_back(path_tex+txtutil::str_to_wstr(get_dds["plasma_missile1_dds"]));
	pile[PLASMA_MISSILE1].tex = create_Texture2DArraySRV(device, context, file_names);
	pile[PLASMA_MISSILE1].pt.init(device, effects::m_PtMissile1, pile[PLASMA_MISSILE1].tex, tex_random_srv, 1, 32, 0);
	// Missile2
	file_names.clear();
	file_names.push_back(path_tex+txtutil::str_to_wstr(get_dds["plasma_missile1_dds"]));
	pile[PLASMA_MISSILE2].tex = create_Texture2DArraySRV(device, context, file_names);
	pile[PLASMA_MISSILE2].pt.init(device, effects::m_PtMissile1, pile[PLASMA_MISSILE2].tex, tex_random_srv, 1, 32, 1);
	// Missile3
	file_names.clear();
	file_names.push_back(path_tex+txtutil::str_to_wstr(get_dds["plasma_missile1_dds"]));
	pile[PLASMA_MISSILE3].tex = create_Texture2DArraySRV(device, context, file_names);
	pile[PLASMA_MISSILE3].pt.init(device, effects::m_PtMissile1, pile[PLASMA_MISSILE3].tex, tex_random_srv, 1, 32, 2);
	// Missile4
	file_names.clear();
	file_names.push_back(path_tex+txtutil::str_to_wstr(get_dds["plasma_missile1_dds"]));
	pile[PLASMA_MISSILE4].tex = create_Texture2DArraySRV(device, context, file_names);
	pile[PLASMA_MISSILE4].pt.init(device, effects::m_PtMissile1, pile[PLASMA_MISSILE4].tex, tex_random_srv, 1, 32, 3);}
//
void state_plasma::update(const float &dt, const float &total_time)
{
	if (!is_active) return;
	auto is_should_del =
		[](const inst_plasam &pla) {return (pla.count_down > -99.0f && pla.count_down < 0.0f);};
	for (auto &pi: pile) {
		pi.second.pt.update(dt, total_time);
		for (auto &inst: pi.second.list_inst) inst.update(dt);
		pi.second.list_inst.remove_if(is_should_del);
	}
}
//
void state_plasma::draw(ID3D11DeviceContext *context, const camera &cam)
{
	if (!is_active) return;
	for (auto &pi: pile) {
		pi.second.pt.set_EyePos(cam.get_Position());
		pi.second.pt.draw_list(context, cam, pi.second.list_inst);
	}
}
//
void state_plasma::remove_all()
{
	for (auto &pi: pile) {
		pi.second.clear();
	}
}
//
void state_plasma::push_back(PLASMA_TYPE type, const float &count_down, const XMFLOAT3 &pos, bool is_ptr)
{
	auto do_push_back =
		[&](size_t &myindex, std::list<inst_plasam> &mylist, const particle &mypt) {
		++myindex;
		if (myindex > mypt.get_ListSize()-1) myindex = 0;
		mylist.emplace_back();
		mylist.back().pos = pos;
		mylist.back().count_down = count_down;
		mylist.back().slot = myindex;
		if (is_ptr) mylist.back().pos_ptr = &pos;
		};
	//
	do_push_back(pile[type].index, pile[type].list_inst, pile[type].pt);
}
//
}
#endif