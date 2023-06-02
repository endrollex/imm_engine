////////////////
// control_state.h
// by Huang Yiting
////////////////
////////////////
#ifndef CONTROL_STATE_H
#define CONTROL_STATE_H
#include "control_math.h"
#include "util_basic.h"
#include "phy_prepare.h"
namespace imm
{
////////////////
// ORDER_ACT_TYPE
////////////////
////////////////
enum ORDER_ACT_TYPE
{
	ORDER_NONE        = 0x0,
	ORDER_JUMP        = 0x1,
	ORDER_MOVE_HIT    = 0x2,
	ORDER_MOVE_TOWARD = 0x4,
	ORDER_MOVE_WASD   = 0x8,
	ORDER_IDLE        = 0x10,
	ORDER_ATK_X       = 0x20,
	ORDER_ATK_Y       = 0x40,
	ORDER_DMG         = 0x80,
	ORDER_ROLL        = 0x100,
	ORDER_GUARD       = 0x200,
	ORDER_GUARD_NO    = 0x400,
	ORDER_ENGAGE      = 0x800,
	ORDER_DMG2        = 0x1000,
	ORDER_KNOCK_DOWN  = 0x2000,
	ORDER_DMG_DOWN    = 0x4000,
};
//
enum ORDER_STAT_TYPE
{
	ORDER_ST_CLEAR     = 0x0,
	ORDER_ST_WALK      = 0x1, // NOT: ORDER_IS_RUN
	ORDER_ST_ROLL      = 0x2,
	ORDER_ST_ENGAGE    = 0x4,
	ORDER_ST_GUARD     = 0x8,
	ORDER_ST_DESTROYED = 0x10,
};
////////////////
// BATTLE_STAT
////////////////
////////////////
enum BATTLE_STAT
{
	BATTLE_STAT_NONE  = 0x0,
	BATTLE_STAT_DODGE = 0x1,
};
////////////////
// state
////////////////
////////////////
template <class T_troll>
struct state
{
	virtual ~state() {;}
	virtual void enter(T_troll*) = 0;
	virtual void execute(T_troll*) = 0;
	virtual void exit(T_troll*) = 0;
};
////////////////
// pose_Idle
////////////////
////////////////
struct troll;
struct pose_Idle: public state<troll>
{
	static pose_Idle *instance();
	void enter(troll*);
	void execute(troll*);
	void exit(troll*);
private:
	pose_Idle() {;}
	pose_Idle(const pose_Idle&);
	pose_Idle &operator=(const pose_Idle&);
};
////////////////
// pose_Move
////////////////
////////////////
struct troll;
struct pose_Move: public state<troll>
{
	static pose_Move *instance();
	void enter(troll*);
	void execute(troll*);
	void exit(troll*);
private:
	pose_Move() {;}
	pose_Move(const pose_Move&);
	pose_Move &operator=(const pose_Move&);
};
////////////////
// pose_Roll
////////////////
////////////////
struct troll;
struct pose_Roll: public state<troll>
{
	static pose_Roll *instance();
	void enter(troll*);
	void execute(troll*);
	void exit(troll*);
private:
	pose_Roll() {;}
	pose_Roll(const pose_Roll&);
	pose_Roll &operator=(const pose_Roll&);
};
////////////////
// pose_Jump
////////////////
////////////////
struct troll;
struct pose_Jump: public state<troll>
{
	static pose_Jump *instance();
	void enter(troll*);
	void execute(troll*);
	void exit(troll*);
private:
	pose_Jump() {;}
	pose_Jump(const pose_Jump&);
	pose_Jump &operator=(const pose_Jump&);
};
////////////////
// pose_Atk
////////////////
////////////////
struct troll;
struct pose_Atk: public state<troll>
{
	static pose_Atk *instance();
	void enter(troll*);
	void execute(troll*);
	void exit(troll*);
private:
	pose_Atk() {;}
	pose_Atk(const pose_Atk&);
	pose_Atk &operator=(const pose_Atk&);
};
////////////////
// pose_Damage
////////////////
////////////////
struct troll;
struct pose_Damage: public state<troll>
{
	static pose_Damage *instance();
	void enter(troll*);
	void execute(troll*);
	void exit(troll*);
private:
	pose_Damage() {;}
	pose_Damage(const pose_Damage&);
	pose_Damage &operator=(const pose_Damage&);
};
////////////////
// pose_FallDown
////////////////
////////////////
struct troll;
struct pose_FallDown: public state<troll>
{
	static pose_FallDown *instance();
	void enter(troll*);
	void execute(troll*);
	void exit(troll*);
private:
	pose_FallDown() {;}
	pose_FallDown(const pose_FallDown&);
	pose_FallDown &operator=(const pose_FallDown&);
};
////////////////
// pose_Eliminated
////////////////
////////////////
struct troll;
struct pose_Eliminated: public state<troll>
{
	static pose_Eliminated *instance();
	void enter(troll*);
	void execute(troll*);
	void exit(troll*);
private:
	pose_Eliminated() {;}
	pose_Eliminated(const pose_Eliminated&);
	pose_Eliminated &operator=(const pose_Eliminated&);
};
////////////////
// act_str
////////////////
////////////////
struct act_str
{
	act_str();
	act_str(int &order_stat);
	std::string Idle();
	std::string Run();
	std::string Jump();
	std::string JumpLand();
	std::string Engage();
	std::string Damage();
	std::string Damage2();
	std::string DamageStr1();
	std::string DownCollapse();
	std::string Roll();
	std::string WalkRev();
	std::string Guard();
	std::string DownKeep();
	std::string GetUp();
	int *p_order_s;
};
////////////////
// action_data
////////////////
////////////////
struct action_data
{
	action_data();
	float velocity_Jump;
	float speed_Walk;
	float speed_Run;
	float speed_Roll;
	float frame_Damage;
	float frame_Damage2;
	float frame_DownCollapse;
	float frame_RollStep;
	float frame_RollToIdle;
	float frame_JumpLand;
	float frame_GetUp;
	float cd_Idle;
	float cd_Jump;
	float cd_Damage;
	float cd_RollStep;
	float cd_RollToIdle;
	float cd_GuardMin;
	float cd_DownKeep;
	float cd_GetUp;
	float cd_Eliminated;
	float scale_time_DamageDown;
};
//
action_data::action_data():
	velocity_Jump(35.0f),
	speed_Walk(4.5f),
	speed_Run(13.5f),
	speed_Roll(20.0f),
	frame_Damage(10.0f*FRAME_RATE_1DIV),
	frame_Damage2(12.0f*FRAME_RATE_1DIV),
	frame_DownCollapse(20.0f*FRAME_RATE_1DIV),
	frame_RollStep(10.0f*FRAME_RATE_1DIV),
	frame_RollToIdle(10.0f*FRAME_RATE_1DIV),
	frame_JumpLand(7.0f*FRAME_RATE_1DIV),
	frame_GetUp(25.0f*FRAME_RATE_1DIV),
	cd_Idle(-1.0f),
	cd_Jump(-1.0f),
	cd_Damage(-1.0f),
	cd_RollStep(-1.0f),
	cd_RollToIdle(-1.0f),
	cd_GuardMin(-1.0f),
	cd_DownKeep(-1.0f),
	cd_GetUp(-1.0f),
	cd_Eliminated(-1.0f),
	scale_time_DamageDown(0.7f)
{
	;
}
////////////////
// act_dump_vec
////////////////
////////////////
struct act_dump_vec
{
	struct dump
	{
		int order_ac;
		char symbol;
	};
	std::vector<dump> command;
	size_t index;
	int atk_return;
	void init(const size_t &index_in);
	void push_atk(const size_t &act_in, const char &symbol_in);
	void update_atk();
};
//
void act_dump_vec::init(const size_t &index_in)
{
	index = index_in;
	atk_return = 0;
}
//
void act_dump_vec::push_atk(const size_t &act_in, const char &symbol_in)
{
	dump temp;
	temp.order_ac= static_cast<int>(act_in);
	temp.symbol = symbol_in;
	command.push_back(temp);
}
////////////////
// troll
////////////////
////////////////
struct troll
{
	troll();
	void init(const size_t &index_in);
	void update();
	void change_state(state<troll> *new_state);
	void change_state_execute(state<troll> *new_state);
	void revert_previous_state();
	bool is_ai();
	bool is_static();
	float speed_move();
	state<troll> *current_state;
	state<troll> *previous_state;
	size_t index;
	int order_ac;
	int order_stat;
	int battle_stat;
	int focus;
	float last_face_rot_time;
	bool is_ON_AIR;
	bool is_DOWN;
	bool is_GET_UP;
	char atk_symbol;
	std::vector<int> guard_damage_index;
	action_data A;
	act_str act;
	act_dump_vec vec_dump;
};
//
troll::troll():
	current_state(pose_Idle::instance()),
	previous_state(pose_Idle::instance()),
	index(0),
	order_ac(ORDER_NONE),
	order_stat(ORDER_ST_CLEAR),
	battle_stat(BATTLE_STAT_NONE),
	focus(-1),
	last_face_rot_time(0.0f),
	is_ON_AIR(false),
	is_DOWN(false),
	is_GET_UP(false),
	atk_symbol('0'),
	guard_damage_index(),
	A(),
	act(order_stat),
	vec_dump()
{
	;
}
//
void troll::init(const size_t &index_in)
{
	index = index_in;
	vec_dump.init(index);
}
//
void troll::update()
{
	current_state->execute(this);
}
//
void troll::change_state(state<troll> *new_state)
{
	assert(current_state && new_state);
	previous_state = current_state;
	current_state->exit(this);
	current_state = new_state;
	current_state->enter(this);
}
//
void troll::change_state_execute(state<troll> *new_state)
{
	assert(current_state && new_state);
	previous_state = current_state;
	current_state->exit(this);
	current_state = new_state;
	current_state->enter(this);
	current_state->execute(this);
}
//
void troll::revert_previous_state()
{
	change_state(previous_state);
}
//
float troll::speed_move()
{
	if (order_stat & ORDER_ST_WALK)	return A.speed_Walk;
	return A.speed_Run;
}
////////////////
// inl
////////////////
////////////////
#include "control_state_act.cc"
}
#endif