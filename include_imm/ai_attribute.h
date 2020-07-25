////////////////
// ai_attribute.h
// by Huang Yiting
////////////////
////////////////
#ifndef AI_ATTRIBUTE_H
#define AI_ATTRIBUTE_H
#include "phy_magic.h"
#include "control_atk.h"
#include <map>
namespace imm
{
////////////////
// ai_points
////////////////
////////////////
struct ai_points
{
	ai_points();
	float hp_max;
	float ap_max;
	float poise_max;
	float hp;
	float ap;
	float poise;
	float str;
	float mgc;
	float def;
	float res;	
};
//
ai_points::ai_points():
	hp_max(20.0f),
	ap_max(20.0f),
	poise_max(5.0f),
	hp(20.0f),
	ap(20.0f),
	poise(1.0f),
	str(5.0f),
	mgc(5.0f),
	def(5.0f),
	res(5.0f)	
{
	;
}
////////////////
// ui_attr
////////////////
////////////////
template <typename T_app>
struct ui_attr
{
	ui_attr();
	void init(T_app *app_in);
	void reset();
	void update();
	void update_target();
	T_app *app;
	float p1_hp;
	float p1_ap;
	float p1_hp_max;
	float p1_ap_max;
	float tar_hp;
	float tar_hp_max;
	size_t tar_ix;
	size_t tar_size;
	size_t tar_flush;
	float dt_target_show;
	float dt_flush;
	bool need_resize;
};
//
template <typename T_app>
ui_attr<T_app>::ui_attr():
	app(nullptr),
	p1_hp(1.0f),
	p1_ap(1.0f),
	p1_hp_max(1.0f),
	p1_ap_max(1.0f),
	tar_hp(1.0f),
	tar_hp_max(1.0f),
	tar_ix(0),
	tar_size(0),
	tar_flush(0),
	dt_target_show(-1.0f),
	dt_flush(-1.0f),
	need_resize(false)
{
	;
}
//
template <typename T_app>
void ui_attr<T_app>::init(T_app *app_in)
{
	app = app_in;
}
//
template <typename T_app>
void ui_attr<T_app>::reset()
{
	dt_target_show = -1.0f;
}
//
template <typename T_app>
void ui_attr<T_app>::update()
{
	if (!app->m_UiMgr.status.m_IsActive) return;
	need_resize = false;
	if (!math::float_is_equal(p1_hp_max, app->m_AiAttr.points[app->m_Control.player1].hp_max)) {
		p1_hp_max = app->m_AiAttr.points[app->m_Control.player1].hp_max;
		app->m_UiMgr.status.define_set_hp_rect(p1_hp_max);
		need_resize = true;
	}
	if (!math::float_is_equal(p1_ap_max, app->m_AiAttr.points[app->m_Control.player1].ap_max)) {
		p1_ap_max = app->m_AiAttr.points[app->m_Control.player1].ap_max;
		app->m_UiMgr.status.define_set_ap_rect(p1_ap_max);
		need_resize = true;
	}
	if (!math::float_is_equal(p1_hp, app->m_AiAttr.points[app->m_Control.player1].hp)) {
		p1_hp = app->m_AiAttr.points[app->m_Control.player1].hp;
		float z = p1_hp / p1_hp_max;
		z = 1.0f - z;
		app->m_UiMgr.status.define_set_hp_bar(z);
		need_resize = true;
	}
	if (!math::float_is_equal(p1_ap, app->m_AiAttr.points[app->m_Control.player1].ap)) {
		p1_ap = app->m_AiAttr.points[app->m_Control.player1].ap;
		float z = p1_ap / p1_ap_max;
		z = 1.0f - z;
		app->m_UiMgr.status.define_set_ap_bar(z);
		need_resize = true;
	}
	update_target();
	if (need_resize) app->m_UiMgr.status.on_resize();
}
//
template <typename T_app>
void ui_attr<T_app>::update_target()
{
	if (dt_target_show > 0.0f) {
		dt_target_show -= AI_DT_LOGIC;
		if (app->m_UiMgr.status.m_IsOtherUIAppear) dt_target_show = -0.1f;
	}
	if (dt_target_show <0.0f && dt_target_show > -10.0f) {
		app->m_UiMgr.status.m_IsTarShow = false;
		app->m_UiMgr.status.group_active("tar", false);
		dt_target_show = -20.0f;
	}
	if (app->m_UiMgr.status.m_IsOtherUIAppear) return;
	if (tar_flush == app->m_Inst.m_Steering[app->m_Control.player1].attack.size()) {
		dt_flush -= AI_DT_LOGIC;
		if (dt_flush < 0.0f) return;
	}
	else {
		dt_flush = 0.3f;
	}
	tar_flush = app->m_Inst.m_Steering[app->m_Control.player1].attack.size();
	if (tar_flush == 0) return;
	size_t tmp_ix = app->m_Inst.m_Steering[app->m_Control.player1].attack.back();
	bool is_get_ix = false;
	if (app->m_Inst.m_Stat[tmp_ix].property & INST_IS_CONTROLLABLE) {
		tar_ix = tmp_ix;
		is_get_ix = true;
	}
	else {
		for (size_t ix = tar_size; ix < tar_flush-1; ++ix) {
			size_t instix = app->m_Inst.m_Steering[app->m_Control.player1].attack[ix];
			if (app->m_Inst.m_Stat[instix].property & INST_IS_CONTROLLABLE) {
				tar_ix = instix;
				is_get_ix = true;
			}
		}
	}
	if (!math::float_is_equal(tar_hp_max, app->m_AiAttr.points[tar_ix].hp_max) ||
		!math::float_is_equal(tar_hp, app->m_AiAttr.points[tar_ix].hp)) {
		tar_hp_max = app->m_AiAttr.points[tar_ix].hp_max;
		tar_hp = app->m_AiAttr.points[tar_ix].hp;
		float z = tar_hp / tar_hp_max;
		z = 1.0f - z;
		app->m_UiMgr.status.define_set_tar_hp_bar(z);
		app->m_UiMgr.status.define_set_tar_hp_rect(tar_hp_max);
		need_resize = true;
	}
	if (!is_get_ix) return;
	tar_size = tar_flush;
	dt_target_show = 10.0f;
	app->m_UiMgr.status.group_active("tar", true);
	app->m_UiMgr.status.define_set_tar_name(
		*app->m_Status.get_name(*app->m_Inst.m_Stat[tar_ix].get_ModelName()));
}
////////////////
// ai_attr
////////////////
////////////////
template <typename T_app>
struct ai_attr
{
	ai_attr();
	void init(T_app *app_in);
	void reset();
	void rebuild();
	void rebuild_points();
	void rebuild_troll();
	void update(const float &dt);
	void update_regenerate();
	void calc_skill_melee_immediately(
		const SKILL_SPECIFY &specify,
		const size_t &ix_atk,
		const size_t &ix_dmg);
	void calc_skill_magic_delay(const SKILL_SPECIFY &specify, const size_t &ix_atk, const size_t &ix_dmg);
	void calc_skill_melee_delay(
		const SKILL_SPECIFY &specify,
		const size_t &ix_atk,
		const size_t &ix_dmg,
		const int &order_stat_dmg,
		const float &poise);
	bool is_required_ap(const SKILL_SPECIFY &specify, const size_t &ix);
	bool is_poise_greater_than_zero(const size_t &ix);
	void should_be_destroyed(const size_t &ix);
	T_app *app;
	float delta_time;
	ui_attr<T_app> ui;
	std::map<size_t, ai_points> points;
	std::map<std::string, size_t> points_name;
};
//
template <typename T_app>
ai_attr<T_app>::ai_attr():
	app(nullptr),
	delta_time(math::calc_randf(-AI_DT_LOGIC, AI_DT_LOGIC)),
	ui(),
	points(),
	points_name()
{
	;
}
//
template <typename T_app>
void ai_attr<T_app>::init(T_app *app_in)
{
	app = app_in;
	ui.init(app);
	app->m_Status.init();
}
//
template <typename T_app>
void ai_attr<T_app>::reset()
{
	points.clear();
	points_name.clear();
	ui.reset();	
}
//
template <typename T_app>
void ai_attr<T_app>::rebuild()
{
	reset();
	rebuild_points();
	rebuild_troll();
}
//
template <typename T_app>
void ai_attr<T_app>::rebuild_points()
{
	size_t ix = 0;
	for (auto &stat: app->m_Inst.m_Stat) {
		if (stat.property & INST_IS_CONTROLLABLE) {
			points[ix];
			app->m_Status.assign_attr(points[ix], *(app->m_Inst.m_Stat[ix].get_ModelName()));
			points_name[*(app->m_Inst.m_Stat[ix].get_ModelName())] = ix;
		}
		++ix;
	}
	if (points.count(app->m_Control.player1)) app->m_UiMgr.status.m_IsActive = true;
	else app->m_UiMgr.status.m_IsActive = false;
}
//
template <typename T_app>
void ai_attr<T_app>::rebuild_troll()
{
	size_t ix = 0;
	for (auto &stat: app->m_Inst.m_Stat) {
		if (app->m_Status.ability.count(*stat.get_ModelName())) {
			app->m_Inst.m_Troll.at(ix).A = app->m_Status.ability[*stat.get_ModelName()];
		}
		++ix;
	}
}
//
template <typename T_app>
void ai_attr<T_app>::update(const float &dt)
{
	delta_time += dt;
	if (delta_time < AI_DT_LOGIC) return;
	else delta_time -= AI_DT_LOGIC;
	ui.update();
	update_regenerate();
}
//
template <typename T_app>
void ai_attr<T_app>::update_regenerate()
{
	for (auto &poi: points) {
		if (poi.second.ap > poi.second.ap_max-0.1f) continue;
		if (PTR->m_Control.atk.is_execute(poi.first)) continue;
		assert(poi.second.ap > 0.0f);
		poi.second.ap += poi.second.ap_max*0.2f*AI_DT_LOGIC;
		//
		if (poi.second.poise < poi.second.poise_max) {
			poi.second.poise += AI_DT_LOGIC;
		}
	}
}
//
template <typename T_app>
void ai_attr<T_app>::calc_skill_melee_immediately(
	const SKILL_SPECIFY &specify,
	const size_t &ix_atk,
	const size_t &ix_dmg)
{
	specify;
	app->m_Inst.m_Troll[ix_dmg].order_ac |= ORDER_DMG;
	if (app->m_Control.atk.current_impulse(ix_atk) > ATK_IMPULSE_DMG2) {
		assert(false);
		app->m_Inst.m_Troll[ix_dmg].order_ac |= ORDER_DMG2;
	}
}
//
template <typename T_app>
void ai_attr<T_app>::calc_skill_magic_delay(const SKILL_SPECIFY &specify, const size_t &ix_atk, const size_t &ix_dmg)
{
	ix_atk;
	app->m_Inst.m_Troll[ix_dmg].order_ac |= ORDER_DMG;
	switch (specify) {
	case SKILL_MAGIC_LIGHTNING:
		points[ix_dmg].hp -= 3.0f;
		if (points[ix_dmg].hp < 0.0f) {
			points[ix_dmg].hp = 0.0f;
			should_be_destroyed(ix_dmg);
		}
		break;
	case SKILL_MAGIC_MISSILE:
		points[ix_dmg].hp -= 30.0f;
		if (points[ix_dmg].hp < 0.0f) {
			points[ix_dmg].hp = 0.0f;
			should_be_destroyed(ix_dmg);
		}
		break;
	default:
		points[ix_dmg].hp -= 3.0f;
		if (points[ix_dmg].hp < 0.0f) {
			points[ix_dmg].hp = 0.0f;
			should_be_destroyed(ix_dmg);
		}
		break;
	}
}
//
template <typename T_app>
void ai_attr<T_app>::calc_skill_melee_delay(
	const SKILL_SPECIFY &specify,
	const size_t &ix_atk,
	const size_t &ix_dmg,
	const int &order_stat_dmg,
	const float &poise)
{
	ix_atk;
	switch (specify) {
	case SKILL_MELEE_STANDARD:
		assert(order_stat_dmg > -1);
		if (!(order_stat_dmg & ORDER_ST_GUARD)) {
			points[ix_dmg].hp -= 3.0f;
			points[ix_dmg].poise -= poise;
		}
		if (points[ix_dmg].hp < 0.0f) {
			points[ix_dmg].hp = 0.0f;
			should_be_destroyed(ix_dmg);
		}
		break;
	default:
		assert(order_stat_dmg > -1);
		if (!(order_stat_dmg & ORDER_ST_GUARD)) {
			points[ix_dmg].hp -= 3.0f;
			points[ix_dmg].poise -= poise;
		}
		if (points[ix_dmg].hp < 0.0f) {
			points[ix_dmg].hp = 0.0f;
			should_be_destroyed(ix_dmg);
		}
		break;
	}
}
//
template <typename T_app>
bool ai_attr<T_app>::is_required_ap(const SKILL_SPECIFY &specify, const size_t &ix_atk)
{
	switch (specify) {
	case SKILL_STAMINA_ROLL_FALSE:
		points[ix_atk].ap += 6.0f;
		if (points[ix_atk].ap > points[ix_atk].ap_max) points[ix_atk].ap = points[ix_atk].ap_max;
		return true;
		break;
	case SKILL_STAMINA_ROLL:
		if (points[ix_atk].ap < 1.0f) return false;
		points[ix_atk].ap -= 6.0f;
		if (points[ix_atk].ap < 0.01f) points[ix_atk].ap = 0.1f;
		return true;
		break;
	default:
		if (points[ix_atk].ap < 1.0f) return false;
		points[ix_atk].ap -= 0.5f;
		if (points[ix_atk].ap < 0.01f) points[ix_atk].ap = 0.1f;
		return true;
		break;
	}
}
//
template <typename T_app>
bool ai_attr<T_app>::is_poise_greater_than_zero(const size_t &ix)
{
	if (points[ix].poise > 0) return true;
	return false;
}
//
template <typename T_app>
void ai_attr<T_app>::should_be_destroyed(const size_t &ix)
{
	app->m_Inst.m_Troll[ix].order_ac |= ORDER_DMG;
	app->m_Inst.m_Troll[ix].order_ac |= ORDER_DMG_DOWN;
}
//
}
#endif