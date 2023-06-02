////////////////
// ui_mgr.h
// by Huang Yiting
////////////////
////////////////
#ifndef UI_MGR_H
#define UI_MGR_H
#include "ui_def_main_menu.h"
#include "ui_def_welcome.h"
#include "ui_def_dialogue.h"
#include "ui_def_status.h"
namespace imm
{
////////////////
// ui_mgr
////////////////
////////////////
template <typename T_app>
struct ui_mgr
{
	ui_mgr();
	void init(T_app *app_in);
	void on_resize();
	void define_update(float dt);
	void draw_d2d();
	void draw_d3d();
	bool on_mouse_down(WPARAM btn_state, const int &pos_x, const int &pos_y);
	bool on_mouse_wheel(const short &z_delta);
	void on_mouse_over(const int &pos_x, const int &pos_y);
	bool is_ui_appear();
	void on_input_keydown(WPARAM &w_param, LPARAM &l_param);
	void on_pad_keydown(const WORD &vkey);
	void define_deactivate_all_default();
	void define_deactivate_all_cmd_slient();
	// Reload
	void reload_active(const std::string &ui_class, const std::string &ui_group);
	void reset();
	void group_active(const std::string &ui_name, const std::string &g_name, const bool &is_act);
	T_app *app;
	ui_def_main_menu<T_app> main_menu;
	ui_def_welcome<T_app> welcome;
	ui_def_dialogue<T_app> dialogue;
	ui_def_status<T_app> status;
	std::vector<ui_base<T_app>*> ui_together;
	std::map<std::string, size_t> map_name;
	bool is_not_draw_response;
};
//
template <typename T_app>
ui_mgr<T_app>::ui_mgr():
	app(nullptr),
	main_menu(),
	welcome(),
	is_not_draw_response(false)
{
	;
}
//
template <typename T_app>
void ui_mgr<T_app>::init(T_app *app_in)
{
	app = app_in;
	main_menu.init(app_in);
	welcome.init(app_in);
	dialogue.init(app_in);
	status.init(app_in);
	ui_together.push_back(&main_menu);
	ui_together.push_back(&welcome);
	ui_together.push_back(&dialogue);
	ui_together.push_back(&status);
	map_name["main_menu"] = 0;
	map_name["welcome"] = 1;
	map_name["dialogue"] = 2;
	map_name["status"] = 2;
}
//
template <typename T_app>
void ui_mgr<T_app>::on_resize()
{
	for (auto &ui: ui_together) ui->on_resize();
}
//
template <typename T_app>
void ui_mgr<T_app>::define_update(float dt)
{
	for (auto &ui: ui_together) ui->define_update(dt);
	// Avoid UI conflict
	bool is_ui_appear = false;
	for (size_t ix = 0; ix != ui_together.size(); ++ix) {
		ui_together[ix]->m_IsOtherUIAppear = false;
		if (ui_together[ix]->is_ui_appear()) {
			is_ui_appear = true;
		}
	}
	if (is_ui_appear) {
		for (auto &ui: ui_together) {
			if (!ui->is_ui_appear()) ui->m_IsOtherUIAppear = true;
		}
	}
	// If aspect ratio over range, 4:3 - 21:9
	if (app->m_AspectRatio < 1.32f || app->m_AspectRatio > 2.34f) is_not_draw_response = true;
	else is_not_draw_response = false;
}
//
template <typename T_app>
void ui_mgr<T_app>::draw_d2d()
{
	if (is_not_draw_response) return;
	for (auto &ui: ui_together) ui->draw_d2d();
}
//
template <typename T_app>
void ui_mgr<T_app>::draw_d3d()
{
	if (is_not_draw_response) return;
	for (auto &ui: ui_together) ui->draw_d3d();
	// Restore default states.
	app->m_D3DDC->RSSetState(0);
	app->m_D3DDC->OMSetDepthStencilState(0, 0);
	app->m_D3DDC->OMSetBlendState(0, FLOAT_4_ZERO, 0xffffffff);
}
//
template <typename T_app>
bool ui_mgr<T_app>::on_mouse_down(WPARAM btn_state, const int &pos_x, const int &pos_y)
{
	if (is_not_draw_response) return false;
	bool rt_bool = false;
	for (auto &ui: ui_together) {
		rt_bool = ui->on_mouse_down(btn_state, pos_x, pos_y);
		if (rt_bool) {
			ui->define_enter_and_exit();
			return rt_bool;
		}
	}
	return rt_bool;
}
//
template <typename T_app>
bool ui_mgr<T_app>::on_mouse_wheel(const short &z_delta)
{
	if (is_not_draw_response) false;
	bool rt_bool = false;
	for (auto &ui: ui_together) {
		rt_bool = ui->on_mouse_wheel(z_delta);
		if (rt_bool) return rt_bool;
	}
	return rt_bool;
}
//
template <typename T_app>
void ui_mgr<T_app>::on_mouse_over(const int &pos_x, const int &pos_y)
{
	for (auto &ui: ui_together) ui->on_mouse_over(pos_x, pos_y);
}
//
template <typename T_app>
bool ui_mgr<T_app>::is_ui_appear()
{
	bool rt_bool = false;
	for (auto &ui: ui_together) {
		rt_bool = ui->is_ui_appear();
		if (rt_bool) return rt_bool;
	}
	return rt_bool;
}
//
template <typename T_app>
void ui_mgr<T_app>::on_input_keydown(WPARAM &w_param, LPARAM &l_param)
{
	if (is_not_draw_response) return;
	for (auto &ui: ui_together) {
		if (ui->on_input_keydown(w_param, l_param)) ui->define_enter_and_exit();
	}
}
//
template <typename T_app>
void ui_mgr<T_app>::on_pad_keydown(const WORD &vkey)
{
	if (is_not_draw_response) return;
	for (auto &ui: ui_together) {
		if (ui->on_pad_keydown(vkey)) ui->define_enter_and_exit();
	}
}
//
template <typename T_app>
void ui_mgr<T_app>::define_deactivate_all_default()
{
	for (auto &ui: ui_together) ui->define_deactivate_all_default();
}
//
template <typename T_app>
void ui_mgr<T_app>::define_deactivate_all_cmd_slient()
{
	for (auto &ui: ui_together) ui->define_deactivate_all_cmd_slient();
}
//
template <typename T_app>
void ui_mgr<T_app>::reload_active(const std::string &ui_class, const std::string &ui_group)
{
	reset();
	if (ui_class == "welcome") {
		if (!welcome.m_MapGroup.count(ui_group)) {
			std::string err_str("UI group not found: ");
			err_str += ui_group;
			ERROR_MESA(err_str.c_str());
		}
		main_menu.m_IsMute = true;
		welcome.m_IsMute = false;
		welcome.group_active(ui_group, true, false, true);
	}
	else {
		status.define_show(true);		
	}
}
//
template <typename T_app>
void ui_mgr<T_app>::reset()
{
	main_menu.m_IsMute = false;
	welcome.m_IsMute = true;
	define_deactivate_all_default();
	for (auto &ui: ui_together) ui->reset();
}
//
template <typename T_app>
void ui_mgr<T_app>::group_active(const std::string &ui_name, const std::string &g_name, const bool &is_act)
{
	assert(map_name.count(ui_name));
	define_deactivate_all_default();
	ui_together[map_name[ui_name]]->group_active(g_name, is_act);
}
//
}
#endif