////////////////
// ai_interface.h
// by Huang Yiting
////////////////
////////////////
#ifndef AI_INTERFACE_H
#define AI_INTERFACE_H
#include "ai_steering.h"
namespace imm
{
////////////////
////////////////
// AI_TYPE
////////////////
////////////////
enum AI_TYPE
{
	AI_NONE           = 0,
	AI_BEAT_PLAYER1   = 1,
	AI_ASSIST_PLAYER1 = 2,
};
////////////////
// ai_mental
////////////////
////////////////
struct ai_mental
{
	ai_mental();
	size_t ix;
	std::string name;
	int type;
	bool is_start;
};
ai_mental::ai_mental():
	ix(0),
	name(),
	type(AI_BEAT_PLAYER1),
	is_start(false)
{
	;
}
////////////////
// ai_interface
////////////////
////////////////
template <typename T_app>
struct ai_interface
{
	ai_interface();
	void init_load(T_app *app_in);
	void reset();
	void rebuild();
	void update(const float &dt);
	void update_beat_player(ai_mental &mind);
	void update_assist_player(ai_mental &mind);
	void set_Seek();
	int get_current_tactics(const size_t &ix);
	T_app *app;
	float delta_time;
	std::vector<ai_mental> mental_data;
	std::vector<ai_mental> mental_scene;
};
//
template <typename T_app>
ai_interface<T_app>::ai_interface():
	app(nullptr),
	delta_time(math::calc_randf(-AI_DT_LOGIC, AI_DT_LOGIC)),
	mental_data(),
	mental_scene()
{
	;
}
//
template <typename T_app>
void ai_interface<T_app>::init_load(T_app *app_in)
{
	app = app_in;
	//
	mental_data.emplace_back();
	mental_data.back().name = "black_warrior";
	mental_data.back().type = AI_BEAT_PLAYER1;
	mental_data.emplace_back();
	mental_data.back().name = "black_warrior2";
	mental_data.back().type = AI_BEAT_PLAYER1;
	mental_data.emplace_back();
	mental_data.back().name = "silver_warrior";
	mental_data.back().type = AI_BEAT_PLAYER1;//*/
	mental_data.emplace_back();
	mental_data.back().name = "tualatin";
	mental_data.back().type = AI_ASSIST_PLAYER1;
}
//
template <typename T_app>
void ai_interface<T_app>::reset()
{
	mental_scene.clear();
}
//
template <typename T_app>
void ai_interface<T_app>::rebuild()
{
	reset();
	for (auto &data: mental_data) {
		if (!app->m_Inst.m_NameMap.count(data.name)) continue;
		mental_scene.push_back(data);
		mental_scene.back().ix = app->m_Inst.m_NameMap[data.name];
		PTR->m_Inst.m_Steering[mental_scene.back().ix].is_active = true;
	}
}
//
template <typename T_app>
void ai_interface<T_app>::update(const float &dt)
{
	delta_time += dt;
	if (delta_time < AI_DT_LOGIC) return;
	else delta_time -= AI_DT_LOGIC;
	//
	for (auto &mind: mental_scene) {
		if (mind.type & AI_BEAT_PLAYER1) update_beat_player(mind);
		if (mind.type & AI_ASSIST_PLAYER1) update_assist_player(mind);
	}
	//
	app->m_AiInfo.update();
}
//
template <typename T_app>
void ai_interface<T_app>::update_beat_player(ai_mental &mind)
{
	auto ste = &app->m_Inst.m_Steering[mind.ix];
	if (!mind.is_start) {
		ste->target = PTR->m_Control.player1;
		ste->count_down = -1.0f;
		ste->tactics |= AI_TAC_PATROL;
		app->m_Inst.m_Probe.set_active(mind.ix);
		mind.is_start = true;
	}
	if (ste->report & AI_REP_TAR_CLOSE) {
		ste->report = AI_REP_NONE;
		ste->tactics |= AI_TAC_ATK;
	}
	if (ste->report & AI_REP_TAR_AWAY) {
		ste->report = AI_REP_NONE;
		ste->tactics |= AI_TAC_SEEK;
	}
	if (ste->report & AI_REP_ALERT) {
		ste->report = AI_REP_NONE;
		ste->tactics |= AI_TAC_SEEK;
	}
	if (ste->report & AI_REP_TAR_ELIMINATE) {
		ste->report = AI_REP_NONE;
		ste->tactics |= AI_TAC_PATROL;
	}
}
//
template <typename T_app>
void ai_interface<T_app>::update_assist_player(ai_mental &mind)
{
	auto ste = &app->m_Inst.m_Steering[mind.ix];
	if (!mind.is_start) {
		ste->target = PTR->m_Control.player1;
		ste->count_down = -1.0f;
		ste->tactics |= AI_TAC_FOLLOW;
		app->m_Inst.m_Probe.set_active(mind.ix);
		mind.is_start = true;
	}
	//
	if (ste->report & AI_REP_TAR_CLOSE) {
		ste->report = AI_REP_NONE;
		ste->tactics |= AI_TAC_ATK;
	}
	if (ste->report & AI_REP_TAR_AWAY) {
		ste->report = AI_REP_NONE;
		ste->tactics |= AI_TAC_SEEK;
	}
	if (ste->report & AI_REP_ALERT) {
		ste->report = AI_REP_NONE;
		ste->tactics |= AI_TAC_SEEK;
	}
	if (ste->report & AI_REP_TAR_ELIMINATE) {
		ste->report = AI_REP_NONE;
		ste->tactics |= AI_TAC_FOLLOW;
	}
}
//
template <typename T_app>
int ai_interface<T_app>::get_current_tactics(const size_t &ix)
{
	auto ste = &app->m_Inst.m_Steering[ix];
	if (ste->current_state == ai_Standby::instance()) return AI_TAC_STANDY;
	if (ste->current_state == ai_Patrol::instance()) return AI_TAC_PATROL;
	if (ste->current_state == ai_Seek::instance()) return AI_TAC_SEEK;
	if (ste->current_state == ai_Atk::instance()) return AI_TAC_ATK;
	if (ste->current_state == ai_Follow::instance()) return AI_TAC_FOLLOW;
	return AI_TAC_NONE;
}
//
}
#endif