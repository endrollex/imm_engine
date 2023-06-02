////////////////
// stru_lua_help.h
// by Huang Yiting
////////////////
////////////////
#ifndef STRU_LUA_HELP_H
#define STRU_LUA_HELP_H
#include <iostream>
#include "util_basic.h"
#include "lua.hpp"
#include <mutex>
#include <regex>
namespace imm
{
////////////////
// lua_reader
////////////////
////////////////
class lua_reader
{
public:
	lua_reader();
	~lua_reader() {if (L) lua_close(L);}
	std::recursive_mutex mutex1;
	lua_State *L;
	std::string file_name;
	void loadfile(const std::string &fname);
	void clear_stack();
	void map_from_string(std::map<std::string, std::string> &map_str);
	void map_from_table(const std::string &table_name, std::map<std::string, std::string> &map_str);
	void vec2d_str_from_table(
		const std::string &table_name,
		std::vector<std::vector<std::string>> &vec2d_str);
	void vec2d_str_from_table_wstr(
		const std::string &table_name,
		std::vector<std::vector<std::wstring>> &vec2d_wstr);
	template <typename T_bool>
	void assign_bool(T_bool &b_out, const std::string &str_in);
	bool is_nil(const std::string &var_name);
private:
	lua_reader(const lua_reader &rhs);
	lua_reader &operator=(const lua_reader &rhs);
};
//
lua_reader::lua_reader():
	mutex1(),
	L(nullptr),
	file_name("")
{
	;
}
//
void lua_reader::loadfile(const std::string &fname)
{
	std::lock_guard<std::recursive_mutex> lock(mutex1);
	if (L) lua_close(L);
	file_name = fname;
	L = luaL_newstate();
	luaL_openlibs(L);
	if (luaL_loadfile(L, file_name.c_str()) || lua_pcall(L, 0, 0, 0)) {
		std::string err_str("Lua error: ");
		err_str += lua_tostring(L, -1);
		ERROR_MESA(err_str.c_str());
		return;
	}
}
//
void lua_reader::clear_stack()
{
	std::lock_guard<std::recursive_mutex> lock(mutex1);
	int n = lua_gettop(L);
	lua_pop(L, n);
}
//
void lua_reader::map_from_string(std::map<std::string, std::string> &map_str)
{
	std::lock_guard<std::recursive_mutex> lock(mutex1);
	if (!L) {assert(false); abort();}
	for (auto map_it = map_str.begin(); map_it != map_str.end(); ++map_it) {
		lua_getglobal(L, map_it->first.c_str());
		if (lua_type(L, -1) == LUA_TSTRING) map_it->second = (lua_tostring(L, -1));
		else {
			std::string err_str("Lua load ");
			err_str += file_name;
			err_str += " error: ";
			err_str += map_it->first;
			ERROR_MESA(err_str.c_str());
			return;
		}
	}
}
//
void lua_reader::map_from_table(const std::string &table_name, std::map<std::string, std::string> &map_str)
{
	std::lock_guard<std::recursive_mutex> lock(mutex1);
	if (!L) {assert(false); abort();}
	map_str.clear();
	lua_getglobal(L, table_name.c_str());
	if (!lua_istable(L, -1)) {assert(false); abort();}
	int l_ix = lua_gettop(L);
	lua_pushnil(L);
	while (lua_next(L, l_ix) != 0) {
		if (!(lua_type(L, -2) == LUA_TSTRING || lua_type(L, -2) == LUA_TNUMBER)) {assert(false); abort();}
		if (lua_type(L, -1) != LUA_TSTRING) {assert(false); abort();}
		if (lua_type(L, -2) == LUA_TNUMBER) {
			map_str[std::to_string(static_cast<int>(lua_tonumber(L, -2)))] = lua_tostring(L, -1);
		}
		else {
			map_str[lua_tostring(L, -2)] = lua_tostring(L, -1);
		}
		lua_pop(L, 1);
	}
}
//
void lua_reader::vec2d_str_from_table(
	const std::string &table_name,
	std::vector<std::vector<std::string>> &vec2d_str)
{
	std::lock_guard<std::recursive_mutex> lock(mutex1);
	if (!L) {assert(false); abort();}
	// must be empyt
	vec2d_str = std::vector<std::vector<std::string>>();
	int ix_1 = 1;
	lua_getglobal(L, table_name.c_str());
	if (!lua_istable(L, -1)) {
		std::string err_str("Lua table name error: ");
		err_str += table_name;
		ERROR_MESA(err_str.c_str());
	}
	lua_pushstring(L, "dummy");
	int offset = 0;
	while (!lua_isnil(L, -1)) {
		lua_pop(L, 1);
		lua_pushinteger(L, ix_1++);
		lua_gettable(L, -2);
		int ix_2 = 1;
		vec2d_str.push_back(std::vector<std::string>());
		if (lua_istable(L, -1)) {
			lua_pushstring(L, "dummy");
			while (!lua_isnil(L, -1)) {
				lua_pop(L, 1);
				lua_pushinteger(L, ix_2++);
				lua_gettable(L, -2);
				if (lua_isstring(L, -1)) {
					vec2d_str[ix_1-2+offset].push_back(lua_tostring(L, -1));
				}
			}
			lua_pop(L, 1);
			// if blank line, delete it
			if (vec2d_str.back().size() == 0) {
				vec2d_str.pop_back();
				--offset;
			}
		}
	}
	vec2d_str.pop_back();
}
//
void lua_reader::vec2d_str_from_table_wstr(
	const std::string &table_name,
	std::vector<std::vector<std::wstring>> &vec2d_wstr)
{
	vec2d_wstr = std::vector<std::vector<std::wstring>>();
	std::vector<std::vector<std::string>> vec2d_str;
	vec2d_str_from_table(table_name, vec2d_str);
	for (size_t ix = 0; ix < vec2d_str.size(); ++ix) {
		vec2d_wstr.push_back(std::vector<std::wstring>());
		for (size_t ix_2 = 0; ix_2 < vec2d_str[ix].size(); ++ix_2)
			vec2d_wstr[ix].push_back(txtutil::str_to_wstr(vec2d_str[ix][ix_2]));
	}
}
//
template <typename T_bool>
void lua_reader::assign_bool(T_bool &b_out, const std::string &str_in)
{
	if (str_in == "true" || str_in == "1") b_out = true;
	else b_out = false;
}
//
bool lua_reader::is_nil(const std::string &var_name)
{
	std::lock_guard<std::recursive_mutex> lock(mutex1);
	if (!L) {assert(false); abort();}
	lua_getglobal(L, var_name.c_str());
	if (lua_isnil(L, -1)) return true;
	return false;
}
////////////////
// rotation_xyz
////////////////
////////////////
struct rotation_xyz
{
	rotation_xyz();
	rotation_xyz(const std::string &xyz);
	XMMATRIX get_Matrix();
	bool is_RotXneg90Y180();
	XMFLOAT3 roation;
	XMFLOAT3 degree;
	bool is_assigned_xyz;
};
//
rotation_xyz::rotation_xyz():
	roation(0.0f, 0.0f, 0.0f),
	degree(0.0f, 0.0f, 0.0f),
	is_assigned_xyz(false)
{
	;
}
//
rotation_xyz::rotation_xyz(const std::string &xyz)
{
	roation = XMFLOAT3(0.0f, 0.0f, 0.0f);
	degree = XMFLOAT3(0.0f, 0.0f, 0.0f);
	std::vector<std::string::size_type> pos;
	pos.push_back(xyz.find_first_of("Xx"));
	pos.push_back(xyz.find_first_of("Yy"));
	pos.push_back(xyz.find_first_of("Zz"));
	//"0123456789" will check converting, stof() first part of string must be a number
	std::string s_number("0123456789");
	for (size_t ix = 0; ix != pos.size(); ++ix) {
		if (pos[ix] != std::string::npos && pos[ix] < xyz.size()-1) {
			std::string::size_type offset = 0;
			if (xyz.substr(pos[ix]+1).find_first_of("+-") == 0) offset = 1;
			if (xyz.substr(pos[ix]+1+offset).find_first_of(s_number) == 0) {
				switch (ix) {
				case 0:
					roation.x = std::stof(xyz.substr(pos[ix]+1));
					degree.x = roation.x/180.0f*XM_PI;
					break;
				case 1:
					roation.y = std::stof(xyz.substr(pos[ix]+1));
					degree.y = roation.y/180.0f*XM_PI;
					break;
				case 2:
					roation.z = std::stof(xyz.substr(pos[ix]+1));
					degree.z = roation.z/180.0f*XM_PI;
					break;
				}
			}
		}
	}
	is_assigned_xyz = true;
}
//
XMMATRIX rotation_xyz::get_Matrix()
{
	XMMATRIX R = XMMatrixIdentity();
	// if rotation 90 degrees about X axis, OBB test will be wrong, but 90.01 degrees is OK, gimbal lock?
	if (abs(degree.x) > 0.01f) R = XMMatrixMultiply(R, XMMatrixRotationX(degree.x+0.01f));
	if (abs(degree.y) > 0.01f) R = XMMatrixMultiply(R, XMMatrixRotationY(degree.y));
	if (abs(degree.z) > 0.01f) R = XMMatrixMultiply(R, XMMatrixRotationZ(degree.z));
	return R;
}
//
bool rotation_xyz::is_RotXneg90Y180()
{
	assert(is_assigned_xyz);
	if (math::float_is_equal(roation.x, -90.0f) && math::float_is_equal(roation.y, 180.0f)) {
		return true;
	}
	return false;
}
////////////////
// csv_string_to_float
////////////////
////////////////
std::vector<float> csv_string_to_float(const std::string &str, const size_t &count, const std::string &separator = " ")
{
	std::vector<float> rt;
	std::string subject = str;
	if (separator != " ") subject = std::regex_replace(str, std::regex(separator), " ");
	std::string::size_type pos = 0, pos_get;
	for (size_t ix = 0; ix != count; ++ix) {
		rt.push_back(std::stof(subject.substr(pos), &pos_get));
		pos += pos_get;
	}
	return rt;
}
////////////////
// csv_value_is_empty
////////////////
////////////////
bool csv_value_is_empty(const std::string &str)
{
	if (str == "none") return true;
	if (str == "0") return true;
	if (str == "empty") return true;
	if (str == "") return true;
	return false;
}
////////////////
// csv_str_split
////////////////
////////////////
std::vector<std::string> csv_str_split(const std::string &str, const char &sep)
{
	std::vector<std::string> ret;
	std::istringstream ss(str);
	std::string token;
	while(std::getline(ss, token, sep)) {
		ret.push_back(token);
	}
	return ret;
}
////////////////
// lua_config
////////////////
////////////////
template <typename T_app>
struct lua_config
{
	lua_config();
	lua_config(T_app *app_in);
	void init_additional();
	T_app *m_App;
};
//
template <typename T_app>
lua_config<T_app>::lua_config():
	m_App(nullptr)
{
	;
}
//
template <typename T_app>
lua_config<T_app>::lua_config(T_app *app_in)
{
	m_App = app_in;
	std::map<std::string, std::string> g_map;
	g_map["is_windowed_fullscreen"] = "";
	g_map["is_4xmsaa"] = "";
	g_map["is_sync_interval"] = "";
	g_map["is_lock_frame_rate"] = "";
	g_map["lock_frame_rate"] = "";
	g_map["loading_wait"] = "";
	lua_reader l_reader;
	l_reader.loadfile(IMM_PATH["script"]+"imm_config.lua");
	l_reader.map_from_string(g_map);
	l_reader.assign_bool(m_App->m_FullScreen, g_map["is_windowed_fullscreen"]);
	l_reader.assign_bool(m_App->m_Enable4xMsaa, g_map["is_4xmsaa"]);
	l_reader.assign_bool(m_App->m_IsSyncInterval, g_map["is_sync_interval"]);
	l_reader.assign_bool(m_App->m_IsLockFrameRate, g_map["is_lock_frame_rate"]);
	double frame_rate = std::stod(g_map["lock_frame_rate"]);
	if (frame_rate > 9.9) m_App->m_FrameDeltaLock = 1.0/frame_rate;
	if (m_App->m_IsSyncInterval) m_App->m_IsLockFrameRate = false;
	m_App->m_Scene.loading_wait = std::stof(g_map["loading_wait"]);
}
template <typename T_app>
void lua_config<T_app>::init_additional()
{
	std::map<std::string, std::string> g_map;
	g_map["is_slient"] = "";
	lua_reader l_reader;
	l_reader.loadfile(IMM_PATH["script"]+"imm_config.lua");
	l_reader.map_from_string(g_map);
	l_reader.assign_bool(m_App->m_Cmd.is_slient, g_map["is_slient"]);
}
}
#endif