////////////////
// control_sys.h
// by Huang Yiting
////////////////
////////////////
#ifndef CONTROL_SYS_H
#define CONTROL_SYS_H
#include "control_cam.h"
#include "control_atk.h"
namespace imm
{
////////////////
// control_sys
////////////////
////////////////
template <typename T_app>
struct control_sys
{
	control_sys();
	// logic fuction
	void init(T_app *app_in);
	void reset();
	void rebuild();
	void mouse_inst_move();
	void pad_inst_move_update();
	void key_inst_move_update();
	void key_inst_guard_update();
	void key_inst_move_switch_stat();
	void inst_jump();
	void inst_roll();
	void inst_atk_x();
	void inst_atk_y();
	void inst_guard(const bool &is_guard);
	void mouse_pick();
	void set_player1(const size_t &p1);
	void update_scene(const float &dt);
	void update_scene_bounds();
	void update_stop(const float &dt);
	void update_keydown_and_pad(const float &dt);
	void on_mouse_down(WPARAM btn_state, const int &pos_x, const int &pos_y);
	void on_pad_down_update(const float &dt);
	void on_input_keydown(WPARAM &w_param, LPARAM &l_param);
	void on_input_keyup(WPARAM &w_param, LPARAM &l_param);
	void on_mouse_move(WPARAM btn_state, const int &pos_x, const int &pos_y);
	void on_mouse_wheel(const short &z_delta);
	// member variable
	T_app *app;
	int picked1;
	int player1;
	int style1;
	float wait_ui_disappear;
	float idle_time;
	POINT mouse_down;
	POINT mouse_move;
	std::map<size_t, control_stop<T_app>> map_stop;
	std::map<size_t, control_speed<T_app>> map_speed;
	control_xinput pad;
	control_cam<T_app> cam;
	control_atk<T_app> atk;
	input_setting setting;
};
//
template <typename T_app>
control_sys<T_app>::control_sys():
	app(nullptr),
	picked1(-1),
	player1(-1),
	style1(CONTROL_CAM_FOLLOW),
	wait_ui_disappear(0.0f),
	idle_time(0.0f),
	pad(),
	cam(),
	atk(),
	setting()
{
	mouse_down.x = 0;
	mouse_down.y = 0;
	mouse_move.x = 0;
	mouse_move.y = 0;
}
//
template <typename T_app>
void control_sys<T_app>::init(T_app *app_in)
{
	app = app_in;
	cam.init(app);
	atk.init(app);
}
//
template <typename T_app>
void control_sys<T_app>::reset()
{
	map_stop.clear();
	map_speed.clear();
	cam.reset();
	atk.reset();
	player1 = -1;
	picked1 = -1;
}
//
template <typename T_app>
void control_sys<T_app>::rebuild()
{
	player1 = static_cast<int>(app->m_Inst.get_index(app->m_Scene.get_misc["player1"]));
	assert(player1 > -1);
	atk.rebuild_action();
}
//
template <typename T_app>
void control_sys<T_app>::mouse_inst_move()
{
	if (player1 < 0 || !(style1 & CONTROL_CAM_FREE)) return;
	app->m_Inst.m_Troll[player1].order_ac|= ORDER_MOVE_HIT;
}
//
template <typename T_app>
void control_sys<T_app>::pad_inst_move_update()
{
	if (player1 < 0) return;
	app->m_Inst.m_Troll[player1].order_ac|= ORDER_MOVE_TOWARD;
	if (pad.is_RT_press()) app->m_Inst.m_Troll[player1].order_stat |= ORDER_ST_WALK;
	else app->m_Inst.m_Troll[player1].order_stat &= ~ORDER_ST_WALK;
}
//
template <typename T_app>
void control_sys<T_app>::key_inst_move_update()
{
	if (player1 < 0 || (style1 & CONTROL_CAM_FREE)) return;
	app->m_Inst.m_Troll[player1].order_ac|= ORDER_MOVE_WASD;
}
//
template <typename T_app>
void control_sys<T_app>::key_inst_move_switch_stat()
{
	app->m_Inst.m_Troll[player1].order_stat ^= ORDER_ST_WALK;
}
//
template <typename T_app>
void control_sys<T_app>::inst_jump()
{
	app->m_Inst.m_Troll[player1].order_ac|= ORDER_JUMP;
}
//
template <typename T_app>
void control_sys<T_app>::inst_roll()
{
	app->m_Inst.m_Troll[player1].order_ac|= ORDER_ROLL;
}
//
template <typename T_app>
void control_sys<T_app>::inst_atk_x()
{
	app->m_Inst.m_Troll[player1].order_ac|= ORDER_ATK_X;
}
//
template <typename T_app>
void control_sys<T_app>::inst_atk_y()
{
	app->m_Inst.m_Troll[player1].order_ac|= ORDER_ATK_Y;
}
//
template <typename T_app>
void control_sys<T_app>::inst_guard(const bool &is_guard)
{
	if (is_guard) {
		if (~app->m_Inst.m_Troll[player1].order_stat & ORDER_ST_GUARD) {
			app->m_Inst.m_Troll[player1].order_ac|= ORDER_GUARD;
		}
		app->m_Inst.m_Troll[player1].order_stat |= ORDER_ST_GUARD;
	}
	else {
		if (~app->m_Inst.m_Troll[player1].order_stat & ORDER_ST_GUARD) return;
		app->m_Inst.m_Troll[player1].order_ac|= ORDER_GUARD_NO;
	}
}
//
template <typename T_app>
void control_sys<T_app>::mouse_pick()
{
	app->m_Inst.m_BoundW.pick(
		mouse_down.x,
		mouse_down.y,
		app->m_ClientWidth,
		app->m_ClientHeight,
		app->m_Cam.get_Proj(),
		app->m_Cam.get_View(),
		picked1);
	//
	if (picked1 < 0) return;
	set_player1(picked1);
}
//
template <typename T_app>
void control_sys<T_app>::set_player1(const size_t &p1)
{
	int previous_p1 = player1;
	if (app->m_Inst.m_Stat[previous_p1].type == MODEL_SIMPLE_P) {
		app->m_Inst.m_Stat[previous_p1].phy.is_drop_fix = true;
	}
	app->m_Inst.m_Troll[previous_p1].order_ac= ORDER_NONE;
	app->m_Inst.m_Troll[previous_p1].order_stat |= ORDER_ST_CLEAR;
	player1 = static_cast<int>(p1);
	assert(previous_p1 != player1);
	map_stop[player1].is_stop = true;
}
//
template <typename T_app>
void control_sys<T_app>::update_scene(const float &dt)
{
	// assert check update
	assert(player1 > -1);
	app->m_Inst.update_all_physics1(dt);
	atk.update(dt);
	app->m_Inst.update_all_physics2(dt);
	update_scene_bounds();
	update_stop(dt);
	// camera follow update even m_Cmd.is_active()
	cam.follow_update();
}
//
template <typename T_app>
void control_sys<T_app>::update_scene_bounds()
{
	// bounds follow player, if distance too far, update
	XMFLOAT3 p1_center_f = app->m_Inst.m_BoundW.center(player1);
	XMVECTOR p1_center = XMLoadFloat3(&p1_center_f);
	XMVECTOR scene_center = XMLoadFloat3(&app->m_Scene.bounds.Center);
	XMVECTOR distance = XMVectorSubtract(p1_center, scene_center);
	distance = XMVector3LengthEst(distance);
	if (XMVectorGetX(distance) > 20.0f) {
		app->m_Scene.bounds.Center = app->m_Inst.m_BoundW.center(player1);
	}
}
//
template <typename T_app>
void control_sys<T_app>::update_stop(const float &dt)
{
	for (auto it = map_stop.begin(); it != map_stop.end(); ++it) {
		it->second.update(app, it->first, dt);
	}
	for (auto it = map_speed.begin(); it != map_speed.end(); ++it) {
		it->second.update(app, it->first, dt);
	}	
}
//
template <typename T_app>
void control_sys<T_app>::update_keydown_and_pad(const float &dt)
{
	if (app->m_Cmd.is_active) return;
	idle_time += dt;
	// player1 and camera update, if !m_Cmd.is_active()
	if (pad.is_enable()) {
		on_pad_down_update(dt);
		pad_inst_move_update();
		cam.pad_update(dt);
	}
	else {
		key_inst_move_update();
		if (GetKeyState(KEY_P1_GUARD) & 0x8000) inst_guard(true);
		else inst_guard(false);
	}
	cam.key_free_update(dt);
}
//
template <typename T_app>
void control_sys<T_app>::on_mouse_down(WPARAM btn_state, const int &pos_x, const int &pos_y)
{
	idle_time = 0;
	mouse_down.x = pos_x;
	mouse_down.y = pos_y;
	if (app->m_UiMgr.on_mouse_down(btn_state, mouse_down.x, mouse_down.y)) return;
	if (pad.is_enable()) return;
	if (app->m_Cmd.is_active) return;
	// player
	if (style1 & CONTROL_CAM_FREE) {
		assert(false);
		if (btn_state & MOUSE_P1_PICK) mouse_pick();
		if (btn_state & MOUSE_P1_MOVE) mouse_inst_move();
	}
	else {
		if (btn_state & MOUSE_P1_ATK_X) inst_atk_x();
		if (btn_state & MOUSE_P1_ATK_Y) inst_atk_y();
	}
}
//
template <typename T_app>
void control_sys<T_app>::on_pad_down_update(const float &dt)
{
	idle_time = 0;
	if (app->m_Cmd.is_active) return;
	WORD get_vkey = 0;
	bool is_on_keydown = pad.is_on_keydown(get_vkey);
	if (is_on_keydown) app->m_UiMgr.on_pad_keydown(get_vkey);
	// avoid ui conflict with control
	if (app->m_UiMgr.is_ui_appear()) wait_ui_disappear = 0.3f;
	wait_ui_disappear -= dt;
	if (wait_ui_disappear > 0.0f) return;
	wait_ui_disappear = -1.0f;
	if (is_on_keydown) cam.on_pad_follow(get_vkey);
	if (player1 < 0) return;
	if (get_vkey == PAD_P1_JUMP) inst_jump();
	if (get_vkey == PAD_P1_ATK_X) inst_atk_x();
	if (get_vkey == PAD_P1_ATK_Y) inst_atk_y();
	if (pad.is_L_active()) {
		if (pad.is_LB_press()) {
			if (app->m_Inst.m_Troll[player1].current_state != pose_Roll::instance()) {
				inst_roll();
			}
		}
	}
	else {
		inst_guard(pad.is_LB_press());
	}
}
//
template <typename T_app>
void control_sys<T_app>::on_input_keydown(WPARAM &w_param, LPARAM &l_param)
{
	idle_time = 0;
	if (!app->m_Cmd.is_active) app->m_UiMgr.on_input_keydown(w_param, l_param);
	if (pad.is_enable()) return;
	if (app->m_Cmd.is_active) return;
	if (player1 < 0) return;
	if (w_param == KEY_P1_WALK_RUN) key_inst_move_switch_stat();
	if (w_param == KEY_P1_JUMP) inst_jump();
	if (w_param == KEY_P1_ROLL) inst_roll();
}
//
template <typename T_app>
void control_sys<T_app>::on_input_keyup(WPARAM &w_param, LPARAM &l_param)
{
	idle_time = 0;
	DUMMY(l_param);
	DUMMY(w_param);
}
//
template <typename T_app>
void control_sys<T_app>::on_mouse_move(WPARAM btn_state, const int &pos_x, const int &pos_y)
{
	idle_time = 0;
	mouse_move.x = pos_x;
	mouse_move.y = pos_y;
	if ((btn_state & MOUSE_CAM_MOVE || MOUSE_CAM_MOVE == 0)) cam.mouse_move();
	app->m_UiMgr.on_mouse_over(mouse_move.x, mouse_move.y);
}
//
template <typename T_app>
void control_sys<T_app>::on_mouse_wheel(const short &z_delta)
{
	idle_time = 0;
	if (app->m_UiMgr.on_mouse_wheel(z_delta)) return;
	cam.mouse_wheel(z_delta);
}
//
}
#endif