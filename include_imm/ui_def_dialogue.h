////////////////
// ui_def_dialogue.h
// by Huang Yiting
////////////////
////////////////
#ifndef UI_DEF_DIALOGUE_H
#define UI_DEF_DIALOGUE_H
#include "ui_base.h"
namespace imm
{
////////////////
// ui_def_dialogue
////////////////
////////////////
// issue: if mouse over rect, but pad press A with none rect, pad will not active rect
template <class T_app>
struct ui_def_dialogue: public ui_base<T_app>
{
	ui_def_dialogue();
	~ui_def_dialogue();
	void define_style();
	bool define_apply_ix_if(int &index);
	bool define_on_input_keydown(WPARAM &w_param, LPARAM &l_param);
	bool define_on_pad_keydown(const WORD &vkey);
	void define_update(float dt);
	void define_deactivate_all_default();
	void define_deactivate_all_cmd_slient();
	void define_enter_and_exit();
	void define_sprite_build_buffer();
	void define_on_resize_sprite();
	void define_text();
	void rebuild_text();
	//
	bool m_IsPromptSymbol;
	float m_WaitPrompt;
};
//
template <typename T_app>
ui_def_dialogue<T_app>::ui_def_dialogue():
	m_IsPromptSymbol(false),
	m_WaitPrompt(1.0f)
{
	;
}
//
template <typename T_app>
ui_def_dialogue<T_app>::~ui_def_dialogue()
{
	;
}
//
template <typename T_app>
void ui_def_dialogue<T_app>::define_style()
{
	m_TitleFontFactor = 32.0f;
	m_Dwrite["32"].init_external_rect(m_App->m_D2DDC, m_App->m_hwnd, L"Consolas", 32.0f, DWRITE_ALIG_STYLE_CENTER);
	m_Dwrite["24"].init_external_rect(m_App->m_D2DDC, m_App->m_hwnd, L"Consolas", 24.0f, DWRITE_ALIG_STYLE_CENTER);
	m_Dwrite["22_page"].init_external_rect(m_App->m_D2DDC, m_App->m_hwnd, L"Consolas", 22.0f, DWRITE_ALIG_STYLE_PAGE);
	// brush
	m_Brush["black"];
	set_Brush(D2D1::ColorF::Black, 0.0f, "black");
	m_Brush["transparent"];
	set_Brush(D2D1::ColorF::Black, 0.0f, "transparent");
	////////////////
	//
	////////////////
	////////////////
	m_Rect.emplace_back();
	m_Rect.back().id_str = "test_sprite";
	m_Rect.back().parent_str = "-1";
	m_Rect.back().group = "test";
	m_Rect.back().tp = ui_rect::type::sprite;
	m_Rect.back().brush_sel = {"transparent"};
	m_Rect.back().text = L"";
	m_Rect.back().dwrite_ix = "32";
	m_Rect.back().margin = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
	//
	m_Rect.emplace_back();
	m_Rect.back().id_str = "test_background";
	m_Rect.back().parent_str = "-1";
	m_Rect.back().group = "test";
	m_Rect.back().tp = ui_rect::type::background;
	m_Rect.back().brush_sel = {"black"};
	m_Rect.back().text = L"";
	m_Rect.back().dwrite_ix = "32";
	m_Rect.back().margin = XMFLOAT4(0.0f, 0.75f, 0.0f, 0.0f);
	//
	m_Rect.emplace_back();
	m_Rect.back().id_str = "test_zz_background_img";
	m_Rect.back().parent_str = "-1";
	m_Rect.back().group = "test";
	m_Rect.back().tp = ui_rect::type::sprite;
	m_Rect.back().brush_sel = {"black"};
	m_Rect.back().text = L"";
	m_Rect.back().dwrite_ix = "32";
	m_Rect.back().margin = XMFLOAT4(0.0f, 0.75f, 0.0f, 0.0f);
	//
	m_Rect.emplace_back();
	m_Rect.back().id_str = "test_button";
	m_Rect.back().parent_str = "-1";
	m_Rect.back().group = "test";
	m_Rect.back().tp = ui_rect::type::button;
	m_Rect.back().brush_sel = {"transparent"};
	m_Rect.back().text = L"";
	m_Rect.back().dwrite_ix = "32";
	m_Rect.back().margin = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
	//
	m_Rect.emplace_back();
	m_Rect.back().id_str = "test_prompt";
	m_Rect.back().parent_str = "-1";
	m_Rect.back().group = "test";
	m_Rect.back().tp = ui_rect::type::text_pure;
	m_Rect.back().brush_sel = {"transparent"};
	m_Rect.back().text = L"";
	m_Rect.back().dwrite_ix = "32";
	m_Rect.back().margin = XMFLOAT4(0.9f, 0.9f, 0.0f, 0.0f);
	////////////////
	//
	////////////////
	////////////////
	build_rect_map();
	//
	for (auto &rect: m_Rect) {
		rect.brush_ix = rect.brush_sel[0];
		if (rect.tp == ui_rect::type::button) {
			rect.brush_sel.emplace_back("transparent");
		}
		rect.active = false;
	}
	m_ClickSound = sfx::Empty;
}
//
template <typename T_app>
bool ui_def_dialogue<T_app>::define_apply_ix_if(int &index)
{
	DUMMY(index);
	if (index == m_MapID["test_button"]) {
		if (m_App->m_Timer.total_time() - m_LastUiActive < m_WaitPrompt) return true;
		if (!m_TxtChunk.get_lines("test", m_MapSpriteName["test_sprite"], m_Rect[m_MapID["test_background"]].text)) {
			group_active("test", false);
			m_Rect[m_MapID["test_prompt"]].text = L"";
			m_IsPromptSymbol = false;
		}
		return true;
	}
	return false;
}
//
template <typename T_app>
bool ui_def_dialogue<T_app>::define_on_input_keydown(WPARAM &w_param, LPARAM &l_param)
{
	DUMMY(l_param);
	DUMMY(w_param);
	return false;
}
//
template <typename T_app>
bool ui_def_dialogue<T_app>::define_on_pad_keydown(const WORD &vkey)
{
	DUMMY(vkey);
	return false;
}
//
template <typename T_app>
void ui_def_dialogue<T_app>::define_update(float dt)
{
	DUMMY(dt);
	if (m_Rect[m_MapID["test_prompt"]].active && !m_IsPromptSymbol) {
		if (m_App->m_Timer.total_time() - m_LastUiActive > m_WaitPrompt) {
			m_Rect[m_MapID["test_prompt"]].text = L">";
			m_IsPromptSymbol = true;
		}
	}
}
//
template <typename T_app>
void ui_def_dialogue<T_app>::define_deactivate_all_default()
{
	deactivate_all();
}
//
template <typename T_app>
void ui_def_dialogue<T_app>::define_deactivate_all_cmd_slient()
{
	deactivate_all();
}
//
template <typename T_app>
void ui_def_dialogue<T_app>::define_enter_and_exit()
{
	m_App->m_UiMgr.status.define_show(!is_ui_appear());
}
//
template <typename T_app>
void ui_def_dialogue<T_app>::define_sprite_build_buffer()
{
	std::string concrete = IMM_PATH["script"]+"concrete_common.lua";
	lua_reader l_reader;
	l_reader.loadfile(concrete);
	std::vector<std::vector<std::string>> vec2d;
	l_reader.vec2d_str_from_table("csv_avatar", vec2d);
	vec2d.erase(vec2d.begin());
	m_Sprite.build_buffer(vec2d);
	m_MapSpriteName["test_sprite"] = "";
	m_MapSpriteName["test_zz_background_img"] = "";
	//
	if (m_Sprite.buffer_check("backg_img")) {
		m_MapSpriteName["test_zz_background_img"] = "backg_img";
	}
	//
}
//
template <typename T_app>
void ui_def_dialogue<T_app>::define_on_resize_sprite()
{
	float scr_height = static_cast<float>(m_RcHWND.bottom - m_RcHWND.top);
	float scr_width = static_cast<float>(m_RcHWND.right - m_RcHWND.left);
	float scale = scr_height/UI_RESOLUTION_HEIGHT*0.6f;
	for (auto it = m_Sprite.map_height.begin(); it != m_Sprite.map_height.end(); ++it) {
		m_Sprite.map_pos[it->first] = XMFLOAT2(scr_width*0.0f*scale, scr_height-(it->second*scale));
	}
	m_Sprite.on_resize(scale);
}
//
template <typename T_app>
void ui_def_dialogue<T_app>::define_text()
{
	;
}
//
template <typename T_app>
void ui_def_dialogue<T_app>::rebuild_text()
{
	std::map<std::string, std::string> get_scene;
	std::string concrete = IMM_PATH["text"]+"dialogue_index.lua";
	lua_reader l_reader;
	l_reader.loadfile(concrete);
	std::string scene_ix = "scene"+m_App->m_Scene.scene_ix;
	if (l_reader.is_nil(scene_ix)) return;
	// build dialogue chunk
	m_TxtChunk.remove_all();
	l_reader.map_from_table(scene_ix, get_scene);
	for (auto it = get_scene.begin(); it != get_scene.end(); ++it) {
		concrete = IMM_PATH["text"]+it->second;
		l_reader.loadfile(concrete);
		std::map<std::string, std::string> get_chunk_info;
		l_reader.map_from_table("chunk", get_chunk_info);
		for (auto &chunk: get_chunk_info) {
			std::map<std::string, std::string> chunk_raw;
			l_reader.map_from_table(chunk.second, chunk_raw);
			m_TxtChunk.insert_chunk(chunk.second, chunk_raw);
		}
	}
	m_TxtChunk.get_lines("test", m_MapSpriteName["test_sprite"], m_Rect[m_MapID["test_background"]].text);
}
//
}
#endif