////////////////
// ui_base.h
// by Huang Yiting
////////////////
////////////////
#ifndef UI_BASE_H
#define UI_BASE_H
#include "ui_text_help.h"
#include "ui_sprite.h"
#include "ui_dwrite.h"
#include "control_key_define.h"
#include <algorithm>
namespace imm
{
////////////////
// ui_base
////////////////
////////////////
template <typename T_app>
struct ui_base
{
	ui_base();
	virtual ~ui_base();
	void init(T_app *app_in);
	void reset();
	void build_rect_map();
	void set_Brush(D2D1::ColorF color, const float &opacity, const std::string index);
	void on_resize_RectFromHWND(const size_t &ix);
	void on_resize_RectFromRect(const size_t &ix, const int &parent);
	void on_resize_TextLayout(const size_t &ix, const bool &is_resize = false);
	PCWSTR get_DefineTxt(const std::string &name);
	void on_resize();
	void draw_d2d();
	void draw_d3d();
	bool on_mouse_down(WPARAM btn_state, const int &pos_x, const int &pos_y);
	bool on_mouse_wheel(const short &z_delta);
	void on_mouse_over(const int &pos_x, const int &pos_y);
	void mouse_pick(const int &pos_x, const int &pos_y);
	bool on_pad_keydown(const WORD &vkey);
	bool on_input_keydown(WPARAM &w_param, LPARAM &l_param);
	void group_active_switch(const std::string &name);
	void group_active(const std::string &name, const bool &is_act, const bool &is_switched = false, const bool &is_slient_click_once = false);
	void pad_loop_button(const bool &is_down, const std::string &select_none = "", const bool &is_slient_click_once = false);
	void pad_roll_text_layout(const bool &is_down);
	bool is_ui_appear();
	void deactivate_all();
	bool apply_ix(int &index, const bool &is_like_pad = false);
	bool is_waiting_for_something();
	// virtual
	virtual void define_style() = 0;
	virtual bool define_apply_ix_if(int &index) = 0;
	virtual bool define_on_input_keydown(WPARAM &w_param, LPARAM &l_param) = 0;
	virtual bool define_on_pad_keydown(const WORD &vkey) = 0;
	virtual void define_update(float dt) = 0;
	virtual void define_deactivate_all_default() = 0;
	virtual void define_deactivate_all_cmd_slient() = 0;
	virtual void define_enter_and_exit() = 0;
	virtual void define_sprite_build_buffer() {;}
	virtual void define_on_resize_sprite() {;}
	virtual void define_text() = 0;
	std::map<std::string, ID2D1SolidColorBrush*> m_Brush;
	std::vector<ui_rect> m_Rect;
	std::map<std::string, std::vector<size_t>> m_MapGroup;
	std::map<std::string, std::vector<size_t>> m_MapButton;
	std::map<std::string, std::vector<size_t>> m_MapTextLayout;
	std::map<std::string, size_t> m_MapSpriteRect;
	std::map<std::string, std::string> m_MapSpriteName;
	std::map<std::string, int> m_MapID;
	std::map<std::string, dwrite_simple> m_Dwrite;
	sprite_simple m_Sprite;
	int m_ClickIxMouse;
	int m_ClickIxPad;
	bool m_IsMute;
	bool m_IsOtherUIAppear;
	bool m_IsPadUsing;
	bool m_IsInitialized;
	bool m_IsSlientClickOnce;
	float m_LastUiActive;
	FLOAT m_TitleFontFactor;
	D2D1_POINT_2F m_TextLayoutOrigin;
	RECT m_RcHWND;
	// if a group has no button, it is no-clickable, otherwise it is clickable
	std::string m_ClickableActived;
	std::string m_ClickSound;
	std::map<std::string, std::wstring> m_DefineTxt;
	ui_text_chunk m_TxtChunk;
	T_app *m_App;
private:
	ui_base(const ui_base &rhs);
	ui_base &operator=(const ui_base &rhs);
};
//
template <typename T_app>
ui_base<T_app>::ui_base():
	m_ClickIxMouse(-1),
	m_ClickIxPad(-1),
	m_IsMute(false),
	m_IsOtherUIAppear(false),
	m_IsPadUsing(false),
	m_IsInitialized(false),
	m_IsSlientClickOnce(false),
	m_LastUiActive(0.0f),
	m_TitleFontFactor(32.0f),
	m_RcHWND(),
	m_ClickableActived("none"),
	m_ClickSound(sfx::Click),
	m_TxtChunk(),
	m_App(0)
{
	m_TextLayoutOrigin.x = 0.0f;
	m_TextLayoutOrigin.y = 0.0f;
}
//
template <typename T_app>
ui_base<T_app>::~ui_base()
{
	for (auto &brush: m_Brush) RELEASE_COM(brush.second);
}
//
template <typename T_app>
void ui_base<T_app>::init(T_app *app_in)
{
	m_App = app_in;
	m_Sprite.init(m_App->m_D3DDevice, m_App->m_D3DDC);
	//
	define_text();
	define_style();
	define_sprite_build_buffer();
	m_IsInitialized = true;
	on_resize();
}
//
template <typename T_app>
void ui_base<T_app>::reset()
{
	m_TxtChunk.reset();
	if (!m_App->m_Scene.audio.map_effect_bank.count(m_ClickSound)) (m_ClickSound = sfx::Empty);
}
//
template <typename T_app>
void ui_base<T_app>::build_rect_map()
{
	// map rect id_str for search parent
	// -1 stands for root
	m_MapID["-1"] = -1;
	for (size_t ix = 0; ix != m_Rect.size(); ++ix) {
		m_MapID[m_Rect[ix].id_str] = static_cast<int>(ix);
	}
	// map rect accroding group name
	for (size_t ix = 0; ix != m_Rect.size(); ++ix) {
		m_Rect[ix].parent = m_MapID[m_Rect[ix].parent_str];
		m_MapGroup[m_Rect[ix].group].emplace_back(ix);
		if (m_Rect[ix].tp == ui_rect::type::button) m_MapButton[m_Rect[ix].group].emplace_back(ix);
		if (m_Rect[ix].tp == ui_rect::type::text_layout) {
			m_MapTextLayout[m_Rect[ix].group].emplace_back(ix);
			on_resize_TextLayout(ix);
		}
		if (m_Rect[ix].tp == ui_rect::type::sprite) m_MapSpriteRect[m_Rect[ix].id_str] = ix;
	}
}
//
template <typename T_app>
void ui_base<T_app>::on_resize_RectFromHWND(const size_t &ix)
{
	LONG height = m_RcHWND.bottom - m_RcHWND.top;
	LONG width = m_RcHWND.right - m_RcHWND.left;
	m_Rect[ix].rc = D2D1::RectF(
		m_RcHWND.left + m_Rect[ix].margin.x*width,
		m_RcHWND.top + m_Rect[ix].margin.y*height,
		m_RcHWND.right - m_Rect[ix].margin.z*width,
		m_RcHWND.bottom - m_Rect[ix].margin.w*height);
}
//
template <typename T_app>
void ui_base<T_app>::on_resize_RectFromRect(const size_t &ix, const int &parent)
{
	assert(parent >= 0);
	LONG height = static_cast<LONG>(m_Rect[parent].rc.bottom - m_Rect[parent].rc.top);
	LONG width = static_cast<LONG>(m_Rect[parent].rc.right - m_Rect[parent].rc.left);
	m_Rect[ix].rc = D2D1::RectF(
		m_Rect[parent].rc.left + m_Rect[ix].margin.x*width,
		m_Rect[parent].rc.top + m_Rect[ix].margin.y*height,
		m_Rect[parent].rc.right - m_Rect[ix].margin.z*width,
		m_Rect[parent].rc.bottom - m_Rect[ix].margin.w*height);
}
//
template <typename T_app>
void ui_base<T_app>::on_resize_TextLayout(const size_t &ix, const bool &is_resize)
{
	float height = static_cast<float>(m_RcHWND.bottom - m_RcHWND.top);
	float width = static_cast<float>(m_RcHWND.right - m_RcHWND.left);
	float txt_width = (1.0f-m_Rect[ix].margin.x-m_Rect[ix].margin.z)*width;
	// uses margin.y as origin.x
	m_Rect[ix].margin.y = (width-txt_width)/2.0f;
	auto rect = &m_Rect[ix];
	if (!is_resize) {
		// init, title len = group size + 1
		m_Dwrite[rect->dwrite_ix].build_TextLayout(
			m_App->m_hwnd,
			rect->text,
			ix,
			txt_width,
			height,
			m_TitleFontFactor,
			rect->group.size()+1);
	}
	else {
		m_Dwrite[rect->dwrite_ix].on_resize_TextLayout(
			m_App->m_hwnd,
			ix,
			txt_width,
			height,
			m_TitleFontFactor,
			rect->group.size()+1);
	}
	// uses margin.w as text layout's height
	m_Dwrite[m_Rect[ix].dwrite_ix].on_size_get_TextLayout_height(ix, m_Rect[ix].margin.w);
}
//
template <typename T_app>
PCWSTR ui_base<T_app>::get_DefineTxt(const std::string &name)
{
	return m_DefineTxt[name].c_str();
}
//
template <typename T_app>
void ui_base<T_app>::on_resize()
{
	// if not init, no resize
	if (!m_IsInitialized) return;
	GetClientRect(m_App->m_hwnd, &m_RcHWND);
	for (auto &dwrite: m_Dwrite) dwrite.second.on_resize_CreateTextFormat(m_App->m_hwnd);
	for (size_t ix = 0; ix != m_Rect.size(); ++ix) {
		if (m_Rect[ix].tp < 3 || m_Rect[ix].tp == 4) {
			if (m_Rect[ix].parent < 0) on_resize_RectFromHWND(ix);
			else on_resize_RectFromRect(ix, m_Rect[ix].parent);
		}
		if (m_Rect[ix].tp == 3) {
			on_resize_TextLayout(ix, true);
		}
	}
	// sprite on resize
	define_on_resize_sprite();
}
//
template <typename T_app>
void ui_base<T_app>::set_Brush(D2D1::ColorF color, const float &opacity, const std::string index)
{
	D2D1_BRUSH_PROPERTIES br_prop;
	br_prop.opacity = opacity;
	br_prop.transform = D2D1::IdentityMatrix();
	HR(m_App->m_D2DDC->CreateSolidColorBrush(color, br_prop, &m_Brush[index]));
}
//
template <typename T_app>
void ui_base<T_app>::draw_d2d()
{
	for (auto it = m_Rect.begin(); it != m_Rect.end(); ++it) {
		if (!it->active) continue;
		if (it->tp < 2) {
			m_App->m_D2DDC->FillRectangle(&it->rc, m_Brush[it->brush_ix]);
		}
		if (it->tp == 0 || it->tp == 2 || it->tp == 4) {
			m_Dwrite[it->dwrite_ix].draw_ExternalRect(m_App->m_D2DDC, it->text, it->rc);
		}
		if (it->tp == 1) {
			m_Dwrite[it->dwrite_ix].draw_ExternalRect(m_App->m_D2DDC, it->text, it->rc, it->brush_ix == it->brush_sel[1]);
		}
		if (it->tp == 3) {
			// uses margin.y as origin.x
			m_TextLayoutOrigin.x = it->margin.y;
			m_Dwrite[it->dwrite_ix].draw_TextLayout(m_App->m_D2DDC, m_TextLayoutOrigin, it-m_Rect.begin());
		}
	}
}
//
template <typename T_app>
void ui_base<T_app>::draw_d3d()
{
	if (!m_IsInitialized) return;
	m_Sprite.draw_d3d(m_MapSpriteRect, m_Rect, m_MapSpriteName);
}
//
template <typename T_app>
bool ui_base<T_app>::on_mouse_down(WPARAM btn_state, const int &pos_x, const int &pos_y)
{
	if (is_waiting_for_something()) return false;
	if (btn_state & MOUSE_UI_PICK) {
		m_IsPadUsing = false;
		mouse_pick(pos_x, pos_y);
		return apply_ix(m_ClickIxMouse);
	}
	return false;
}
//
template <typename T_app>
bool ui_base<T_app>::on_mouse_wheel(const short &z_delta)
{
	if (is_waiting_for_something()) return false;
	for (auto &map: m_MapTextLayout) {
		if (m_Rect[map.second[0]].active) {
			float restrict_y = -m_Rect[map.second[0]].margin.w+100.0f;
			m_TextLayoutOrigin.y += z_delta/120*30;
			if (m_TextLayoutOrigin.y < restrict_y) m_TextLayoutOrigin.y = restrict_y;
			if (m_TextLayoutOrigin.y > 0.0f) m_TextLayoutOrigin.y = 0.0f;
			return true;
		}
	}
	return false;
}
//
template <typename T_app>
void ui_base<T_app>::on_mouse_over(const int &pos_x, const int &pos_y)
{
	if (is_waiting_for_something()) return;
	for (size_t ix = 0; ix != m_Rect.size(); ++ix) {
		if (m_Rect[ix].tp == ui_rect::type::button && m_Rect[ix].active &&
			pos_y > m_Rect[ix].rc.top && pos_y < m_Rect[ix].rc.bottom &&
			pos_x > m_Rect[ix].rc.left && pos_x < m_Rect[ix].rc.right) {
			m_Rect[ix].brush_ix = m_Rect[ix].brush_sel[1];
		}
		else m_Rect[ix].brush_ix = m_Rect[ix].brush_sel[0];
	}
}
//
template <typename T_app>
void ui_base<T_app>::mouse_pick(const int &pos_x, const int &pos_y)
{
	if (is_waiting_for_something()) return;
	for (size_t ix = 0; ix != m_Rect.size(); ++ix) {
		if (m_Rect[ix].tp == ui_rect::type::button && m_Rect[ix].active &&
			pos_y > m_Rect[ix].rc.top && pos_y < m_Rect[ix].rc.bottom &&
			pos_x > m_Rect[ix].rc.left && pos_x < m_Rect[ix].rc.right) {
			m_ClickIxMouse = static_cast<int>(ix);
			break;
		}
	}
}
//
template <typename T_app>
bool ui_base<T_app>::on_pad_keydown(const WORD &vkey)
{
	if (is_waiting_for_something()) return false;
	m_IsPadUsing = true;
	if (define_on_pad_keydown(vkey)) return true;
	if (vkey == PAD_UI_DWON1 || vkey == PAD_UI_DWON2) {
		pad_loop_button(true);
		if (vkey == PAD_UI_DWON1) pad_roll_text_layout(true);
		return true;
	}
	if (vkey == PAD_UI_UP1 || vkey == PAD_UI_UP2) {
		pad_loop_button(false);
		if (vkey == PAD_UI_UP1) pad_roll_text_layout(false);
		return true;
	}
	//
	if (m_ClickableActived != "none") {
		if (vkey == PAD_UI_APPLY) apply_ix(m_ClickIxPad);
		return true;
	}
	return false;
}
//
template <typename T_app>
bool ui_base<T_app>::on_input_keydown(WPARAM &w_param, LPARAM &l_param)
{
	if (is_waiting_for_something()) return false;
	m_IsPadUsing = false;
	if (define_on_input_keydown(w_param, l_param)) return true;
	// if not use a mouse, only use keyboard, this behavior is similar as using pad
	if (w_param == KEY_UI_DOWN1 || w_param == KEY_UI_DOWN2) {
		pad_loop_button(true);
		return true;
	}
	if (w_param == KEY_UI_UP1 || w_param == KEY_UI_UP2) {
		pad_loop_button(false);
		return true;
	}
	if (w_param == KEY_UI_PAGEDOWN) {
		pad_roll_text_layout(true);
		return true;
	}
	if (w_param == KEY_UI_PAGEUP) {
		pad_roll_text_layout(false);
		return true;
	}
	if (m_ClickableActived != "none" && w_param == KEY_UI_ENTER) {
		apply_ix(m_ClickIxPad, true);
		return true;
	}
	return false;
}
//
template <typename T_app>
void ui_base<T_app>::group_active_switch(const std::string &name)
{
	// avoid active two clickable group
	if (!(m_ClickableActived == "none" || m_ClickableActived == name)) {
		define_deactivate_all_default();
		return;
	}
	for (auto &ix: m_MapGroup[name]) m_Rect[ix].active = !m_Rect[ix].active;
	group_active(name, false, true);
}
//
template <typename T_app>
void ui_base<T_app>::group_active(const std::string &name, const bool &is_act, const bool &is_from_switched, const bool &is_slient_click_once)
{
	// avoid click paly fx twice
	if (is_slient_click_once) m_IsSlientClickOnce = true;
	if (!is_from_switched) for (auto &ix: m_MapGroup[name]) m_Rect[ix].active = is_act;
	if (m_Rect[m_MapGroup[name][0]].active) {
		m_ClickIxPad = -1;
		m_LastUiActive = m_App->m_Timer.total_time();
		if (m_MapButton[name].size() > 0) {
			m_ClickableActived = name;
			pad_loop_button(true, name);
		}
		else {
			// keep no button group, as no-clickable
			m_ClickableActived = "none";
		}
	}
	else {
		m_ClickableActived = "none";
		// reset text_layout position
		m_TextLayoutOrigin.y = 0.0f;
	}
}
//
template <typename T_app>
void ui_base<T_app>::pad_loop_button(const bool &is_down, const std::string &select_none, const bool &is_slient_click_once)
{
	// avoid click paly fx twice
	if (is_slient_click_once) m_IsSlientClickOnce = true;
	if (!m_IsSlientClickOnce) m_App->m_Scene.audio.play_effect(m_ClickSound);
	else m_IsSlientClickOnce = false;
	// select none
	if (select_none != "") {
		assert(m_MapButton[select_none].size() > 0);
		for (auto &ix: m_MapButton[select_none]) m_Rect[ix].brush_ix = m_Rect[ix].brush_sel[0];
		// if a button brush_sel[0] == brush_sel[1], it is not need to do a hit test,
		// the button invisible, thus force to select first
		// pad always select first
		auto *button = &m_Rect[m_MapButton[select_none][0]];
		if (m_IsPadUsing || button->brush_sel[0] == button->brush_sel[1]) pad_loop_button(true, "", true);
		return;
	}
	// select first
	if (m_ClickIxPad == -1 && m_ClickableActived != "none") {
		assert(m_MapButton[m_ClickableActived].size() > 0);
		m_Rect[m_MapButton[m_ClickableActived][0]].brush_ix =
			m_Rect[m_MapButton[m_ClickableActived][0]].brush_sel[1];
		m_ClickIxPad = static_cast<int>(m_MapButton[m_ClickableActived][0]);
		return;
	}
	// select next
	if (m_ClickIxPad != -1 && m_ClickableActived != "none") {
		m_Rect[m_ClickIxPad].brush_ix = m_Rect[m_ClickIxPad].brush_sel[0];
		// point to next button
		size_t pos =
			find(m_MapButton[m_ClickableActived].begin(), m_MapButton[m_ClickableActived].end(),
			m_ClickIxPad) - m_MapButton[m_ClickableActived].begin();
		int next = static_cast<int>(pos);
		//
		if (is_down) ++next;
		else --next;
		int int_size = static_cast<int>(m_MapButton[m_ClickableActived].size());
		if (next > int_size-1) next = 0;
		if (next < 0) next = int_size-1;
		m_ClickIxPad = static_cast<int>(m_MapButton[m_ClickableActived][next]);
		m_Rect[m_MapButton[m_ClickableActived][next]].brush_ix =
			m_Rect[m_MapButton[m_ClickableActived][next]].brush_sel[1];
	}
}
//
template <typename T_app>
void ui_base<T_app>::pad_roll_text_layout(const bool &is_down)
{
	for (auto &map: m_MapTextLayout) {
		if (m_Rect[map.second[0]].active) {
			float restrict_y = -m_Rect[map.second[0]].margin.w+100.0f;
			float delta = 100.0f;
			if (is_down) delta = -delta;
			m_TextLayoutOrigin.y += delta;
			if (m_TextLayoutOrigin.y < restrict_y) m_TextLayoutOrigin.y = restrict_y;
			if (m_TextLayoutOrigin.y > 0.0f) m_TextLayoutOrigin.y = 0.0f;
			return;
		}
	}
	return;
}
//
template <typename T_app>
bool ui_base<T_app>::is_ui_appear()
{
	if (m_ClickableActived == "none") return false;
	return true;
}
//
template <typename T_app>
void ui_base<T_app>::deactivate_all()
{
	for (auto &map: m_MapGroup) {
		group_active(map.first, false);
	}
	m_ClickableActived = "none";
}
//
template <typename T_app>
bool ui_base<T_app>::apply_ix(int &index, const bool &is_like_pad)
{
	if (index != -1) {
		if (define_apply_ix_if(index)) {
			m_App->m_Scene.audio.play_effect(m_ClickSound);
			// no change m_ClickIxPad and m_ClickIxMouse value in define_ functions
			// let pad select first button immediately from another group apply, keep m_ClickIxPad
			// otherwise m_ClickIxPad will be clean by apply, the first selected buttom will be invalid
			//
			// if not use a mouse, only use keyboard, this behavior is similar as using pad
			if (!m_IsPadUsing && !is_like_pad) {
				index = -1;
			}
			return true;
		}
	}
	return false;
}
//
template <typename T_app>
bool ui_base<T_app>::is_waiting_for_something()
{
	if (m_IsMute | m_IsOtherUIAppear) return true;
	return false;
}
//
}
#endif