////////////////
// imm_cmd.h
// by Huang Yiting
////////////////
////////////////
#ifndef IMM_CMD_H
#define IMM_CMD_H
#include "imm_cmd_util.h"
#include "ui_dwrite.h"
#include "ui_sprite.h"
#include "control_key_define.h"
#include <DirectXColors.h>
namespace imm
{
////////////////
// cmd_shell
////////////////
////////////////
template <typename T_app>
struct cmd_shell
{
	cmd_shell();
	std::atomic<bool> is_active;
	std::atomic<bool> is_slient;
	std::atomic<bool> is_busying;
	std::atomic<bool> is_preparing;
	std::atomic<bool> is_wait_loading;
	std::atomic<bool> is_draw_loading_1frame;
	FLOAT margin_factor;
	float font_factor;
	float loading_time_min;
	std::wstring cmd;
	atomic_wstring input;
	atomic_wstring input_loading;
	XMVECTORF32 back_color;
	T_app *app;
	dwrite_simple dwrite;
	dwrite_simple dwrite_loading;
	sprite_simple sprite_loading;
	std::map<std::string, size_t> sprite_rect_map;
	std::vector<ui_rect> sprite_rect;
	std::map<std::string, std::string> sprite_name_map;
	void init(T_app *app_in);
	void sprite_build_buffer();
	void on_input_char(WPARAM &w_param, LPARAM &l_param);
	void on_input_keydown(WPARAM &w_param, LPARAM &l_param);
	void apply();
	void on_resize();
	void on_resize_sprite();
	void draw_d2d();
	void draw_d2d_loading();
	void draw_d3d_loading();
	void do_slient_on();
	void ascii_loading_predefined();
	void change_loading_font_factor(const float &font_f);
	bool is_waiting_for_something();
};
//
template <typename T_app>
cmd_shell<T_app>::cmd_shell():
	is_active(false),
	is_slient(true),
	is_busying(false),
	is_preparing(true),
	is_wait_loading(false),
	is_draw_loading_1frame(false),
	margin_factor(0.018f),
	font_factor(16.0f),
	loading_time_min(1.0f),
	cmd(),
	input(),
	input_loading(),
	back_color(Colors::DarkBlue),
	app(nullptr),
	dwrite()
{
	;
}
//
template <typename T_app>
void cmd_shell<T_app>::init(T_app *app_in)
{
	app = app_in;
	dwrite.init_member_rect(app->m_D2DDC, app->m_hwnd, L"Consolas", margin_factor, font_factor, DWRITE_ALIG_STYLE_CMD);
	dwrite_loading.init_member_rect(app->m_D2DDC, app->m_hwnd, L"Consolas", margin_factor, font_factor, DWRITE_ALIG_STYLE_CENTER);
	sprite_loading.init(app->m_D3DDevice, app->m_D3DDC);
	sprite_build_buffer();
	ascii_loading_predefined();
}
//
template <typename T_app>
void cmd_shell<T_app>::sprite_build_buffer()
{
	sprite_name_map["loading"] = "";
	sprite_rect_map["loading"] = 0;
	//
	std::string concrete = IMM_PATH["script"]+"concrete_common.lua";
	lua_reader l_reader;
	l_reader.loadfile(concrete);
	std::vector<std::vector<std::string>> vec2d;
	l_reader.vec2d_str_from_table("csv_avatar", vec2d);
	vec2d.erase(vec2d.begin());
	std::vector<std::vector<std::string>> loading_dds;
	for (size_t ix = 0; ix != vec2d.size(); ++ix) {
		if (vec2d[ix][0] == "loading") {
			loading_dds.push_back(vec2d[ix]);
			sprite_name_map["loading"] = vec2d[ix][0];
			break;
		}
	}
	sprite_loading.build_buffer(loading_dds);
	//
	sprite_rect.emplace_back();
	sprite_rect.back().id_str = "loading";
	sprite_rect.back().parent_str = "-1";
	sprite_rect.back().group = "loading";
	sprite_rect.back().tp = ui_rect::type::sprite;
	sprite_rect.back().brush_sel = {""};
	sprite_rect.back().text = L"";
	sprite_rect.back().dwrite_ix = "";
	sprite_rect.back().margin = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
	//
	sprite_rect.back().active = true;
}
//
template <typename T_app>
void cmd_shell<T_app>::on_input_char(WPARAM &w_param, LPARAM &l_param)
{
	if (!is_active) return;
	DUMMY(l_param);
	if (w_param != VK_BACK) {
		input += (wchar_t)w_param;
		if (w_param == VK_RETURN) apply();
		else cmd += (wchar_t)w_param;
	}
	else {
		if (input.size() > 0) input.pop_back();
		if (cmd.size() > 0) cmd.pop_back();
	}
}
template <typename T_app>
void cmd_shell<T_app>::on_input_keydown(WPARAM &w_param, LPARAM &l_param)
{
	DUMMY(l_param);
	if (w_param == KEY_CMD_ACT1 && GetKeyState(KEY_CMD_ACT2)) {
		is_active = !is_active;
	}
}
//
template <typename T_app>
void cmd_shell<T_app>::apply()
{
	std::wstring cmd_get(cmd);
	cmd.clear();
	if (cmd_get == L"exit") {
		DestroyWindow(app->m_hwnd);
		SendMessage(app->m_hwnd, WM_DESTROY, 0, 0);
		return;
	}
	if (cmd_get == L"help") {
		input += L"\n> about                About this program.";
		input += L"\n> cls                  Clears the screen.";
		input += L"\n> color                Sets the console font and background colors.";
		input += L"\n> exit                 Quits this program.";
		input += L"\n> help                 Prints command briefs.";
		input += L"\n> slient               Switches between slient and normal.";
		input += L"\n> reload               Reload scene.";
		input += L"\n> wire                 Draw BoundingBox wireframe or not.";
		input += L"\n> role                 Select an instance to play.";
		input += L"\n";
		return;
	}
	if (cmd_get == L"cls") {
		input.clear();
		return;
	}
	if (cmd_get == L"about") {
		input += L"\n> immature engine Demo [Development testing 001]";
		input += L"\n> Copyright 2015-2017 Huang Yiting (http://endrollex.com)";
		input += L"\n";
		return;
	}
	if (cmd_get.substr(0, 5) == L"color") {
		if (cmd_get.size() < 7) {
			input += L"\n> Specify following command to sets a color:";
			input += L"\n> color 0              Font White";
			input += L"\n> color 1              Font Black";
			input += L"\n> color 2              Font Purple";
			input += L"\n> color 3              Font Yellow";
			input += L"\n> color a              Background Blue";
			input += L"\n> color b              Background Black";
			input += L"\n";
			return;
		}
		if (cmd_get == L"color 0") {
			dwrite.set_Brush(app->m_D2DDC, D2D1::ColorF::White);
			return;
		}
		if (cmd_get == L"color 1") {
			dwrite.set_Brush(app->m_D2DDC, D2D1::ColorF::Black);
			return;
		}
		if (cmd_get == L"color 2") {
			dwrite.set_Brush(app->m_D2DDC, D2D1::ColorF::Purple);
			return;
		}
		if (cmd_get == L"color 3") {
			dwrite.set_Brush(app->m_D2DDC, D2D1::ColorF::Yellow);
			return;
		}
		if (cmd_get == L"color a") {
			back_color = Colors::Blue;
			return;
		}
		if (cmd_get == L"color b") {
			back_color = Colors::Black;
			return;
		}
	}
	if (cmd_get == L"slient") {
		is_slient = !is_slient;
		if (is_slient) do_slient_on();
		else is_active = false;
		return;
	}
	if (cmd_get.substr(0, 6) == L"reload") {
		if (cmd_get.size() < 8) {
			input += L"\n> Usage: reload [scene_index]";
			input += L"\n> scene_index is a string indicate which scene.";
			input += L"\n";
			return;
		}
		std::wstring scene_ix = cmd_get.substr(7);
		std::string concrete = IMM_PATH["script"]+"scene"+txtutil::wstr_to_str(scene_ix)+"\\concrete_instance.lua";
		if (data_is_file_exist(concrete)) {
			app->m_Scene.reload(scene_ix);
			is_slient = false;
			is_active = false;
		}
		else {
			input += L"\n> Scene "+scene_ix+L" not found.";
			input += L"\n";
		}
		return;
	}
	if (cmd_get == L"wire") {
		app->m_Scene.phy_wire.switch_drawing();
		std::wstring info_mes = L"off.";
		if (app->m_Scene.phy_wire.is_drawing) info_mes = L"on, notice drawing AABB is wrong.";
		input += L"\n> Draw bounding wireframe is "+info_mes;
		input += L"\n";
	}
	if (cmd_get.substr(0, 4) == L"role") {
		if (cmd_get.size() < 6) {
			input += L"\n> Usage: role [instance_name]";
			input += L"\n";
			return;
		}
		std::wstring inst_name_w = cmd_get.substr(5);
		std::string inst_name = txtutil::wstr_to_str(inst_name_w);
		if (app->m_Inst.m_NameMap.count(inst_name)) {
			size_t inst_ix = app->m_Inst.m_NameMap[inst_name];
			if (inst_ix == app->m_Control.player1) {
				input += L"\n> Role not change.";
				input += L"\n";
				return;
			}
			app->m_Control.set_player1(inst_ix);
			input += L"\n> Player1 = "+inst_name_w+L".";
			input += L"\n";
		}
		else {
			input += L"\n> Instance "+inst_name_w+L" not found.";
			input += L"\n";
		}
	}
	if (cmd_get == L"dump1") {
		bool is_ok = app->m_Inst.m_Build.inst_dump();
		if (is_ok) {
			input += L"\nOK";
			input += L"\n";
		}
		else {
			input += L"\nFailed to open the file.";
			input += L"\n";
		}
	}
}
//
template <typename T_app>
void cmd_shell<T_app>::on_resize()
{
	if (app == nullptr) return;
	dwrite.on_resize_CreateTextFormat(app->m_hwnd);
	dwrite.on_resize_LayoutRc(app->m_hwnd, margin_factor);
	dwrite_loading.on_resize_CreateTextFormat(app->m_hwnd);
	float left_offset = 0.3f;
	if (app->m_Scene.is_show_logo) left_offset = 0.0f;
	dwrite_loading.on_resize_LayoutRc(app->m_hwnd, margin_factor, left_offset);
	on_resize_sprite();
}
//
template <typename T_app>
void cmd_shell<T_app>::on_resize_sprite()
{
	RECT get_rect;
	GetClientRect(app->m_hwnd, &get_rect);
	float scr_height = static_cast<float>(get_rect.bottom - get_rect.top);
	float scr_width = static_cast<float>(get_rect.right - get_rect.left);
	float scale = scr_height/UI_RESOLUTION_HEIGHT*0.6f;
	float left_offset = math::calc_lerp(0.0f, 0.2f, (scr_width/scr_height-1.33f)/1.01f);
	for (auto it = sprite_loading.map_height.begin(); it != sprite_loading.map_height.end(); ++it) {
		sprite_loading.map_pos[it->first] = XMFLOAT2(scr_width*left_offset*scale, (scr_height-(it->second*scale))*0.5f);
	}
	sprite_loading.on_resize(scale);
}
//
template <typename T_app>
void cmd_shell<T_app>::draw_d2d()
{
	if (app->m_UiMgr.is_not_draw_response) {
		input.clear();
		input += L"Screen aspect ratio error.";
	}
	dwrite.draw_MemberRect(app->m_D2DDC, input);
}
//
template <typename T_app>
void cmd_shell<T_app>::draw_d2d_loading()
{
	dwrite_loading.draw_MemberRect(app->m_D2DDC, input_loading);
}
//
template <typename T_app>
void cmd_shell<T_app>::draw_d3d_loading()
{
	if (app->m_Scene.is_show_logo) return;
	sprite_loading.draw_d3d(sprite_rect_map, sprite_rect, sprite_name_map);
}
//
template <typename T_app>
void cmd_shell<T_app>::do_slient_on()
{
	is_active = true;
	app->m_UiMgr.define_deactivate_all_cmd_slient();
}
//
template <typename T_app>
void cmd_shell<T_app>::ascii_loading_predefined()
{
	std::wstring predefined;
	predefined = L"immature engine\n";
	predefined += L"  _                           _                                     _             `\n";
	predefined += L" (_)                         | |                                   (_)            `\n";
	predefined += L"  _ _ __ ___  _ __ ___   __ _| |_ _   _ _ __ ___    ___ _ __   __ _ _ _ __   ___  `\n";
	predefined += L" | | '_ ` _ \\| '_ ` _ \\ / _` | __| | | | '__/ _ \\  / _ \\ '_ \\ / _` | | '_ \\ / _ \\ `\n";
	predefined += L" | | | | | | | | | | | | (_| | |_| |_| | | |  __/ |  __/ | | | (_| | | | | |  __/ `\n";
	predefined += L" |_|_| |_| |_|_| |_| |_|\\__,_|\\__|\\__,_|_|  \\___|  \\___|_| |_|\\__, |_|_| |_|\\___| `\n";
	predefined += L"                                                               __/ |              `\n";
	predefined += L"                                                              |___/               `\n";
	// http://patorjk.com/software/taag/#p=display&f=Big&t=immature%20engine
	input_loading.assign(predefined);
}
//
template <typename T_app>
void cmd_shell<T_app>::change_loading_font_factor(const float &font_f)
{
	dwrite_loading.m_FontFactor = font_f;
	on_resize();
}
//
template <typename T_app>
bool cmd_shell<T_app>::is_waiting_for_something()
{
	if (is_slient || is_preparing || is_wait_loading) return true;
	if (app->m_UiMgr.is_not_draw_response) return true;
	return false;
}
//
}
#endif