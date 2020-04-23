////////////////
// control_atk.h
// by Huang Yiting
////////////////
////////////////
#ifndef CONTROL_ATK_H
#define CONTROL_ATK_H
#include "control_state.h"
#include "phy_magic.h"
#include "ai_skill_pool.h"
#include <vector>
#include <set>
namespace imm
{
////////////////
// SKILL_TYPE
////////////////
////////////////
enum SKILL_TYPE
{
	SKILL_TYPE_MELEE,
	SKILL_TYPE_MAGIC,
};
////////////////
// skill_para
////////////////
////////////////
struct skill_para
{
	skill_para();
	std::string model_name;
	int skill_ix;
	int current_ix;
	size_t inst_ix;
	bool is_busy;
	bool is_turn_next;
	bool is_judge;
	bool is_execute;
	bool is_adjust_dmg_dir;
	float count_down;
	char symbol;
};
//
skill_para::skill_para():
	skill_ix(-1),
	current_ix(-1),
	inst_ix(0),
	is_busy(false),
	is_turn_next(false),
	is_judge(false),
	is_execute(false),
	is_adjust_dmg_dir(false),
	count_down(-1.0f),
	symbol('A')
{
	;
}
////////////////
// skill_data
////////////////
////////////////
struct skill_data
{
	skill_data();
private:
	void current_apply(skill_para &pa);
	void chunk_over(skill_para &pa);
public:
	bool is_required_ap(skill_para &pa);
	int strike(skill_para &pa);
	void update(const float &dt, skill_para &pa);
	SKILL_TYPE get_skill_type(const skill_para &pa);
	std::vector<std::string> atk;
	std::vector<float> frame_end;
	std::vector<float> frame_turn;
	std::vector<float> judge_start;
	std::vector<float> judge_end;
	std::vector<int> next_ix;
	std::vector<SKILL_SPECIFY> specify;
	std::vector<std::vector<std::string>> atk_box;
	std::vector<std::list<XMFLOAT2>> inst_speed2;
	std::vector<float> impulse;
	std::vector<float> poise;
	std::map<char, int> chunk;
};
////////////////
// damage_data
////////////////
////////////////
struct damage_data
{
	damage_data();
	void update(const float &dt);
	void update_melee(const float &dt);
	void update_magic(const float &dt);
	void stamp();
	size_t ix_atk;
	size_t ix_dmg;
	int index;
	int skill_ix;
	int order_stat_dmg;
	float count_down;
	float delay;
	bool is_calculated;
	bool is_delay;
	const XMFLOAT3 *box_center;
	const skill_data *m_skill_data;
	SKILL_SPECIFY specify;
};
////////////////
// control_atk
////////////////
////////////////
template <typename T_app>
struct control_atk
{
	control_atk();
	void init(T_app *app_in);
	void rebuild_action();
	void reset();
	void init_skill_para(const size_t &index_in);
	void cause_damage(
		const size_t &inst_ix_atk,
		const size_t &inst_ix_dmg,
		const XMFLOAT3 &box_center,
		const SKILL_SPECIFY &specify);
	int execute(const size_t &index_in, const char &symbol);
	void update(const float &dt);
	bool is_execute(const size_t &index_in);
	bool is_cannot_be_attacked(const size_t &damage_ix);
	float current_impulse(const size_t &index_in);
	T_app *app;
	std::map<std::string, skill_data> ski_data;
	std::map<size_t, skill_para> ski_para;
	std::map<int, damage_data> damage;
	std::map<size_t, std::set<size_t>> hits;
	std::map<SKILL_SPECIFY, std::string> casting_bone;
	skill_pool pool;
};
//
}
#endif