////////////////
// ui_def_welcome.h
// by Huang Yiting
////////////////
////////////////
#ifndef UI_DEF_WELCOME_H
#define UI_DEF_WELCOME_H
#include "ui_base.h"
namespace imm
{
////////////////
// ui_def_welcome
////////////////
////////////////
template <class T_app>
struct ui_def_welcome: public ui_base<T_app>
{
	ui_def_welcome() {;}
	~ui_def_welcome() {;}
	void define_style();
	bool define_apply_ix_if(int &index);
	bool define_on_input_keydown(WPARAM &w_param, LPARAM &l_param);
	bool define_on_pad_keydown(const WORD &vkey);
	void define_update(float dt);
	void define_deactivate_all_default();
	void define_deactivate_all_cmd_slient();
	void define_enter_and_exit() {;}
	void define_sprite_build_buffer();
	void define_on_resize_sprite();
	void define_text();
};
//
template <typename T_app>
void ui_def_welcome<T_app>::define_style()
{
	m_TitleFontFactor = 32.0f;
	m_Dwrite["32"].init_external_rect(m_App->m_D2DDC, m_App->m_hwnd, L"Consolas", 32.0f, DWRITE_ALIG_STYLE_CENTER);
	m_Dwrite["24"].init_external_rect(m_App->m_D2DDC, m_App->m_hwnd, L"Consolas", 24.0f, DWRITE_ALIG_STYLE_CENTER);
	m_Dwrite["22_page"].init_external_rect(m_App->m_D2DDC, m_App->m_hwnd, L"Consolas", 22.0f, DWRITE_ALIG_STYLE_PAGE);
	// brush
	m_Brush["black"];
	set_Brush(D2D1::ColorF::Black, 0.5f, "black");
	m_Brush["transparent"];
	set_Brush(D2D1::ColorF::Black, 0.0f, "transparent");
	m_Brush["darkgray"];
	set_Brush(D2D1::ColorF::DarkGray, 0.5f, "darkgray");
	m_Brush["orange"];
	set_Brush(D2D1::ColorF::Orange, 0.9f, "orange");
	////////////////
	// entrance
	////////////////
	////////////////
	m_Rect.emplace_back();
	m_Rect.back().id_str = "entrance_start";
	m_Rect.back().parent_str = "-1";
	m_Rect.back().group = "entrance";
	m_Rect.back().tp = ui_rect::type::button;
	m_Rect.back().brush_sel = {"transparent"};
	m_Rect.back().text = L"Start Game";
	m_Rect.back().dwrite_ix = "32";
	m_Rect.back().margin = XMFLOAT4(0.0f, 0.5f, 0.0f, 0.42f);
	//
	m_Rect.emplace_back();
	m_Rect.back().id_str = "entrance_options";
	m_Rect.back().parent_str = "-1";
	m_Rect.back().group = "entrance";
	m_Rect.back().tp = ui_rect::type::button;
	m_Rect.back().brush_sel = {"transparent"};
	m_Rect.back().text = L"Options";
	m_Rect.back().dwrite_ix = "32";
	m_Rect.back().margin = XMFLOAT4(0.0f, 0.58f, 0.0f, 0.34f);
	//
	m_Rect.emplace_back();
	m_Rect.back().id_str = "entrance_credit";
	m_Rect.back().parent_str = "-1";
	m_Rect.back().group = "entrance";
	m_Rect.back().tp = ui_rect::type::button;
	m_Rect.back().brush_sel = {"transparent"};
	m_Rect.back().text = L"Credit";
	m_Rect.back().dwrite_ix = "32";
	m_Rect.back().margin = XMFLOAT4(0.0f, 0.66f, 0.0f, 0.26f);
	//
	m_Rect.emplace_back();
	m_Rect.back().id_str = "entrance_about";
	m_Rect.back().parent_str = "-1";
	m_Rect.back().group = "entrance";
	m_Rect.back().tp = ui_rect::type::button;
	m_Rect.back().brush_sel = {"transparent"};
	m_Rect.back().text = L"About";
	m_Rect.back().dwrite_ix = "32";
	m_Rect.back().margin = XMFLOAT4(0.0f, 0.74f, 0.0f, 0.18f);
	//
	m_Rect.emplace_back();
	m_Rect.back().id_str = "entrance_exit";
	m_Rect.back().parent_str = "-1";
	m_Rect.back().group = "entrance";
	m_Rect.back().tp = ui_rect::type::button;
	m_Rect.back().brush_sel = {"transparent"};
	m_Rect.back().text = L"Exit Game";
	m_Rect.back().dwrite_ix = "32";
	m_Rect.back().margin = XMFLOAT4(0.0f, 0.82f, 0.0f, 0.10f);
	////////////////
	// credit
	////////////////
	////////////////
	m_Rect.emplace_back();
	m_Rect.back().id_str = "credit_backg";
	m_Rect.back().parent_str = "-1";
	m_Rect.back().group = "credit";
	m_Rect.back().tp = ui_rect::type::background;
	m_Rect.back().brush_sel = {"black"};
	m_Rect.back().text = L"";
	m_Rect.back().dwrite_ix = "32";
	m_Rect.back().margin = XMFLOAT4(0.1f, 0.0f, 0.1f, 0.0f);
	//
	m_Rect.emplace_back();
	m_Rect.back().id_str = "credit_close";
	m_Rect.back().parent_str = "-1";
	m_Rect.back().group = "credit";
	m_Rect.back().tp = ui_rect::type::button;
	m_Rect.back().brush_sel = {"darkgray"};
	m_Rect.back().text = L"Close";
	m_Rect.back().dwrite_ix = "24";
	m_Rect.back().margin = XMFLOAT4(0.8f, 0.84f, 0.05f, 0.08f);
	//
	m_Rect.emplace_back();
	m_Rect.back().id_str = "credit_content";
	m_Rect.back().parent_str = "-1";
	m_Rect.back().group = "credit";
	m_Rect.back().tp = ui_rect::type::text_layout;
	m_Rect.back().brush_sel = {"black"};
	m_Rect.back().text = get_DefineTxt("credit");
	m_Rect.back().dwrite_ix = "22_page";
	m_Rect.back().margin = XMFLOAT4(0.15f, 0.0f, 0.15f, 0.0f);
	////////////////
	// about
	////////////////
	////////////////
	m_Rect.emplace_back();
	m_Rect.back().id_str = "about_backg";
	m_Rect.back().parent_str = "-1";
	m_Rect.back().group = "about";
	m_Rect.back().tp = ui_rect::type::background;
	m_Rect.back().brush_sel = {"black"};
	m_Rect.back().text = L"";
	m_Rect.back().dwrite_ix = "32";
	m_Rect.back().margin = XMFLOAT4(0.1f, 0.0f, 0.1f, 0.0f);
	//
	m_Rect.emplace_back();
	m_Rect.back().id_str = "about_close";
	m_Rect.back().parent_str = "-1";
	m_Rect.back().group = "about";
	m_Rect.back().tp = ui_rect::type::button;
	m_Rect.back().brush_sel = {"darkgray"};
	m_Rect.back().text = L"Close";
	m_Rect.back().dwrite_ix = "24";
	m_Rect.back().margin = XMFLOAT4(0.8f, 0.84f, 0.05f, 0.08f);
	//
	m_Rect.emplace_back();
	m_Rect.back().id_str = "about_content";
	m_Rect.back().parent_str = "-1";
	m_Rect.back().group = "about";
	m_Rect.back().tp = ui_rect::type::text_layout;
	m_Rect.back().brush_sel = {"black"};
	m_Rect.back().text = get_DefineTxt("about");
	m_Rect.back().dwrite_ix = "22_page";
	m_Rect.back().margin = XMFLOAT4(0.15f, 0.0f, 0.15f, 0.0f);
	////////////////
	// exit
	////////////////
	////////////////
	m_Rect.emplace_back();
	m_Rect.back().id_str = "exit_backg";
	m_Rect.back().parent_str = "-1";
	m_Rect.back().group = "exit";
	m_Rect.back().tp = ui_rect::type::background;
	m_Rect.back().brush_sel = {"black"};
	m_Rect.back().text = L"";
	m_Rect.back().dwrite_ix = "32";
	m_Rect.back().margin = XMFLOAT4(0.0f, 0.3f, 0.0f, 0.3f);
	//
	m_Rect.emplace_back();
	m_Rect.back().id_str = "exit_yes";
	m_Rect.back().parent_str = "-1";
	m_Rect.back().group = "exit";
	m_Rect.back().tp = ui_rect::type::button;
	m_Rect.back().brush_sel = {"darkgray"};
	m_Rect.back().text = L"Yes";
	m_Rect.back().dwrite_ix = "24";
	m_Rect.back().margin = XMFLOAT4(0.25f, 0.5f, 0.6f, 0.42f);
	//
	m_Rect.emplace_back();
	m_Rect.back().id_str = "exit_no";
	m_Rect.back().parent_str = "-1";
	m_Rect.back().group = "exit";
	m_Rect.back().tp = ui_rect::type::button;
	m_Rect.back().brush_sel = {"darkgray"};
	m_Rect.back().text = L"No";
	m_Rect.back().dwrite_ix = "24";
	m_Rect.back().margin = XMFLOAT4(0.6f, 0.5f, 0.25f, 0.42f);
	//
	m_Rect.emplace_back();
	m_Rect.back().id_str = "exit_title";
	m_Rect.back().parent_str = "-1";
	m_Rect.back().group = "exit";
	m_Rect.back().tp = ui_rect::type::text_pure;
	m_Rect.back().brush_sel = {"black"};
	m_Rect.back().text = L"Exit Game?";
	m_Rect.back().dwrite_ix = "32";
	m_Rect.back().margin = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.2f);
	////////////////
	//
	////////////////
	////////////////
	build_rect_map();
	//
	for (auto &rect: m_Rect) {
		rect.brush_ix = rect.brush_sel[0];
		if (rect.tp == ui_rect::type::button) {
			if (rect.group == "entrance") rect.brush_sel.emplace_back("black");
			else rect.brush_sel.emplace_back("orange");
		}
		rect.active = false;
	}
}
//
template <typename T_app>
bool ui_def_welcome<T_app>::define_apply_ix_if(int &index)
{
	// entrance
	if (index == m_MapID["entrance_start"]) {
		// avoid pad update case scene reload conflict
		if (!m_App->m_Cmd.is_preparing) m_App->m_Scene.reload(SCENE_SECOND);
		return true;
	}
	if (index == m_MapID["entrance_options"]) {
		return true;
	}
	if (index == m_MapID["entrance_credit"]) {
		group_active("entrance", false);
		group_active("credit", true);
		return true;
	}
	if (index == m_MapID["entrance_about"]) {
		group_active("entrance", false);
		group_active("about", true);
		return true;
	}
	if (index == m_MapID["entrance_exit"]) {
		group_active("entrance", false);
		group_active("exit", true);
		return true;
	}
	// credit
	if (index == m_MapID["credit_close"]) {
		group_active("credit", false);
		group_active("entrance", true);
		return true;
	}
	// about
	if (index == m_MapID["about_close"]) {
		group_active("about", false);
		group_active("entrance", true);
		return true;
	}
	// exit
	if (index == m_MapID["exit_yes"]) {
		DestroyWindow(m_App->m_hwnd);
		SendMessage(m_App->m_hwnd, WM_DESTROY, 0, 0);
		return true;
	}
	if (index == m_MapID["exit_no"]) {
		group_active("exit", false);
		group_active("entrance", true);
		return true;
	}
	return false;
}
//
template <typename T_app>
bool ui_def_welcome<T_app>::define_on_input_keydown(WPARAM &w_param, LPARAM &l_param)
{
	DUMMY(l_param);
	DUMMY(w_param);
	return false;
}
//
template <typename T_app>
bool ui_def_welcome<T_app>::define_on_pad_keydown(const WORD &vkey)
{
	DUMMY(vkey);
	return false;
}
//
template <typename T_app>
void ui_def_welcome<T_app>::define_update(float dt)
{
	DUMMY(dt);
}
//
template <typename T_app>
void ui_def_welcome<T_app>::define_deactivate_all_default()
{
	deactivate_all();
}
//
template <typename T_app>
void ui_def_welcome<T_app>::define_deactivate_all_cmd_slient()
{
	deactivate_all();
}
//
template <typename T_app>
void ui_def_welcome<T_app>::define_sprite_build_buffer()
{
	;
}
//
template <typename T_app>
void ui_def_welcome<T_app>::define_on_resize_sprite()
{
	;
}
//
template <typename T_app>
void ui_def_welcome<T_app>::define_text()
{
	std::map<std::string, std::string> get_welcome;
	std::string concrete = IMM_PATH["text"]+"welcome.lua";
	lua_reader l_reader;
	l_reader.loadfile(concrete);
	std::string var_name = "welcome";
	if (l_reader.is_nil(var_name)) {assert(false); abort();}
	l_reader.map_from_table(var_name, get_welcome);
	for (auto it = get_welcome.begin(); it != get_welcome.end(); ++it) {
		m_DefineTxt[it->first] = txtutil::str_to_wstr(it->second);
	}
}
//
}
#endif