////////////////
// imm_app.h
// by Huang Yiting
////////////////
////////////////
#ifndef IMM_APP_H
#define IMM_APP_H
#include <ctime>
#include "ai_interface.h"
#include "ai_status_data.h"
#include "ai_information.h"
#include "condition_trigger.h"
#include "phy_hit_box.h"
#include "control_sys.h"
#include "imm_cmd.h"
#include "ui_mgr.h"
#include "stru_scene_mgr.h"
#include "stru_draw_instance.h"
#include "imm_base.h"
#include "render_shadow.h"
#include "audio_sfx_select.h"
namespace imm
{
////////////////
// imm_app
////////////////
////////////////
class imm_app: public base_win<imm_app>
{
public:
	imm_app();
	~imm_app();
	bool init_imm();
	void on_resize();
	void update_scene(float dt);
	void update_scene_keydown(float dt);
	void draw_scene();
	void on_mouse_down(WPARAM btn_state, int x, int y);
	void on_mouse_up(WPARAM btn_state, int x, int y);
	void on_mouse_move(WPARAM btn_state, int x, int y);
	void on_mouse_wheel(WPARAM btn_state, int x, int y);
	void on_input_char(WPARAM w_param, LPARAM l_param);
	void on_input_keydown(WPARAM w_param, LPARAM l_param);
	void on_input_keyup(WPARAM w_param, LPARAM l_param);
	void game_suspend(const bool &is_stop);
	void build_shadow_transform();
	void draw_scene_d3d();
	void draw_scene_d3d_shadow();
	void draw_scene_d3d_slient();
	void draw_scene_d2d();
	// light_helper.fx SMAP_SIZE
	static const int m_SMapSize = 8192;
	bool m_IsSyncInterval;
	cmd_shell<imm_app> m_Cmd;
	POINT m_LastMousePos;
	camera m_Cam;
	shadow_map *m_Smap;
	scene_mgr<imm_app> m_Scene;
	instance_mgr<imm_app> m_Inst;
	phy_position<imm_app> m_PhyPos;
	phy_hit_arrange<imm_app> m_Hit;
	phy_magic<imm_app> m_Magic;
	ui_mgr<imm_app> m_UiMgr;
	condition_trigger<imm_app> m_Condition;
	lua_config<imm_app> m_Config;
	control_sys<imm_app> m_Control;
	ai_attr<imm_app> m_AiAttr;
	ai_interface<imm_app> m_AiInterf;
	ai_info<imm_app> m_AiInfo;
	sfx_select<imm_app> m_SfxSelect;
	status_data m_Status;
	MOUSE_MODEL m_MouseMode;
	XMFLOAT4X4 m_LightView;
	XMFLOAT4X4 m_LightProj;
	XMFLOAT4X4 m_ShadowTransform;
};
//
}
#endif