////////////////
// imm_app.cc
// by Huang Yiting
////////////////
////////////////
#include "imm_app.h"
#include "imm_ptr.h"
namespace imm
{
imm_app::imm_app():
	base_win<imm_app>(),
	m_IsSyncInterval(false),
	m_Cmd(),
	m_Cam(),
	m_Smap(0),
	m_Scene(),
	m_Inst(),
	m_PhyPos(),
	m_Hit(),
	m_Magic(),
	m_UiMgr(),
	m_Condition(),
	m_Config(this),
	m_Control(),
	m_AiInfo(),
	m_AiAttr(),
	m_AiInterf(),
	m_SfxSelect(),
	m_Status(),
	m_MouseMode(CAM_MOVE_BY_BUTTON)
{
	m_WindowName = L"immature engine Demo";
	m_LastMousePos.x = 0;
	m_LastMousePos.y = 0;
	m_Cam.set_Position(0.0f, 7.0f, -25.0f);
}
//
imm_app::~imm_app()
{
	echo_close();
	m_D3DDC->ClearState();
	delete m_Smap;
	effects::destroy_all();
	input_layouts::destroy_all();
	render::destroy_all();
}
//
bool imm_app::init_imm()
{
	srand(static_cast<unsigned>(time(NULL)));
	effects::init_all(m_D3DDevice, IMM_PATH["shader"]);
	input_layouts::init_all(m_D3DDevice);
	render::init_all(m_D3DDevice);
	m_Smap = new shadow_map(m_D3DDevice, m_SMapSize, m_SMapSize);
	m_Cam.set_Lens(0.25f*XM_PI, aspect_ratio(), 1.0f, 1000.0f);
	ptr_app_init(this);
	m_UiMgr.init(this);
	m_Cmd.init(this);
	m_Inst.init(this);
	m_PhyPos.init(this);
	m_Control.init(this);
	m_SfxSelect.init(this);
	m_Scene.init_load(this);
	m_Condition.init(this);
	m_Config.init_additional();
	return true;
}
//
void imm_app::on_resize()
{
	m_Cmd.on_resize();
	m_UiMgr.on_resize();
	base_win<imm_app>::on_resize();
	m_Cam.set_Lens(0.25f*XM_PI, aspect_ratio(), 1.0f, 1000.0f);
	m_Inst.on_resize();
}
//
void imm_app::update_scene(float dt)
{
	m_Scene.update_listen_thread_for_reload();
	// m_UiMgr before m_Cmd, always update, m_Cmd will close m_UiMgr
	m_UiMgr.define_update(dt);
	if (m_Cmd.is_waiting_for_something()) return;
	update_scene_keydown(dt);
	// Ensure m_Cmd.is_waiting_for_something()
	if (m_Cmd.is_waiting_for_something()) return;
	build_shadow_transform();
	m_Cam.update_view_matrix();
	m_Condition.update(dt);
	m_Control.update_scene(dt);
	m_Scene.update_atmosphere(dt);
	m_AiAttr.update(dt);
	m_AiInterf.update(dt);
	m_SfxSelect.update(dt);
}
//
void imm_app::update_scene_keydown(float dt)
{
	m_Control.update_keydown_and_pad(dt);
}
//
void imm_app::on_mouse_down(WPARAM btn_state, int x, int y)
{
	if (m_Cmd.is_waiting_for_something()) return;
	SetCapture(this->get_hwnd());
	m_Control.on_mouse_down(btn_state, x, y);
	if (btn_state & MOUSE_CAM_MOVE || MOUSE_CAM_MOVE == 0) {
		m_LastMousePos.x = x;
		m_LastMousePos.y = y;
	}
}
//
void imm_app::on_mouse_up(WPARAM btn_state, int x, int y)
{
	DUMMY(x);
	DUMMY(y);	
	if (m_Cmd.is_waiting_for_something()) return;
	DUMMY(btn_state);
	ReleaseCapture();
}
//
void imm_app::on_mouse_move(WPARAM btn_state, int x, int y)
{
	if (m_Cmd.is_waiting_for_something()) return;
	m_Control.on_mouse_move(btn_state, x, y);
	if (btn_state & MOUSE_CAM_MOVE || MOUSE_CAM_MOVE == 0) {
		m_LastMousePos.x = x;
		m_LastMousePos.y = y;
	}
}
//
void imm_app::on_mouse_wheel(WPARAM btn_state, int x, int y)
{
	DUMMY(x);
	DUMMY(y);
	if (m_Cmd.is_waiting_for_something()) return;
	short z_delta = GET_WHEEL_DELTA_WPARAM(btn_state);
	m_Control.on_mouse_wheel(z_delta);
}
//
void imm_app::on_input_char(WPARAM w_param, LPARAM l_param)
{
	m_Cmd.on_input_char(w_param, l_param);
}
//
void imm_app::on_input_keydown(WPARAM w_param, LPARAM l_param)
{
	m_Cmd.on_input_keydown(w_param, l_param);
	if (m_Cmd.is_waiting_for_something()) return;
	m_Control.on_input_keydown(w_param, l_param);
}
//
void imm_app::on_input_keyup(WPARAM w_param, LPARAM l_param)
{
	m_Control.on_input_keyup(w_param, l_param);
}
//
void imm_app::game_suspend(const bool &is_stop)
{
	base_win::game_suspend(is_stop);
	m_Scene.audio.suspend(is_stop);
}
////////////////
// inl
////////////////
////////////////
#include "imm_app_draw.cc"
}