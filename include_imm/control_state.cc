////////////////
// control_state.cc
// by Huang Yiting
////////////////
////////////////
#include "control_state.h"
#include "imm_ptr.h"
namespace imm
{
////////////////
// act_dump_vec
////////////////
////////////////
//
void act_dump_vec::update_atk()
{
	if (command.size() == 0) return;
	if (atk_return == 1) {
		command.pop_back();
		atk_return = 2;
		return;
	}
	PTR->m_Inst.m_Troll[index].atk_symbol = command.back().symbol;
	PTR->m_Inst.m_Troll[index].order_ac = command.back().order_ac;
}
////////////////
// troll
////////////////	
////////////////
bool troll::is_ai()
{
	if (index == PTR->m_Control.player1) return false;
	return true;
}
//
bool troll::is_static()
{
	if (PTR->m_Inst.m_Stat[PTR->m_Control.player1].type != MODEL_SKINNED) return true;
	return false;
}
////////////////
// pose_Idle
////////////////
////////////////
pose_Idle *pose_Idle::instance()
{
	static pose_Idle instance;
	return &instance;
}
//
void pose_Idle::enter(troll *tro)
{
	if (tro->order_ac & ORDER_ENGAGE) {
		tro->order_ac ^= ORDER_ENGAGE;
		tro->A.cd_Idle = 3.0f+math::calc_randf(-1.0f, 1.0f);
		tro->order_stat |= ORDER_ST_ENGAGE;
	}
	if (tro->order_ac & ORDER_IDLE) {
		tro->order_ac ^= ORDER_IDLE;
	}
	if (!PTR->m_Inst.m_Stat[tro->index].is_in_switch_clip()) {
		int is_do_clip = 1;
		if (PTR->m_Inst.m_Stat[tro->index].get_ClipName() == &EMPTY_STRING) is_do_clip = 0;
		if (*PTR->m_Inst.m_Stat[tro->index].get_ClipName() == tro->act.Idle()) is_do_clip = 0;
		if ((tro->order_ac & ORDER_GUARD) 
			&& (*PTR->m_Inst.m_Stat[tro->index].get_ClipName() == tro->act.DamageStr1())) is_do_clip = 2;
		if (is_do_clip == 1) {
			PTR->m_Inst.m_Stat[tro->index].set_switch_current_ClipName(tro->act.Idle(), 5);
		}
		if (is_do_clip == 2) {
			PTR->m_Inst.m_Stat[tro->index].check_set_ClipName(tro->act.Engage());
		}
	}
}
//
void pose_Idle::execute(troll *tro)
{
	if (tro->A.cd_GuardMin > 0.0f) {
		tro->A.cd_GuardMin -= PTR->m_Timer.delta_time();
	}
	if (tro->order_stat & ORDER_ST_ENGAGE) {
		if (tro->A.cd_Idle > 0.0f) {
			tro->A.cd_Idle -= PTR->m_Timer.delta_time();
		}
		else {
			tro->order_stat ^= ORDER_ST_ENGAGE;
			PTR->m_Inst.m_Stat[tro->index].set_switch_current_ClipName(tro->act.Idle(), 5);
		}
	}
	// ORDER_DMG
	if (tro->order_ac & ORDER_DMG) {
		if (PTR->m_AiAttr.is_poise_greater_than_zero(tro->index)) {
			if (tro->order_ac & ORDER_ATK_X || tro->order_ac & ORDER_ATK_Y) {
				tro->change_state_execute(pose_Atk::instance());
				return;
			}
		}
		tro->change_state_execute(pose_Damage::instance());
		return;
	}
	// ORDER_ATK_X ORDER_ATK_Y
	if (tro->order_ac & ORDER_ATK_X || tro->order_ac & ORDER_ATK_Y) {
		tro->change_state_execute(pose_Atk::instance());
		return;
	}
	//
	if (tro->order_ac & ORDER_JUMP) {
		tro->change_state_execute(pose_Jump::instance());
		return;
	}
	if (tro->order_ac & ORDER_GUARD) {
		tro->order_ac ^= ORDER_GUARD;
		bool is_do_guard = true;
		if (*PTR->m_Inst.m_Stat[tro->index].get_ClipName() == tro->act.Idle()) is_do_guard = false;
		tro->order_stat |= ORDER_ST_GUARD;
		if (is_do_guard) {
			PTR->m_Inst.m_Stat[tro->index].set_switch_current_ClipName(tro->act.Idle(), 5);
			tro->A.cd_GuardMin = FPS60_1DIV;
		}
	}
	if (tro->order_ac & ORDER_GUARD_NO) {
		bool is_do_guard_no = true;
		if (tro->A.cd_GuardMin > 0.0f) is_do_guard_no = false;
		if (!(tro->order_stat & ORDER_ST_GUARD)) {
			// clear fake ORDER_GUARD_NO
			tro->order_ac ^= ORDER_GUARD_NO;
			is_do_guard_no = false;
		}
		if (is_do_guard_no) {
			tro->order_ac ^= ORDER_GUARD_NO;
			tro->order_ac &= ~ORDER_GUARD;
			tro->order_stat ^= ORDER_ST_GUARD;
			tro->order_stat |= ORDER_ST_ENGAGE;
			tro->A.cd_Idle = 3.0f;
			PTR->m_Inst.m_Stat[tro->index].set_switch_current_ClipName(tro->act.Idle(), 5);
		}
		return;
	}
	if (tro->order_ac & ORDER_MOVE_HIT) {
		tro->change_state_execute(pose_Move::instance());
		return;
	}
	if (tro->order_ac & ORDER_MOVE_TOWARD) {
		tro->change_state_execute(pose_Move::instance());
		return;
	}
	if (tro->order_ac & ORDER_MOVE_WASD) {
		tro->change_state_execute(pose_Move::instance());
		return;
	}
}
//
void pose_Idle::exit(troll *tro)
{
	tro;
}
////////////////
// pose_Move
////////////////
////////////////
pose_Move *pose_Move::instance()
{
	static pose_Move instance;
	return &instance;
}
//
void pose_Move::enter(troll *tro)
{
	tro;
}
//
void pose_Move::execute(troll *tro)
{
	if (tro->order_ac & ORDER_DMG) {
		math2::ai_move_pos_stop(tro->index);
		tro->change_state_execute(pose_Damage::instance());
		return;
	}
	if (tro->order_ac & ORDER_IDLE) {
		if (tro->is_ai()) math2::ai_move_pos_stop(tro->index);
		tro->change_state(pose_Idle::instance());
		return;
	}
	// just for test
	if (tro->order_ac & ORDER_MOVE_HIT) {
		if (!PTR->m_Inst.m_Stat[tro->index].phy.is_on_land) return;
		tro->order_ac ^= ORDER_MOVE_HIT;
		if (tro->is_ai()) math2::ai_move_pos(tro->index, tro->speed_move());
		else math2::mouse_inst_move(tro->index, tro->speed_move());
		PTR->m_Inst.m_Stat[tro->index].check_set_ClipName(tro->act.Run());
		return;
	}
	if (tro->order_ac & ORDER_MOVE_TOWARD) {
		if (!PTR->m_Inst.m_Stat[tro->index].phy.is_on_land) return;
		// roll
		if (tro->order_ac & ORDER_ROLL) {
			tro->change_state_execute(pose_Roll::instance());
			return;
		}
		//
		tro->order_ac ^= ORDER_MOVE_TOWARD;
		if (PTR->m_Control.pad.is_L_active()) {
			math2::pad_move_toward(tro->index, tro->speed_move());
			PTR->m_Inst.m_Stat[tro->index].check_set_ClipName(tro->act.Run());
		}
		else {
			PTR->m_Inst.m_Stat[tro->index].phy.vel_indirect = XMFLOAT3(0.0f, 0.0f, 0.0f);
			tro->change_state(pose_Idle::instance());
		}
	}
	if (tro->order_ac & ORDER_MOVE_WASD) {
		if (!PTR->m_Inst.m_Stat[tro->index].phy.is_on_land) return;
		// roll
		if (tro->order_ac & ORDER_ROLL) {
			tro->change_state_execute(pose_Roll::instance());
			return;
		}
		//
		tro->order_ac ^= ORDER_MOVE_WASD;
		if (!math2::key_move_wasd(tro->index, tro->speed_move())) tro->change_state(pose_Idle::instance());
		else PTR->m_Inst.m_Stat[tro->index].check_set_ClipName(tro->act.Run());
	}
	if (tro->order_ac & ORDER_JUMP) {
		tro->change_state_execute(pose_Jump::instance());
		return;
	}
	if (tro->order_ac & ORDER_ATK_X || tro->order_ac & ORDER_ATK_Y) {
		tro->change_state(pose_Atk::instance());
		return;
	}
}
//
void pose_Move::exit(troll *tro)
{
	tro;
}
////////////////
// pose_Roll
////////////////
////////////////
pose_Roll *pose_Roll::instance()
{
	static pose_Roll instance;
	return &instance;
}
//
void pose_Roll::enter(troll *tro)
{
	tro;
}
//
void pose_Roll::execute(troll *tro)
{
	if (tro->order_ac & ORDER_DMG) {
		math2::ai_move_pos_stop(tro->index);
		tro->change_state_execute(pose_Damage::instance());
		return;
	}
	if (tro->order_ac & ORDER_MOVE_WASD || tro->order_ac & ORDER_MOVE_TOWARD) {
		// roll
		if (tro->A.cd_RollStep > 0.0f) {
			tro->A.cd_RollStep -= PTR->m_Timer.delta_time();
			return;
		}
		if (tro->A.cd_RollToIdle > 0.0f) {
			tro->A.cd_RollToIdle -= PTR->m_Timer.delta_time();
			math2::key_move_wasd(tro->index, 0.0f);
			tro->A.cd_RollStep = tro->A.cd_RollToIdle;
			tro->A.cd_RollToIdle = -1.0f;
			tro->battle_stat ^= BATTLE_STAT_DODGE;
			tro->order_ac = ORDER_ENGAGE;
			return;
		}
		// roll
		if (tro->order_ac & ORDER_ROLL) {
			tro->order_ac ^= ORDER_ROLL;
			if (!PTR->m_AiAttr.is_required_ap(SKILL_STAMINA_ROLL, tro->index)) {
				tro->revert_previous_state();
				return;
			}
			if (tro->order_ac & ORDER_MOVE_WASD) {
				if (!math2::key_move_wasd(tro->index, tro->A.speed_Roll)) {
					PTR->m_AiAttr.is_required_ap(SKILL_STAMINA_ROLL_FALSE, tro->index);
					tro->change_state(pose_Idle::instance());
					return;
				}
			}
			if (tro->order_ac & ORDER_MOVE_TOWARD) {
				if (PTR->m_Control.pad.is_L_active()) {
					math2::pad_move_toward(tro->index, tro->A.speed_Roll);
				}
				else {
					PTR->m_AiAttr.is_required_ap(SKILL_STAMINA_ROLL_FALSE, tro->index);
					tro->change_state(pose_Idle::instance());
					return;
				}
			}
			PTR->m_Inst.m_Stat[tro->index].check_set_ClipName(tro->act.Roll(), true);
			tro->A.cd_RollStep = tro->A.frame_RollStep;
			tro->A.cd_RollToIdle = tro->A.frame_RollToIdle;
			tro->battle_stat |= BATTLE_STAT_DODGE;
			return;
		}
		if (tro->order_ac & ORDER_ENGAGE) {
			tro->change_state(pose_Idle::instance());
			return;
		}
		return;
	}
}
//
void pose_Roll::exit(troll *tro)
{
	tro;
}
////////////////
// pose_Jump
////////////////
////////////////
pose_Jump *pose_Jump::instance()
{
	static pose_Jump instance;
	return &instance;
}
//
void pose_Jump::enter(troll *tro)
{
	tro->order_ac = ORDER_NONE;
	if (!PTR->m_Inst.m_Stat[tro->index].phy.is_on_land) tro->revert_previous_state();
	PTR->m_Inst.m_Stat[tro->index].phy.velocity.y = tro->A.velocity_Jump;
	PTR->m_Inst.m_Stat[tro->index].check_set_ClipName(tro->act.Jump());
	tro->A.cd_Jump = TIME_1_MINITE;
}
//
void pose_Jump::execute(troll *tro)
{
	if (tro->order_ac & ORDER_DMG) {
		tro->change_state_execute(pose_Damage::instance());
		return;
	}
	//
	bool is_on_land = PTR->m_Inst.m_Stat[tro->index].phy.is_on_land;
	if (!tro->is_ON_AIR && !is_on_land && tro->A.cd_Jump > TIME_59_SECONDS) {
		tro->is_ON_AIR = true;
	}
	if (tro->is_ON_AIR && is_on_land) {
		PTR->m_Inst.m_Stat[tro->index].check_set_ClipName(tro->act.JumpLand(), true);
		tro->is_ON_AIR = false;
		PTR->m_Inst.m_Stat[tro->index].phy.friction_rev_give = 0.01f;
		// Idle do full JumpLand
		if (tro->previous_state == pose_Idle::instance()) tro->A.cd_Jump = tro->A.frame_JumpLand;
		else tro->A.cd_Jump = 0.1f;
	}
	//
	if (!tro->is_ON_AIR && is_on_land && tro->A.cd_Jump > TIME_59_SECONDS) {
		tro->A.cd_Jump += PTR->m_Timer.delta_time();
		// no jump happen
		if (tro->A.cd_Jump > TIME_1_MINITE + (FPS60_1DIV*5.0f)) {
			tro->revert_previous_state();
		}
	}
	if (!tro->is_ON_AIR && is_on_land && tro->A.cd_Jump < TIME_59_SECONDS) {
		if (tro->A.cd_Jump > 0.0f) {
			tro->A.cd_Jump -= PTR->m_Timer.delta_time();
		}
		else {
			tro->revert_previous_state();
		}
	}
	if (tro->order_ac & ORDER_JUMP && tro->A.cd_Jump > tro->A.frame_JumpLand) {
		tro->order_ac ^= ORDER_JUMP;
	}
}
//
void pose_Jump::exit(troll *tro)
{
	tro;
}
////////////////
// pose_Atk
////////////////
////////////////
pose_Atk *pose_Atk::instance()
{
	static pose_Atk instance;
	return &instance;
}
//
void pose_Atk::enter(troll *tro)
{
	PTR->m_Inst.m_Stat[tro->index].check_set_ClipName(tro->act.Engage());
	// clear guard stat
	tro->order_stat &= ~ORDER_ST_GUARD;
	if (tro->is_ai()) math2::ai_move_pos_stop(tro->index);
}
//
void pose_Atk::execute(troll *tro)
{
	auto clear_order_atk = [](int &order_ref) {
		order_ref &= ~ORDER_ATK_X;
		order_ref &= ~ORDER_ATK_Y;
	};
	if (tro->order_ac & ORDER_DMG_DOWN) {
		tro->change_state_execute(pose_Damage::instance());
		return;
	}
	if (tro->order_ac & ORDER_DMG) {
		if (PTR->m_AiAttr.is_poise_greater_than_zero(tro->index)) {
			// damage but not break state
			tro->order_stat &= ~ORDER_ST_GUARD;
			tro->order_ac &= ~ORDER_DMG;
			for (auto &damage_ix: tro->guard_damage_index) {
				PTR->m_Control.atk.damage[damage_ix].order_stat_dmg = tro->order_stat;
			}
			tro->guard_damage_index.clear();
		}
		else {
			tro->change_state_execute(pose_Damage::instance());
			return;
		}
	}
	if (tro->order_ac & ORDER_ATK_X || tro->order_ac & ORDER_ATK_Y) {
		if (tro->atk_symbol != '0') {
			clear_order_atk(tro->order_ac);
			int atk_int;
			atk_int = PTR->m_Control.atk.execute(tro->index, tro->atk_symbol);
			PTR->m_Inst.m_Steering[tro->index].dump_act.atk_return = atk_int;
			return;
		}
	}
	if (tro->order_ac & ORDER_ATK_X) {
		clear_order_atk(tro->order_ac);
		PTR->m_Control.atk.execute(tro->index, 'A');
		return;
	}
	if (tro->order_ac & ORDER_ATK_Y) {
		clear_order_atk(tro->order_ac);
		PTR->m_Control.atk.execute(tro->index, 'B');
		return;
	}
	if (PTR->m_Control.atk.is_execute(tro->index)) {
		return;
	}
	if (tro->order_ac & ORDER_IDLE) {
		PTR->m_Inst.m_Stat[tro->index].set_sequence_ClipName(tro->act.Engage());
		tro->change_state(pose_Idle::instance());
		return;
	}
}
//
void pose_Atk::exit(troll *tro)
{
	// clear guard stat
	tro->order_stat &= ~ORDER_ST_GUARD;
}
////////////////
// pose_Damage
////////////////
////////////////
pose_Damage *pose_Damage::instance()
{
	static pose_Damage instance;
	return &instance;
}
//
void pose_Damage::enter(troll *tro)
{
	auto clear_order_dmg = [](int &order_ref) {
		order_ref &= ~ORDER_DMG;
		order_ref &= ~ORDER_DMG2;
		order_ref &= ~ORDER_DMG_DOWN;
	};
	//
	for (auto &damage_ix: tro->guard_damage_index) {
		PTR->m_Control.atk.damage[damage_ix].order_stat_dmg = tro->order_stat;
	}
	tro->guard_damage_index.clear();
	//
	if (tro->order_ac & ORDER_DMG2) {
		PTR->m_Inst.m_Stat[tro->index].check_set_ClipName(tro->act.Damage2(), true);
		tro->A.cd_Damage = tro->A.frame_Damage2;
		tro->is_DOWN = true;
		clear_order_dmg(tro->order_ac);
		return;
	}
	if (tro->order_ac & ORDER_DMG_DOWN) {
		PTR->m_Inst.m_Stat[tro->index].check_set_ClipName(tro->act.Damage(), true, tro->A.scale_time_DamageDown);
		tro->A.cd_Damage = tro->A.frame_Damage/tro->A.scale_time_DamageDown;
		tro->is_DOWN = true;
		tro->order_stat |= ORDER_ST_DESTROYED;
		clear_order_dmg(tro->order_ac);
		return;
	}
	if (tro->order_stat & ORDER_ST_GUARD) {
		PTR->m_Inst.m_Stat[tro->index].check_set_ClipName(tro->act.Damage(), true);
		tro->A.cd_Damage = tro->A.frame_Damage;
	}
	else {
		PTR->m_Inst.m_Stat[tro->index].check_set_ClipName(tro->act.Damage(), true);
		tro->A.cd_Damage = tro->A.frame_Damage;
	}
	clear_order_dmg(tro->order_ac);
}
//
void pose_Damage::execute(troll *tro)
{
	for (auto &damage_ix: tro->guard_damage_index) {
		PTR->m_Control.atk.damage[damage_ix].order_stat_dmg = tro->order_stat;
	}
	tro->guard_damage_index.clear();
	//
	if (tro->order_ac & ORDER_DMG_DOWN) {
		tro->is_DOWN = true;
		tro->order_stat |= ORDER_ST_DESTROYED;
		tro->order_ac ^= ORDER_DMG_DOWN;
		PTR->m_Inst.m_Stat[tro->index].check_set_ClipName(tro->act.Damage(), false, tro->A.scale_time_DamageDown);
		tro->A.cd_Damage /= tro->A.scale_time_DamageDown;
	}
	tro->A.cd_Damage -= PTR->m_Timer.delta_time();
	if (tro->A.cd_Damage < 0.0f) {
		if (tro->is_DOWN) {
			tro->change_state(pose_FallDown::instance());
		}
		else {
			tro->order_ac = ORDER_ENGAGE;
			tro->change_state(pose_Idle::instance());
		}
	}
}
//
void pose_Damage::exit(troll *tro)
{
	tro;
}
////////////////
// pose_FallDown
////////////////
////////////////
pose_FallDown *pose_FallDown::instance()
{
	static pose_FallDown instance;
	return &instance;
}
//
void pose_FallDown::enter(troll *tro)
{
	tro->is_DOWN = true;
	tro->is_GET_UP = false;
	tro->A.cd_DownKeep = tro->A.frame_DownCollapse;
	tro->A.cd_GetUp = tro->A.frame_GetUp;
	if (tro->order_stat & ORDER_ST_DESTROYED) {
		PTR->m_Inst.m_Stat[tro->index].check_set_ClipName(tro->act.DownCollapse(), true);
	}
	else {
		PTR->m_Inst.m_Stat[tro->index].check_set_ClipName(tro->act.DownKeep());
	}
}
//
void pose_FallDown::execute(troll *tro)
{
	if (tro->A.cd_DownKeep > 0.0f) {
		tro->A.cd_DownKeep -= PTR->m_Timer.delta_time();
	}
	else if (tro->A.cd_GetUp > 0.0f) {
	if (tro->order_stat & ORDER_ST_DESTROYED) {
		tro->change_state(pose_Eliminated::instance());
		return;
	}
	// get up
	tro->A.cd_GetUp -= PTR->m_Timer.delta_time();
	if (!tro->is_GET_UP) {
		PTR->m_Inst.m_Stat[tro->index].check_set_ClipName(tro->act.GetUp(), true);
		tro->is_GET_UP = true;
	}
	}
	if (tro->A.cd_GetUp < 0.0f) {
		// return idle
		tro->order_ac = ORDER_NONE;
		tro->order_ac = ORDER_ENGAGE;
		tro->change_state(pose_Idle::instance());
	}
}
//
void pose_FallDown::exit(troll *tro)
{
	tro->is_DOWN = false;
}
////////////////
// pose_Eliminated
////////////////
////////////////
pose_Eliminated *pose_Eliminated::instance()
{
	static pose_Eliminated instance;
	return &instance;
}
//
void pose_Eliminated::enter(troll *tro)
{
	tro->A.cd_Eliminated = 0.5f;
	PTR->m_Inst.m_Stat[tro->index].check_set_ClipName(tro->act.DownKeep());
	XMFLOAT3 center = PTR->m_Inst.m_BoundW.center(tro->index);
	PTR->m_SfxSelect.play_effect(SKILL_SYSTEM_ELIMINATE1, tro->index, tro->index, center);
}
//
void pose_Eliminated::execute(troll *tro)
{
	if (tro->A.cd_Eliminated < -2.0f) return;
	if (tro->A.cd_Eliminated > 0.0f) {
		tro->A.cd_Eliminated -= PTR->m_Timer.delta_time();
	}
	else {
		PTR->m_Inst.m_Stat[tro->index].set_IsOffline(true);
		XMFLOAT3 center = PTR->m_Inst.m_BoundW.center(tro->index);
		PTR->m_SfxSelect.play_effect(SKILL_SYSTEM_ELIMINATE2, tro->index, tro->index, center);
		tro->A.cd_Eliminated = -3.0f;
	}
}
//
void pose_Eliminated::exit(troll *tro)
{
	tro;
}
//
}