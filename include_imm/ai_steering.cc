////////////////
// ai_steering.cc
// by Huang Yiting
////////////////
////////////////
#include "ai_steering.h"
#include "imm_ptr.h"
namespace imm
{
////////////////
// steering
////////////////
////////////////
//
void steering::update(const float &dt)
{
	// clear old data
	if (attack.size() > 100) attack.erase(attack.begin(), attack.begin()+50);
	if (damage.size() > 100) damage.erase(damage.begin(), damage.begin()+50);
	if (PTR->m_Control.player1 == index || !is_active) return;
	update_dt += dt;
	action_dt += dt;
	if (update_dt > AI_DT_PHY_SLOW) {
		update_dt -= AI_DT_PHY_SLOW;
		if (!is_out_of_control()) current_state->execute(this);
	}
	//
	commond_input();
}
//
bool steering::is_out_of_control()
{
	if (PTR->m_Inst.m_Troll[index].current_state == pose_FallDown::instance()) return true;
	if (PTR->m_Inst.m_Troll[index].current_state == pose_Eliminated::instance()) return true;
	return false;
}
//
void steering::commond_input()
{
	dump_act.update_atk();
}
////////////////
// ai_Standby
////////////////
////////////////
ai_Standby *ai_Standby::instance()
{
	static ai_Standby instance;
	return &instance;
}
//
void ai_Standby::enter(steering *ste)
{
	ste;
}
//
void ai_Standby::execute(steering *ste)
{
	if (ste->tactics & AI_TAC_SEEK) {
		ste->tactics = AI_TAC_NONE;
		ste->change_state(ai_Seek::instance());
	}
	if (ste->tactics & AI_TAC_ATK) {
		ste->tactics = AI_TAC_NONE;
		ste->change_state(ai_Atk::instance());
	}
	if (ste->tactics & AI_TAC_PATROL) {
		ste->tactics = AI_TAC_NONE;
		ste->change_state(ai_Patrol::instance());
	}
	if (ste->tactics & AI_TAC_FOLLOW) {
		ste->tactics = AI_TAC_NONE;
		ste->change_state(ai_Follow::instance());
	}
}
//
void ai_Standby::exit(steering *ste)
{
	ste;
}
////////////////
// ai_Patrol
////////////////
////////////////
ai_Patrol *ai_Patrol::instance()
{
	static ai_Patrol instance;
	return &instance;
}
//
void ai_Patrol::enter(steering *ste)
{
	ste->tar_list = PTR->m_AiInfo.set_friend;
}
//
void ai_Patrol::execute(steering *ste)
{
	int get_tar = -1;
	for (auto &tar_ix: ste->tar_list) {
		if (PTR->m_Inst.m_Stat[tar_ix].get_IsOffline()) continue;
		if (PTR->m_Inst.m_Steering[ste->index].alert[tar_ix]) {
			get_tar = static_cast<int>(tar_ix);
			break;
		}
	}
	if (get_tar > 0) {
		ste->target = get_tar;
		PTR->m_Inst.m_Troll[ste->index].order_ac |= ORDER_IDLE;
		ste->report |= AI_REP_ALERT;
		ste->change_state(ai_Standby::instance());
		return;
	}
	if (ste->action_dt > ste->random_t) {
		XMFLOAT3 random_pos = PTR->m_Inst.m_BoundW.center(ste->index);
		random_pos.x += math::calc_randf(5.0f, 15.0f)*(rand()%2 ? 1.0f : -1.0f);
		random_pos.z += math::calc_randf(5.0f, 15.0f)*(rand()%2 ? 1.0f : -1.0f);
		PTR->m_AiInfo.clip_pos(random_pos.x, random_pos.z);
		ste->desired_pos = random_pos;
		PTR->m_Inst.m_Troll[ste->index].order_stat |= ORDER_ST_WALK;
		PTR->m_Inst.m_Troll[ste->index].order_ac |= ORDER_MOVE_HIT;
		ste->action_dt = 0.0f;
		ste->random_t = math::calc_randf(10.0f, 20.0f);
	}
	if (PTR->m_Control.map_stop[ste->index].is_lag) {
		ste->random_t = 0.0f;
	}
}
//
void ai_Patrol::exit(steering *ste)
{
	if (PTR->m_Inst.m_Troll[ste->index].order_stat & ORDER_ST_WALK) {
		PTR->m_Inst.m_Troll[ste->index].order_stat ^= ORDER_ST_WALK;
	}
}
////////////////
// ai_Seek
////////////////
////////////////
ai_Seek *ai_Seek::instance()
{
	static ai_Seek instance;
	return &instance;
}
//
void ai_Seek::enter(steering *ste)
{
	ste->count_down_reset();
}
//
void ai_Seek::execute(steering *ste)
{
	if (PTR->m_Inst.m_Stat[ste->target].get_IsOffline()) {
		ste->report |= AI_REP_TAR_ELIMINATE;
		ste->change_state(ai_Standby::instance());
		return;
	}
	if (PTR->m_Inst.m_Troll[ste->target].order_stat & ORDER_ST_DESTROYED) {
		return;
	}
	if (ste->count_down < 0.0f) {
		ste->desired_pos = PTR->m_Inst.m_BoundW.center(ste->target);
		PTR->m_Inst.m_Troll[ste->index].order_ac |= ORDER_MOVE_HIT;
		ste->count_down = 1.0f;
	}
	if (PTR->m_Control.map_stop[ste->index].is_stop) {
		if (!ste->close[ste->target]) ste->count_down -= AI_DT_PHY_SLOW;
	}
	if (PTR->m_Control.map_stop[ste->index].last_time > 5.0f) {
		PTR->m_Inst.m_Troll[ste->index].order_ac |= ORDER_IDLE;
		ste->count_down = -1.0f;
	}
	if (PTR->m_Inst.m_Steering[ste->index].close[ste->target]) {
		PTR->m_Inst.m_Troll[ste->index].order_ac |= ORDER_IDLE;
		ste->report |= AI_REP_TAR_CLOSE;
		ste->change_state(ai_Standby::instance());
		return;
	}
}
//
void ai_Seek::exit(steering *ste)
{
	ste;
}
////////////////
// ai_Atk
////////////////
////////////////
ai_Atk *ai_Atk::instance()
{
	static ai_Atk instance;
	return &instance;
}
//
void ai_Atk::enter(steering *ste)
{
	ste;
}
//
void ai_Atk::execute(steering *ste)
{
	if (PTR->m_Inst.m_Stat[ste->target].get_IsOffline()) {
		ste->report |= AI_REP_TAR_ELIMINATE;
		ste->change_state(ai_Standby::instance());
		return;
	}
	// wait pose_Idle
	if (PTR->m_Inst.m_Troll[ste->index].current_state == pose_Move::instance()) {
		return;
	}
	if (PTR->m_Control.atk.is_execute(ste->index)) {
		return;
	}
	if (PTR->m_Inst.m_Troll[ste->index].current_state != pose_Atk::instance()) {
		if (ste->action_dt > AI_DT_PHY_SLOW*3.0f) {
			ste->action_dt = 0.0f;
			math2::set_inst_face_to_inst2(ste->index, ste->target);
		}
	}
	if (!PTR->m_Inst.m_Steering[ste->index].close[ste->target]) {
		ste->report |= AI_REP_TAR_AWAY;
		ste->change_state(ai_Standby::instance());
		return;
	}
	else {
		std::string model = *PTR->m_Inst.m_Stat[ste->index].get_ModelName();
		auto *chunk = &PTR->m_Control.atk.ski_data[model].chunk;
		int chunk_size = static_cast<int>(chunk->size());
		int casual = rand() % (chunk_size+1);
		auto chunk_it = chunk->begin();
		if (casual < chunk_size) {
			for (int ix = 0; ix != casual; ++ix) ++chunk_it;
			PTR->m_Control.atk.pool.inquiry(model, chunk_it->first, ste->index);
		}
		else {
			ste->change_state(ai_Guard::instance());
		}
		return;
	}
}
//
void ai_Atk::exit(steering *ste)
{
	ste;
}
////////////////
// ai_Guard
////////////////
////////////////
ai_Guard *ai_Guard::instance()
{
	static ai_Guard instance;
	return &instance;
}
//
void ai_Guard::enter(steering *ste)
{
	PTR->m_Inst.m_Troll[ste->index].order_ac |= ORDER_GUARD;
	PTR->m_Inst.m_Troll[ste->index].order_stat |= ORDER_ST_GUARD;
	int casual = rand()%3;
	ste->count_down = static_cast<float>(casual)+0.5f;
}
//
void ai_Guard::execute(steering *ste)
{
	if (PTR->m_Inst.m_Troll[ste->index].current_state == pose_Atk::instance()) {
		ste->change_state(ai_Atk::instance());
		return;
	}
	if (ste->count_down > 0.0f) {
		ste->count_down -= AI_DT_PHY_SLOW;
	}
	else {
		ste->change_state(ai_Atk::instance());
	}
	PTR->m_Inst.m_Troll[ste->index].order_ac |= ORDER_GUARD;
	PTR->m_Inst.m_Troll[ste->index].order_stat |= ORDER_ST_GUARD;
}
//
void ai_Guard::exit(steering *ste)
{
	PTR->m_Inst.m_Troll[ste->index].order_ac |= ORDER_GUARD_NO;
}
////////////////
// ai_Follow
////////////////
////////////////
ai_Follow *ai_Follow::instance()
{
	static ai_Follow instance;
	return &instance;
}
//
void ai_Follow::enter(steering *ste)
{
	ste->tar_list = PTR->m_AiInfo.set_enemy;
}
//
void ai_Follow::execute(steering *ste)
{
	size_t player1 = PTR->m_Control.player1;
	bool hurry_up = false;
	for (auto &tar: ste->tar_list) {
		auto it_set = PTR->m_Inst.m_Steering[player1].hatred.find(tar);
		if (it_set != PTR->m_Inst.m_Steering[player1].hatred.end()) {
			hurry_up = true;
			break;
		}
	}
	int get_tar = -1;
	for (auto &tar_ix: ste->tar_list) {
		if (PTR->m_Inst.m_Stat[tar_ix].get_IsOffline()) continue;
		if (PTR->m_Inst.m_Steering[ste->index].alert[tar_ix]) {
			get_tar = static_cast<int>(tar_ix);
			break;
		}
	}
	if (get_tar > 0) {
		ste->target = get_tar;
		PTR->m_Inst.m_Troll[ste->index].order_ac |= ORDER_IDLE;
		ste->report |= AI_REP_ALERT;
		ste->change_state(ai_Standby::instance());
		return;
	}
	if (ste->action_dt > ste->random_t) {
		XMFLOAT3 random_pos = PTR->m_Inst.m_BoundW.center(player1);
		random_pos.x += math::calc_randf(5.0f, 10.0f)*(rand()%2 ? 1.0f : -1.0f);
		random_pos.z += math::calc_randf(5.0f, 10.0f)*(rand()%2 ? 1.0f : -1.0f);
		PTR->m_AiInfo.clip_pos(random_pos.x, random_pos.z);
		ste->desired_pos = random_pos;
		if (PTR->m_Inst.m_Troll[player1].order_stat & ORDER_ST_ENGAGE) {
			PTR->m_Inst.m_Troll[ste->index].order_stat &= ~ORDER_ST_WALK;
		}
		else {
			PTR->m_Inst.m_Troll[ste->index].order_stat |= ORDER_ST_WALK;
		}
		if (hurry_up) {
			PTR->m_Inst.m_Troll[ste->index].order_stat &= ~ORDER_ST_WALK;
		}
		PTR->m_Inst.m_Troll[ste->index].order_ac |= ORDER_MOVE_HIT;
		ste->action_dt = 0.0f;
		ste->random_t = math::calc_randf(3.0f, 5.0f);
	}
	if (PTR->m_Control.map_stop[ste->index].is_lag) {
		ste->random_t = 0.0f;
	}
}
//
void ai_Follow::exit(steering *ste)
{
	if (PTR->m_Inst.m_Troll[ste->index].order_stat & ORDER_ST_WALK) {
		PTR->m_Inst.m_Troll[ste->index].order_stat ^= ORDER_ST_WALK;
	}
}
//
}