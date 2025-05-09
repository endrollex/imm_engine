////////////////
// control_xinput.h
// by Huang Yiting
////////////////
////////////////
#ifndef CONTROL_XINPUT_H
#define CONTROL_XINPUT_H
#include "control_key_define.h"
#include <cmath>
namespace imm
{
////////////////
// control_xinput
////////////////
////////////////
struct control_xinput
{
	control_xinput();
	bool is_enable();
	bool is_L_active();
	bool is_R_active();
	bool is_RT_press();
	bool is_LT_press();
	bool is_LB_press();
	bool is_on_keydown(WORD &vkey);
	float L_radians();
	float R_radians();
	bool is_force_disable;
	bool is_LB_press_on;
	XINPUT_STATE state;
	XINPUT_KEYSTROKE key;
	float deadzone;
	int user_index;
};
//
control_xinput::control_xinput():
	is_force_disable(false),
	is_LB_press_on(false),
	deadzone(5000.0f),
	user_index(0)
{
	ZeroMemory(&state, sizeof(XINPUT_STATE));
	key.UserIndex = static_cast<BYTE>(user_index);
}
//
bool control_xinput::is_enable()
{
	if (is_force_disable) return false;
	DWORD dw_result;
	for (int ix = 0; ix != 4; ++ix) {
		dw_result = XInputGetState(ix, &state);
		if (dw_result == ERROR_SUCCESS) {
			user_index = ix;
			return true;
		}
	}
	if (dw_result != ERROR_SUCCESS) {
		user_index = 0;
		return false;
	}
	return false;
}
//
bool control_xinput::is_L_active()
{
	float LX = state.Gamepad.sThumbLX;
	float LY = state.Gamepad.sThumbLY;
	float magnitude = sqrt(LX*LX + LY*LY);
	if (magnitude > deadzone) return true;
	return false;
}
//
bool control_xinput::is_R_active()
{
	float RX = state.Gamepad.sThumbRX;
	float RY = state.Gamepad.sThumbRY;
	float magnitude = sqrt(RX*RX + RY*RY);
	if (magnitude > deadzone) return true;
	return false;
}
//
bool control_xinput::is_RT_press()
{
	return (state.Gamepad.bRightTrigger > 50);
}
//
bool control_xinput::is_LT_press()
{
	return (state.Gamepad.bLeftTrigger > 50);
}
//
bool control_xinput::is_LB_press()
{
	return is_LB_press_on;
}
//
bool control_xinput::is_on_keydown(WORD &vkey)
{
	if (XInputGetKeystroke(user_index, 0, &key) == ERROR_SUCCESS) {
		if (key.Flags & XINPUT_KEYSTROKE_KEYUP) {
			if (key.VirtualKey == VK_PAD_LSHOULDER) is_LB_press_on = false;
		}
		if (key.Flags & XINPUT_KEYSTROKE_KEYDOWN) {
			vkey = key.VirtualKey;
			if (key.VirtualKey == VK_PAD_LSHOULDER) is_LB_press_on = true;
			return true;
		}
	}
	return false;
}
//
float control_xinput::L_radians()
{
	float LX = state.Gamepad.sThumbLX;
	float LY = state.Gamepad.sThumbLY;
	return atan2(LX, LY);
}
//
float control_xinput::R_radians()
{
	float RX = state.Gamepad.sThumbRX;
	float RY = state.Gamepad.sThumbRY;
	return atan2(RX, RY);
}
//
}
#endif