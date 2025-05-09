////////////////
// condition_trigger.h
// by Huang Yiting
////////////////
////////////////
#ifndef CONDITION_TRIGGER_H
#define CONDITION_TRIGGER_H
#include <string>
#include <map>
namespace imm
{
////////////////
// condition_task
////////////////
////////////////
struct condition_task
{
	enum type {
		once_time,
	};
	condition_task();
	bool is_ran;
	float certain_time;
};
//
condition_task::condition_task():
	is_ran(false),
	certain_time(0.0f)
{
	;
}
////////////////
// condition_trigger
////////////////
////////////////
template <typename T_app>
struct condition_trigger
{
	condition_trigger();
	void init(T_app *app_in);
	void reset();
	bool trigger(const std::string &task_name);
	void update(const float &dt);
	void update_scene_00();
	void update_scene_01();
	void update_scene_02();
	float scene_pass_time;
	float delta_time;
	std::map<std::string, condition_task> task;
	T_app *app;
};
//
template <typename T_app>
condition_trigger<T_app>::condition_trigger():
	scene_pass_time(0.0f),
	delta_time(math::calc_randf(-AI_DT_LOGIC, AI_DT_LOGIC)),
	app(nullptr)
{
	;
}
//
template <typename T_app>
void condition_trigger<T_app>::reset()
{
	for (auto &tas: task) {
		tas.second.is_ran = false;
	}
}
//
template <typename T_app>
bool condition_trigger<T_app>::trigger(const std::string &task_name)
{
	auto tas = &task.at(task_name);
	if (tas->is_ran) return false;
	if (scene_pass_time > tas->certain_time) {
		tas->is_ran = true;
		return true;
	}
	else return false;
}
//
template <typename T_app>
void condition_trigger<T_app>::init(T_app *app_in)
{
	app = app_in;
	task.emplace("clear_cmd", condition_task());
	task["clear_cmd"].certain_time = 3.0f;
	task.emplace("_01_test", condition_task());
	task.emplace("_00_add_fire", condition_task());
	task["_00_add_fire"].certain_time = -1.0f;
	task["_01_test"].certain_time = 5.0f;
	task.emplace("_02_test", condition_task());
	task["_02_test"].certain_time = 5.0f;
}
//
template <typename T_app>
void condition_trigger<T_app>::update(const float &dt)
{
	delta_time += dt;
	if (delta_time < AI_DT_LOGIC) return;
	else delta_time -= AI_DT_LOGIC;
	scene_pass_time = app->m_Scene.pass_time();
	if (trigger("clear_cmd")) {
		if (!app->m_Cmd.is_active) app->m_Cmd.input.clear();
	}
	//
	update_scene_00();
	update_scene_01();
	update_scene_02();
}
//
template <typename T_app>
void condition_trigger<T_app>::update_scene_00()
{
	if (app->m_Scene.scene_ix != "_00") return;
	if (trigger("_00_add_fire")) {
		app->m_Scene.plasma.push_back(PLASMA_FIRE, 60.0f, XMFLOAT3(16.0f, 18.0f, -10.0f));
	}
}
//
template <typename T_app>
void condition_trigger<T_app>::update_scene_01()
{
	if (app->m_Scene.scene_ix != "_01") return;
	if (trigger("_01_test")) {
		app->m_UiMgr.group_active("dialogue", "test", true);
	}
}
//
template <typename T_app>
void condition_trigger<T_app>::update_scene_02()
{
	if (app->m_Scene.scene_ix != "_02") return;
	if (trigger("_02_test")) {
		app->m_UiMgr.group_active("dialogue", "test", true);
	}
}
//
}
#endif