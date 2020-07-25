////////////////
// phy_magic.h
// by Huang Yiting
////////////////
////////////////
#ifndef PHY_MAGIC_H
#define PHY_MAGIC_H
#include "phy_prepare.h"
#include "control_math.h"
#include "phy_motion.h"
#include <list>
namespace imm
{
////////////////
// SKILL_SPECIFY
////////////////
////////////////
enum SKILL_SPECIFY
{
	SKILL_MELEE_STANDARD     = 0,
	SKILL_MAGIC_PREPARE01    = 1,
	SKILL_MAGIC_LIGHTNING    = 2,
	SKILL_STAMINA_ROLL       = 3,
	SKILL_SYSTEM_ELIMINATE1  = 4,
	SKILL_SYSTEM_ELIMINATE2  = 5,
	SKILL_MAGIC_MISSILE      = 6,
	SKILL_STAMINA_ROLL_FALSE = 7,
};
//
SKILL_SPECIFY skill_specify_str(const std::string &str)
{
	if (str == "SKILL_MELEE_STANDARD") return SKILL_MELEE_STANDARD;
	if (str == "SKILL_MAGIC_PREPARE01") return SKILL_MAGIC_PREPARE01;
	if (str == "SKILL_MAGIC_LIGHTNING") return SKILL_MAGIC_LIGHTNING;
	if (str == "SKILL_MAGIC_MISSILE") return SKILL_MAGIC_MISSILE;
	assert(false);
	return SKILL_MELEE_STANDARD;
}
////////////////
// magic_inst
////////////////
////////////////
struct magic_inst
{
	magic_inst();
	~magic_inst();
	void update(const float &dt);
	void apply_MAGIC_LIGHTNING();
	void apply_MAGIC_MISSILE();
	void update_MAGIC_MISSILE();
	float plasma_duration();
	SKILL_SPECIFY specify;
	std::vector<moti_circle> vec_circle;
	std::vector<std::string> vec_bone_name;
	std::vector<BoundingSphere> vec_hit_shperes;
	std::set<size_t> set_hit_ix;
	float count_down;
	float duration;
	size_t caster;
	int target;
	bool is_run;
	bool is_stop;
	bool is_abort;
};
////////////////
// phy_magic
////////////////
////////////////
template <typename T_app>
struct phy_magic
{
	phy_magic();
	~phy_magic();
	void init(T_app *app_in);
	void remove_all();
	void update(const float &dt);
	void invoke(const SKILL_SPECIFY &specify, const size_t &caster);
	void push_MAGIC_LIGHTNING(const size_t &caster);
	void push_MAGIC_MISSILE(const size_t &caster);
	std::list<magic_inst> inst;
	T_app *app;
};
//
//
template <typename T_app>
phy_magic<T_app>::phy_magic():
	app(nullptr)
{
	;
}
//
template <typename T_app>
phy_magic<T_app>::~phy_magic()
{
	;
}
//
template <typename T_app>
void phy_magic<T_app>::init(T_app *app_in)
{
	app = app_in;
}
//
template <typename T_app>
void phy_magic<T_app>::remove_all()
{
	inst.clear();
}
//
template <typename T_app>
void phy_magic<T_app>::update(const float &dt)
{
	for (auto &ins: inst) ins.update(dt);
	auto is_should_del =
		[](const magic_inst &mag) {return (mag.is_stop);};
	inst.remove_if(is_should_del);
}
//
template <typename T_app>
void phy_magic<T_app>::invoke(const SKILL_SPECIFY &specify, const size_t &caster)
{
	switch (specify) {
	case SKILL_MAGIC_PREPARE01:
		app->m_Scene.audio.play_effect(sfx::Prepare01);
		app->m_Inst.m_Adapter.call_magic_text(caster, 1.5f);
		break;
	case SKILL_MAGIC_LIGHTNING:
		push_MAGIC_LIGHTNING(caster);
		break;
	case SKILL_MAGIC_MISSILE:
		app->m_Inst.m_Adapter.call_magic_text(caster, 0.9f);
		push_MAGIC_MISSILE(caster);
		break;
	}
}
//
template <typename T_app>
void phy_magic<T_app>::push_MAGIC_LIGHTNING(const size_t &caster)
{
	inst.emplace_back();
	inst.back().count_down = 0.5f;
	inst.back().specify = SKILL_MAGIC_LIGHTNING;
	inst.back().caster = caster;
	inst.back().vec_hit_shperes.resize(1);
	inst.back().vec_hit_shperes[0].Radius = 4.0f;
}
//
template <typename T_app>
void phy_magic<T_app>::push_MAGIC_MISSILE(const size_t &caster)
{
	inst.emplace_back();
	inst.back().count_down = 0.5f;
	inst.back().specify = SKILL_MAGIC_MISSILE;
	inst.back().caster = caster;
	inst.back().vec_hit_shperes.resize(1);
	inst.back().vec_hit_shperes[0].Radius = 0.5f;
}
//
}
#endif