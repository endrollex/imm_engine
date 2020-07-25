////////////////
// control_cam.h
// by Huang Yiting
////////////////
////////////////
#ifndef CONTROL_CAM_H
#define CONTROL_CAM_H
#include "control_auxiliary.h"
#include <map>
namespace imm
{
////////////////
// CONTROL_STYLE_TYPE
////////////////
////////////////
enum CONTROL_STYLE_TYPE {
	CONTROL_CAM_FOLLOW = 0x0, // MOVE_BY_WASD
	CONTROL_CAM_FREE   = 0x1, // MOVE_BY_MOUSE
};
////////////////
// control_cam
////////////////
////////////////
template <typename T_app>
struct control_cam
{
	control_cam();
	void init(T_app *app_in);
	void reset();
	void pad_update(const float &dt);
	void on_pad_follow(const WORD &vkey);
	void key_free_update(const float &dt);
	void mouse_wheel(const short &z_delta);
	void mouse_move();
	void follow_update();
	XMVECTOR get_rot_front_conj(const size_t &ix);
	void follow_update_reset(const XMVECTOR &rot_quat);
	bool is_restrict();
	void rebuild_setting();
	float follow_up_def_value();
	T_app *app;
	bool is_pad_follow_reset;
	bool is_smooth;
	bool is_resetting;
	float follow_walk_def;
	float follow_up_def1;
	float follow_up_def2;
	float follow_walk;
	float follow_walk_min;
	float follow_walk_max;
	float follow_up;
	float follow_reset_timer;
	float follow_reset_cd_max;
	float restrict_pitch_range;
	float restrict_pitch_value;
	XMFLOAT3 reset_R;
	XMFLOAT3 reset_U;
	XMFLOAT3 reset_L;
	std::map<size_t, XMFLOAT4> map_rot_front_c;
};
//
template <typename T_app>
control_cam<T_app>::control_cam():
	app(nullptr),
	is_pad_follow_reset(false),
	is_smooth(true),
	is_resetting(false),
	follow_walk_def(-30.0f),
	follow_up_def1(3.0f),
	follow_up_def2(0.0f),
	follow_walk_min(-40.0f),
	follow_walk_max(-10.0f),
	follow_reset_timer(0.0f),
	follow_reset_cd_max(5.0f),
	restrict_pitch_range(0.0f),
	restrict_pitch_value(0.0f),
	reset_R(0.0f, 0.0f, 0.0f),
	reset_U(0.0f, 0.0f, 0.0f),
	reset_L(0.0f, 0.0f, 0.0f)
{
	follow_walk = follow_walk_def;
	follow_up = follow_up_def1;
}
//
template <typename T_app>
void control_cam<T_app>::init(T_app *app_in)
{
	app = app_in;
}
//
template <typename T_app>
void control_cam<T_app>::reset()
{
	map_rot_front_c.clear();
	restrict_pitch_value = 0.0f;
}
//
template <typename T_app>
void control_cam<T_app>::pad_update(const float &dt)
{
	if (app->m_Control.pad.is_R_active() && !is_resetting) {
		if (is_restrict()) return;
		float dx = XMConvertToRadians(50.0f*app->m_Control.pad.state.Gamepad.sThumbRX/32767*dt);
		float dy = XMConvertToRadians(50.0f*app->m_Control.pad.state.Gamepad.sThumbRY/32767*dt);
		app->m_Cam.pitch(dy);
		app->m_Cam.rotate_y(-dx);
		is_smooth = false;
	}
	else {
		is_smooth = true;
	}
	if (app->m_UiMgr.is_ui_appear()) return;
	WORD w_buttons = app->m_Control.pad.state.Gamepad.wButtons;
	if (app->m_Control.style1 & CONTROL_CAM_FREE) {
		if (w_buttons & XGPAD_CAM_FREE_UP) app->m_Cam.up_down(10.0f*dt);
		if (w_buttons & XGPAD_CAM_FREE_DOWN) app->m_Cam.up_down(-10.0f*dt);
		if (w_buttons & XGPAD_CAM_FREE_LEFT) app->m_Cam.strafe(-10.0f*dt);
		if (w_buttons & XGPAD_CAM_FREE_RIGHT) app->m_Cam.strafe(10.0f*dt);
		if (w_buttons & XGPAD_CAM_FREE_FORWARD) app->m_Cam.walk(10.0f*dt);
		if (w_buttons & XGPAD_CAM_FREE_BACKWARD) app->m_Cam.walk(-10.0f*dt);
	}
	else {
		if (w_buttons & XGPAD_CAM_FOLLOW_FORWARD) follow_walk += 10.0f*dt;
		if (w_buttons & XGPAD_CAM_FOLLOW_BACKWARD) follow_walk += -10.0f*dt;
		follow_walk = math::calc_clamp(follow_walk, follow_walk_min, follow_walk_max);
		follow_up = follow_walk * (follow_up_def_value()/follow_walk_def);
	}
}
//
template <typename T_app>
void control_cam<T_app>::on_pad_follow(const WORD &vkey)
{
	if (app->m_Control.style1 & CONTROL_CAM_FREE) return;
	if (vkey == PAD_CAM_FOLLOW_RESET) is_pad_follow_reset = true;
}
//
template <typename T_app>
void control_cam<T_app>::key_free_update(const float &dt)
{
	if (!(app->m_Control.style1 & CONTROL_CAM_FREE)) return;
	if (GetKeyState(KEY_CAM_FREE_LEFT) & 0x8000) app->m_Cam.strafe(-10.0f*dt);
	if (GetKeyState(KEY_CAM_FREE_RIGHT) & 0x8000) app->m_Cam.strafe(10.0f*dt);
	if (GetKeyState(KEY_CAM_FREE_UP) & 0x8000) app->m_Cam.up_down(10.0f*dt);
	if (GetKeyState(KEY_CAM_FREE_DOWN) & 0x8000) app->m_Cam.up_down(-10.0f*dt);
}
//
template <typename T_app>
void control_cam<T_app>::mouse_wheel(const short &z_delta)
{
	if (app->m_Control.style1 & CONTROL_CAM_FREE) {
		app->m_Cam.walk(z_delta/120*1.0f);
	}
	else {
		follow_walk += z_delta/120*1.0f;
		follow_walk = math::calc_clamp(follow_walk, follow_walk_min, follow_walk_max);
		follow_up = follow_walk * (follow_up_def_value()/follow_walk_def);
	}
}
//
template <typename T_app>
void control_cam<T_app>::mouse_move()
{
	if (is_resetting) return;
	// Make each pixel correspond to a quarter of a degree.
	float dx = XMConvertToRadians(0.25f*static_cast<float>(app->m_Control.mouse_move.x - app->m_LastMousePos.x));
	float dy = XMConvertToRadians(0.25f*static_cast<float>(app->m_Control.mouse_move.y - app->m_LastMousePos.y));
	if (is_restrict()) {
		if (abs(restrict_pitch_value+dy) > restrict_pitch_range) dy = 0.0f;
		restrict_pitch_value += dy;
		dx = 0.0f;
	}
	app->m_Cam.pitch(dy);
	app->m_Cam.rotate_y(dx);
	// keep move continuous
	is_smooth = false;
	follow_update();
	is_smooth = true;
}
//
template <typename T_app>
void control_cam<T_app>::follow_update()
{
	if (app->m_Control.player1 < 0 || app->m_Control.style1 & CONTROL_CAM_FREE) return;
	int player1 = app->m_Control.player1;
	XMFLOAT4X4 player_world = *(app->m_Inst.m_Stat[player1].get_World());
	XMMATRIX W = XMLoadFloat4x4(&player_world);
	XMVECTOR scale, rot_quat, pos, L, U;
	XMMatrixDecompose(&scale, &rot_quat, &pos, W);
	// walk, up
	scale = XMVectorReplicate(follow_walk);
	L = XMLoadFloat3(&app->m_Cam.m_Look);
	pos = XMVectorMultiplyAdd(scale, L, pos);
	scale = XMVectorReplicate(follow_up);
	U = XMLoadFloat3(&app->m_Cam.m_Up);
	pos = XMVectorMultiplyAdd(scale, U, pos);
	if (is_smooth) {
		// default 1fps = 1/60s
		float factor = app->m_Timer.delta_time()*FPS60*0.5f;
		XMVECTOR pos_old = app->m_Cam.get_PositionXM();
		pos = XMVectorLerp(pos_old, pos, factor);
	}
	XMStoreFloat3(&app->m_Cam.m_Position, pos);
	// reset
	if (is_restrict()) return;
	if (follow_reset_timer > -1.0f) {
		follow_reset_timer -= app->m_Timer.delta_time();
	}
	if (follow_reset_timer < -2.0f) {
		follow_reset_timer += app->m_Timer.delta_time();
		return;
	}
	if (GetKeyState(KEY_CAM_FOLLOW_RESET) & 0x8000 || is_pad_follow_reset) {
		follow_reset_timer = follow_reset_cd_max;
	}
	if (follow_reset_timer > 0.0f) follow_update_reset(rot_quat);
}
//
template <typename T_app>
XMVECTOR control_cam<T_app>::get_rot_front_conj(const size_t &ix)
{
	if (!map_rot_front_c.count(ix)) {
		XMVECTOR scale, rot_front_conj, translation;
		XMMATRIX RF = XMLoadFloat4x4(app->m_Inst.m_Stat[ix].get_RotFront());
		XMMatrixDecompose(&scale, &rot_front_conj, &translation, RF);
		rot_front_conj = XMQuaternionConjugate(rot_front_conj);
		XMStoreFloat4(&map_rot_front_c[ix], rot_front_conj);
	}
	return XMLoadFloat4(&map_rot_front_c[ix]);
}
//
template <typename T_app>
void control_cam<T_app>::follow_update_reset(const XMVECTOR &rot_quat)
{
	is_pad_follow_reset = false;
	follow_walk = follow_walk_def;
	XMVECTOR rot_front_conj, R, L, U, epsilon, R_old, U_old, L_old;
	if (!is_resetting) {
		is_resetting = true;
		assert(app->m_Control.player1 > -1);
		rot_front_conj = get_rot_front_conj(app->m_Control.player1);
		L = XMVectorSet(0.0f, 0.0f, -1.0f, 0.0f);
		L = XMVector3Rotate(L, rot_front_conj);
		L = XMVector3Rotate(L, rot_quat);
		L = XMVector3Normalize(L);
		U = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
		R = XMVector3Normalize(XMVector3Cross(U, L));
		U = XMVector3Cross(L, R);
		// pitch, rotate cam to default angle, about 0.34f
		XMMATRIX M_R = XMMatrixRotationAxis(R, 0.34f);
		U = XMVector3TransformNormal(U, M_R);
		L = XMVector3TransformNormal(L, M_R);
		XMStoreFloat3(&reset_L, L);
		XMStoreFloat3(&reset_R, R);
		XMStoreFloat3(&reset_U, U);
	}
	else {
		L = XMLoadFloat3(&reset_L);
		R = XMLoadFloat3(&reset_R);
		U = XMLoadFloat3(&reset_U);
	}
	epsilon = XMVectorSet(0.01f, 0.01f, 0.01f, 0.0f);
	// default 1fps = 1/60s
	float time_ratio = follow_reset_timer/follow_reset_cd_max;
	float factor = app->m_Timer.delta_time()*FPS60*(0.05f+0.25f*time_ratio);
	R_old = app->m_Cam.get_RightXM();
	U_old = app->m_Cam.get_UpXM();
	L_old = app->m_Cam.get_LookXM();
	if (XMVector3NearEqual(R_old, R, epsilon) &&
		XMVector3NearEqual(U_old, U, epsilon) &&
		XMVector3NearEqual(L_old, L, epsilon)) {
		//
		follow_reset_timer = -2.1f;
		factor = 1.0f;
		is_resetting = false;
	}
	R = XMVectorLerp(R_old, R, factor);
	U = XMVectorLerp(U_old, U, factor);
	L = XMVectorLerp(L_old, L, factor);
	//
	XMStoreFloat3(&app->m_Cam.m_Look, L);
	XMStoreFloat3(&app->m_Cam.m_Right, R);
	XMStoreFloat3(&app->m_Cam.m_Up, U);	
}
//
template <typename T_app>
bool control_cam<T_app>::is_restrict()
{
	if (app->m_Cam.m_Preset != 0) return true;
	return false;
}
//
template <typename T_app>
void control_cam<T_app>::rebuild_setting()
{
	if (is_restrict()) follow_walk = follow_walk_min;
	else follow_walk = follow_walk_def;
	follow_up = follow_up_def_value();
}
//
template <typename T_app>
float control_cam<T_app>::follow_up_def_value()
{
	if (is_restrict()) return follow_up_def2;
	else return follow_up_def1;
}
//
}
#endif