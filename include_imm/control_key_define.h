////////////////
// control_key_define.h
// by Huang Yiting
////////////////
////////////////
#ifndef CONTROL_KEY_DEFINE_H
#define CONTROL_KEY_DEFINE_H
// include "Windows.h" before "XInput.h"
#include "Windows.h"
#include "XInput.h"
namespace imm
{
////////////////
// keyborad static
//
// two methods:
// GetKeyState                (in function suffix _update / combination key)
// WM_KEYDOWN / WM_SYSKEYDOWN (in function suffix _keydown)
//
// Virtual-Key Codes:
// https://msdn.microsoft.com/en-us/library/windows/desktop/dd375731(v=vs.85).aspx
////////////////
////////////////
static int KEY_P1_JUMP;
static int KEY_P1_WALK_RUN;
static int KEY_P1_W;
static int KEY_P1_A;
static int KEY_P1_S;
static int KEY_P1_D;
static int KEY_P1_ROLL;
static int KEY_P1_GUARD;
static int KEY_UI_ESC;
static int KEY_UI_FPS1;
static int KEY_UI_FPS2;
static int KEY_UI_DOWN1;
static int KEY_UI_DOWN2;
static int KEY_UI_UP1;
static int KEY_UI_UP2;
static int KEY_UI_ENTER;
static int KEY_UI_PAGEDOWN;
static int KEY_UI_PAGEUP;
static int KEY_CAM_FREE_LEFT;
static int KEY_CAM_FREE_RIGHT;
static int KEY_CAM_FREE_UP;
static int KEY_CAM_FREE_DOWN;
static int KEY_CAM_FOLLOW_RESET;
static int KEY_CMD_ACT1;
static int KEY_CMD_ACT2;
////////////////
// mouse static
////////////////
////////////////
// CONTORL_MOVE_BY_MOUSE
static int MOUSE_CAM_MOVE;
static int MOUSE_P1_PICK;
static int MOUSE_P1_MOVE;
static int MOUSE_UI_PICK;
// CONTORL_MOVE_BY_WASD
static int MOUSE_P1_ATK_X;
static int MOUSE_P1_ATK_Y;
////////////////
// pad static
//
// two methods:
// XINPUT_STATE structure     (in function suffix _update)
// XINPUT_KEYSTROKE structure (in function suffix _down)
////////////////
////////////////
static int PAD_P1_JUMP;
static int PAD_P1_ATK_X;
static int PAD_P1_ATK_Y;
static int PAD_P1_ROLL;
static int PAD_P1_GUARD;
static int PAD_CAM_FOLLOW_RESET;
static int PAD_UI_MENU;
static int PAD_UI_DWON1;
static int PAD_UI_DWON2;
static int PAD_UI_UP1;
static int PAD_UI_UP2;
static int PAD_UI_DEACTIVATE;
static int PAD_UI_APPLY;
static int XGPAD_CAM_FREE_UP;
static int XGPAD_CAM_FREE_DOWN;
static int XGPAD_CAM_FREE_LEFT;
static int XGPAD_CAM_FREE_RIGHT;
static int XGPAD_CAM_FREE_FORWARD;
static int XGPAD_CAM_FREE_BACKWARD;
static int XGPAD_CAM_FOLLOW_FORWARD;
static int XGPAD_CAM_FOLLOW_BACKWARD;
////////////////
// control coefficient
////////////////
////////////////
static float COEF_MOUSE_MOVE  = 1.0f;
static float COEF_MOUSE_WHEEL = 1.0f;
static float COEF_PAD_THUMB   = 1.0f;
////////////////
// MOUSE_MODEL
////////////////
////////////////
enum MOUSE_MODEL
{
	CAM_MOVE_BY_BUTTON,
	HIDDEN_CURSOR,
};
//
////////////////
// input_setting
////////////////
////////////////
struct input_setting
{
	input_setting();
	int user_index;
	void reset();
};
//
input_setting::input_setting():
	user_index(0)
{
	reset();
}
void input_setting::reset()
{
	//
	KEY_P1_JUMP          = 'C';
	KEY_P1_WALK_RUN      = 'J';
	KEY_P1_W             = 'W';
	KEY_P1_A             = 'A';
	KEY_P1_S             = 'S';
	KEY_P1_D             = 'D';
	KEY_P1_ROLL          = VK_SPACE;
	KEY_P1_GUARD         = 'E';
	KEY_UI_ESC           = VK_ESCAPE;
	KEY_UI_FPS1          = 'R';
	KEY_UI_FPS2          = VK_CONTROL;
	KEY_UI_DOWN1         = VK_DOWN;
	KEY_UI_DOWN2         = VK_RIGHT;
	KEY_UI_UP1           = VK_UP;
	KEY_UI_UP2           = VK_LEFT;
	KEY_UI_ENTER         = VK_RETURN;
	KEY_UI_PAGEDOWN      = VK_NEXT;
	KEY_UI_PAGEUP        = VK_PRIOR;
	KEY_CAM_FREE_LEFT    = 'A';
	KEY_CAM_FREE_RIGHT   = 'D';
	KEY_CAM_FREE_UP      = 'W';
	KEY_CAM_FREE_DOWN    = 'S';
	KEY_CAM_FOLLOW_RESET = 'Z';
	KEY_CMD_ACT1         = VK_DELETE;
	KEY_CMD_ACT2         = VK_MENU;	
	//
	MOUSE_CAM_MOVE = MK_MBUTTON;
	MOUSE_P1_PICK  = MK_RBUTTON;
	MOUSE_P1_MOVE  = MK_LBUTTON;
	MOUSE_UI_PICK  = MK_LBUTTON;
	//
	MOUSE_P1_ATK_X = MK_LBUTTON;
	MOUSE_P1_ATK_Y = MK_RBUTTON;
	//
	PAD_P1_JUMP               = VK_PAD_A;
	PAD_P1_ATK_X              = VK_PAD_X;
	PAD_P1_ATK_Y              = VK_PAD_Y;
	PAD_P1_ROLL               = VK_PAD_LSHOULDER;
	PAD_P1_GUARD              = VK_PAD_LSHOULDER;
	PAD_CAM_FOLLOW_RESET      = VK_PAD_RSHOULDER;
	PAD_UI_MENU               = VK_PAD_START;
	PAD_UI_DWON1              = VK_PAD_DPAD_DOWN;
	PAD_UI_DWON2              = VK_PAD_DPAD_RIGHT;
	PAD_UI_UP1                = VK_PAD_DPAD_UP;
	PAD_UI_UP2                = VK_PAD_DPAD_LEFT;
	PAD_UI_DEACTIVATE         = VK_PAD_B;
	PAD_UI_APPLY              = VK_PAD_A;
	XGPAD_CAM_FREE_UP         = XINPUT_GAMEPAD_DPAD_UP;
	XGPAD_CAM_FREE_DOWN       = XINPUT_GAMEPAD_DPAD_DOWN;
	XGPAD_CAM_FREE_LEFT       = XINPUT_GAMEPAD_DPAD_LEFT;
	XGPAD_CAM_FREE_RIGHT      = XINPUT_GAMEPAD_DPAD_RIGHT;
	XGPAD_CAM_FREE_FORWARD    = XINPUT_GAMEPAD_LEFT_SHOULDER;
	XGPAD_CAM_FREE_BACKWARD   = XINPUT_GAMEPAD_RIGHT_SHOULDER;
	XGPAD_CAM_FOLLOW_FORWARD  = XINPUT_GAMEPAD_LEFT_THUMB;
	XGPAD_CAM_FOLLOW_BACKWARD = XINPUT_GAMEPAD_RIGHT_THUMB;	
}
//
}
#endif