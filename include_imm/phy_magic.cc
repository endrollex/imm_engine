////////////////
// phy_magic.cc
// by Huang Yiting
////////////////
////////////////
#include "phy_magic.h"
#include "control_atk.h"
#include "imm_ptr.h"
namespace imm
{
////////////////
// magic_inst
////////////////
////////////////
magic_inst::magic_inst():
	specify(SKILL_MAGIC_LIGHTNING),
	count_down(1.0f),
	duration(1.0f),
	caster(0),
	target(-1),
	is_run(false),
	is_stop(false),
	is_abort(false)
{
	;
}
//
magic_inst::~magic_inst()
{
	;
}
//
void magic_inst::update(const float &dt)
{
	count_down -= dt;
	duration -=dt;
	if (count_down < 0.0f && !is_run) {
		is_run = true;
		switch (specify) {
		case SKILL_MAGIC_LIGHTNING:
			apply_MAGIC_LIGHTNING();
			is_stop = true;
			break;
		case SKILL_MAGIC_MISSILE:
			apply_MAGIC_MISSILE();
			break;
		}
	}
	if (duration < 0.0f) {
		is_run = false;
		is_stop = true;
	}
	if (is_run && !is_stop) {
		size_t ix = 0;
		for (auto &circle: vec_circle) {
			circle.update(dt, PTR, this, ix);
			++ix;
		}
		//
		switch (specify) {
		case SKILL_MAGIC_LIGHTNING:
			break;
		case SKILL_MAGIC_MISSILE:
			update_MAGIC_MISSILE();
			break;
		}
	}
}
//
void magic_inst::apply_MAGIC_LIGHTNING()
{
	assert(vec_hit_shperes.size() > 0);
	target = -1;
	target = PTR->m_AiInfo.random_an_all_screen(caster);
	assert(target != -1);
	vec_hit_shperes[0].Center = PTR->m_Inst.m_BoundW.center(target);
	PTR->m_Scene.plasma.push_back(PLASMA_LIGHTNING, 3.0f, vec_hit_shperes[0].Center);
	PTR->m_Scene.audio.play_effect(sfx::Lightning);
	for (auto &ix_inst: PTR->m_AiInfo.vec_all) {
		bool is_touch = PTR->m_Inst.m_BoundW.intersects(ix_inst, vec_hit_shperes[0]);
		if (is_touch) {
			PTR->m_Control.atk.cause_damage(
				caster,
				ix_inst,
				math2::VECTOR3_NULL,
				SKILL_MAGIC_LIGHTNING);
			//
		}
	}
}
//
void magic_inst::apply_MAGIC_MISSILE()
{
	assert(vec_hit_shperes.size() > 0);
	duration = 45.0f;
	std::string missile_bone = PTR->m_Control.atk.casting_bone.at(SKILL_MAGIC_MISSILE);
	vec_bone_name.emplace_back(missile_bone);
	vec_circle.emplace_back();
	vec_circle[0].active(PTR->m_Hit.center_box(caster, vec_bone_name[0]));
	PTR->m_Scene.plasma.push_back(PLASMA_MISSILE1, plasma_duration(), vec_circle[0].edge, true);
	vec_hit_shperes[0].Center = vec_circle[0].edge;
	target = PTR->m_AiInfo.select_nearby_enemy(caster);
	if (target == -1) {
		is_abort = true;
		is_stop = true;
	}
	PTR->m_Scene.audio.play_effect(sfx::Whoosh);
}
//
void magic_inst::update_MAGIC_MISSILE()
{
	assert(vec_hit_shperes.size() > 0);
	vec_hit_shperes[0].Center = vec_circle[0].edge;
	bool is_hit = PTR->m_Inst.m_BoundW.intersects(target, vec_hit_shperes[0]);
	if (is_hit) is_stop = true;
	for (auto &ix_inst: PTR->m_AiInfo.vec_all) {
		if (ix_inst == caster) continue;
		if (set_hit_ix.find(ix_inst) != set_hit_ix.end()) continue;
		bool is_touch = PTR->m_Inst.m_BoundW.intersects(ix_inst, vec_hit_shperes[0]);
		if (is_touch) {
			PTR->m_Control.atk.cause_damage(
				caster,
				ix_inst,
				math2::VECTOR3_NULL,
				SKILL_MAGIC_MISSILE);
			//
			PTR->m_Scene.plasma.push_back(PLASMA_STRIKE, 0.5f, vec_hit_shperes[0].Center);
			PTR->m_Scene.plasma.push_back(PLASMA_MISSILE2, 0.2f, vec_hit_shperes[0].Center);
			set_hit_ix.insert(ix_inst);
		}
	}
}
//
float magic_inst::plasma_duration()
{
	// magic_inst exists after plasma destruct
	return duration - 1.0f;
}
//
}