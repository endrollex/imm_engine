////////////////
// ai_information.h
// by Huang Yiting
////////////////
////////////////
#ifndef AI_INFORMATION_H
#define AI_INFORMATION_H
#include <set>
#include <vector>
namespace imm
{
////////////////
// AGENT_TYPE
////////////////
////////////////
enum AGENT_TYPE
{
	AGENT_TYPE_STATIC,
	AGENT_TYPE_AI,
};
////////////////
// ai_info
////////////////
////////////////
template <typename T_app>
struct ai_info
{
	ai_info();
	void init(T_app *app_in);
	void reset();
	void rebuild();
	void rebuild_map();
	void update();
	void clip_pos(float &x, float &z);
	int random_an_enemy(const size_t &caller);
	int random_an_enemy_screen(const size_t &caller);
	int random_an_all(const size_t &caller);
	int random_an_all_screen(const size_t &caller);
	int select_nearby_enemy(const size_t &caller);
	T_app *app;
	std::set<size_t> set_friend;
	std::set<size_t> set_enemy;
	std::set<size_t> set_all;
	std::vector<size_t> vec_friend;
	std::vector<size_t> vec_enemy;
	std::vector<size_t> vec_all;
	float map_x;
	float map_z;
};
//
template <typename T_app>
ai_info<T_app>::ai_info():
	app(nullptr),
	map_x(0.0f),
	map_z(0.0f)
{
	;
}
//
template <typename T_app>
void ai_info<T_app>::init(T_app *app_in)
{
	app = app_in;
}
//
template <typename T_app>
void ai_info<T_app>::reset()
{
	set_friend.clear();
	set_enemy.clear();
	set_all.clear();
	vec_friend.clear();
	vec_enemy.clear();
	vec_all.clear();
}
//
template <typename T_app>
void ai_info<T_app>::rebuild()
{
	reset();
	for (auto &mental: app->m_AiInterf.mental_scene) {
		if (mental.type == AI_BEAT_PLAYER1) {
			if (app->m_Inst.m_Troll[mental.ix].order_ac & ORDER_DMG_DOWN) continue;
			set_enemy.insert(mental.ix);
			vec_enemy.push_back(mental.ix);
			set_all.insert(mental.ix);
			vec_all.push_back(mental.ix);
		}
		if (mental.type == AI_ASSIST_PLAYER1) {
			if (app->m_Inst.m_Troll[mental.ix].order_ac & ORDER_DMG_DOWN) continue;
			set_friend.insert(mental.ix);
			vec_friend.push_back(mental.ix);
			set_all.insert(mental.ix);
			vec_all.push_back(mental.ix);
		}
	}
	//
	size_t p1ix = app->m_Control.player1;
	set_friend.insert(p1ix);
	vec_friend.push_back(p1ix);
	set_all.insert(p1ix);
	vec_all.push_back(p1ix);
	//
	rebuild_map();
}
//
template <typename T_app>
void ai_info<T_app>::rebuild_map()
{
	if (csv_value_is_empty(app->m_Scene.get_misc["terrain_info"])) {
		int ix_land = app->m_Inst.m_PlaneLandIx;
		map_x = app->m_Inst.m_BoundW.extents_x(ix_land);
		map_z = app->m_Inst.m_BoundW.extents_z(ix_land);
	}
	else {
		app->m_Scene.terrain1;
		map_x = app->m_Scene.terrain1.get_Width();
		map_z = app->m_Scene.terrain1.get_Depth();
	}
	map_x /= 2.0f;
	map_z /= 2.0f;
}
//
template <typename T_app>
void ai_info<T_app>::update()
{
	for (auto &mental: app->m_AiInterf.mental_scene) {
		if (!(app->m_Inst.m_Troll[mental.ix].order_stat & ORDER_ST_DESTROYED)) continue;
		if (!set_all.count(mental.ix)) continue;
		set_all.erase(mental.ix);
		vec_all.erase(std::remove(vec_all.begin(), vec_all.end(), mental.ix));
		if (set_enemy.count(mental.ix)) {
			set_enemy.erase(mental.ix);
			vec_enemy.erase(std::remove(vec_enemy.begin(), vec_enemy.end(), mental.ix));
		}
		if (set_friend.count(mental.ix)) {
			set_friend.erase(mental.ix);
			vec_friend.erase(std::remove(vec_friend.begin(), vec_friend.end(), mental.ix));
		}
	}
}
//
template <typename T_app>
void ai_info<T_app>::clip_pos(float &x, float &z)
{
	float px = map_x-0.3f;
	float pz = map_z-0.3f;
	math().calc_clamp(x, -px, px);
	math().calc_clamp(z, -pz, pz);
}
//
template <typename T_app>
int ai_info<T_app>::random_an_enemy(const size_t &caller)
{
	caller;
	size_t range = vec_enemy.size();
	assert(range != 0);
	int offset = rand() % range;
	return static_cast<int>(vec_enemy[offset]);
}
template <typename T_app>
int ai_info<T_app>::random_an_enemy_screen(const size_t &caller)
{
	int ret = static_cast<int>(caller);
	std::vector<size_t> vec_screen;
	for (auto &enemy: vec_enemy) {
		if (app->m_Inst.m_Stat[enemy].phy.is_on_screen) vec_screen.push_back(enemy);
	}
	size_t range = vec_screen.size();
	if (range == 0) return ret;
	int offset = rand() % range;
	ret = static_cast<int>(vec_screen[offset]);
	return ret;
}
//
template <typename T_app>
int ai_info<T_app>::random_an_all(const size_t &caller)
{
	caller;
	size_t range = vec_all.size();
	assert(range != 0);
	int offset = rand() % range;
	return  static_cast<int>(vec_all[offset]);
}
//
template <typename T_app>
int ai_info<T_app>::random_an_all_screen(const size_t &caller)
{
	int ret = static_cast<int>(caller);
	std::vector<size_t> vec_screen;
	for (auto &one: vec_all) {
		if (app->m_Inst.m_Stat[one].phy.is_on_screen) vec_screen.push_back(one);
	}
	size_t range = vec_screen.size();
	if (range == 0) return ret;
	int offset = rand() % range;
	ret = static_cast<int>(vec_screen[offset]);
	return ret;
}
//
template <typename T_app>
int ai_info<T_app>::select_nearby_enemy(const size_t &caller)
{
	XMVECTOR core = XMLoadFloat3(&app->m_Inst.m_BoundW.center(caller));
	int ret = -1;
	float length_min = 1000.0f;
	for (auto &enemy: vec_enemy) {
		XMVECTOR compar = XMLoadFloat3(&app->m_Inst.m_BoundW.center(enemy));
		XMVECTOR diff = XMVectorSubtract(core, compar);
		float length = XMVectorGetX(XMVector3LengthEst(diff));
		if (length < length_min) {
			length_min = length;
			ret = static_cast<int>(enemy);
		}
	}
	return ret;
}
//
}
#endif