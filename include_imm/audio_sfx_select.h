////////////////
// audio_sfx_select.h
// by Huang Yiting
////////////////
////////////////
#ifndef AUDIO_SFX_SELECT_H
#define AUDIO_SFX_SELECT_H
#include "phy_magic.h"
namespace imm
{
////////////////
// sfx_task
////////////////
////////////////
struct sfx_task
{
	XMFLOAT3 center;
	size_t ix;
};
////////////////
// sfx_select
////////////////
////////////////
template <typename T_app>
struct sfx_select
{
	sfx_select();
	void init(T_app *app_in);
	void update(float dt);
	void update_block(float dt);
	void play_effect(
		const SKILL_SPECIFY &skill,
		const size_t &ix1,
		const size_t &ix2,
		const XMFLOAT3 &center,
		const int &order_stat_dmg = -1);
	void effect_block_task(const size_t &ix1);
	T_app *app;
	std::vector<sfx_task> block;
	std::map<size_t, float> block_cd;
};
//
template <typename T_app>
sfx_select<T_app>::sfx_select():
	app(nullptr)
{
	;
}
//
template <typename T_app>
void sfx_select<T_app>::init(T_app *app_in)
{
	app = app_in;
}
//
template <typename T_app>
void sfx_select<T_app>::play_effect(
	const SKILL_SPECIFY &skill,
	const size_t &ix1,
	const size_t &ix2,
	const XMFLOAT3 &center,
	const int &order_stat_dmg)
{
	ix1;
	ix2;
	ID3D11ShaderResourceView* resource = nullptr;
	switch(skill) {
	case SKILL_SYSTEM_ELIMINATE1:	
		app->m_Scene.plasma.push_back(PLASMA_CHARGE, 0.5f, center);
		app->m_Inst.m_Stat[ix1].set_SwatchTex(SWATCH_TEX_TWINKLE, 1.0f, resource);
		break;
	case SKILL_SYSTEM_ELIMINATE2:
		app->m_Scene.plasma.push_back(PLASMA_BROKEN, 2.0f, center);
		app->m_Scene.audio.play_effect(sfx::Breaking);
		break;
	}
	//
	if (skill == SKILL_MELEE_STANDARD) {
		assert(order_stat_dmg > -1);
		if (order_stat_dmg & ORDER_ST_GUARD) {
			app->m_Scene.plasma.push_back(PLASMA_STRIKE2, 0.5f, center);
			app->m_Scene.audio.play_effect(sfx::AttackGuard);
		}
		else {
			app->m_Scene.plasma.push_back(PLASMA_STRIKE, 0.5f, center);
			app->m_Scene.audio.play_effect(sfx::Attack);
		}
	}
}
//
template <typename T_app>
void sfx_select<T_app>::update(float dt)
{
	update_block(dt);
}
//
template <typename T_app>
void sfx_select<T_app>::update_block(float dt)
{
	for (auto &task: block) {
		app->m_Scene.plasma.push_back(PLASMA_BLOCK, 0.1f, task.center);
	}
	block.clear();
	for (auto &cd: block_cd) {
		if (cd.second > 0.0f) cd.second -= dt;
	}
}
//
template <typename T_app>
void sfx_select<T_app>::effect_block_task(const size_t &ix1)
{
	if (block_cd.count(ix1)) {
		if (block_cd[ix1] > 0.0f) return;
	}
	sfx_task task;
	task.center = app->m_Inst.m_BoundW.center(ix1);
	task.ix = ix1;
	block.push_back(task);
	block_cd[ix1] = 0.1f;
}
//
}
#endif