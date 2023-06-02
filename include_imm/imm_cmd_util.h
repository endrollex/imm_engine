////////////////
// imm_cmd_util.h
// by Huang Yiting
////////////////
////////////////
#ifndef IMM_CMD_UTIL_H
#define IMM_CMD_UTIL_H
#include "stru_load_help.h"
#include <atomic>
namespace imm
{
////////////////
// atomic_wstring
////////////////
////////////////
struct atomic_wstring
{
	std::recursive_mutex mutex1;
	std::wstring str;
	template <typename T_wadde>
	atomic_wstring &operator+=(const T_wadde &str2);
	std::string get_string();
	void pop_back();
	template <typename T_itor>
	atomic_wstring &assign(const T_itor &first, const T_itor &last);
	template <typename T_wstr>
	atomic_wstring &assign(const T_wstr &str2);
	size_t size();
	std::wstring::iterator begin();
	std::wstring::iterator end();
	const wchar_t* c_str();
	void clear();
};
//
template <typename T_wadde>
atomic_wstring &atomic_wstring::operator+=(const T_wadde &str2)
{
	std::lock_guard<std::recursive_mutex> lock(mutex1);
	str += str2;
	return *this;
}
//
std::string atomic_wstring::get_string()
{
	std::lock_guard<std::recursive_mutex> lock(mutex1);
	return txtutil::wstr_to_str(str);
}
//
void atomic_wstring::pop_back()
{
	std::lock_guard<std::recursive_mutex> lock(mutex1);
	str.pop_back();
}
//
template <typename T_itor>
atomic_wstring &atomic_wstring::assign(const T_itor &first, const T_itor &last)
{
	std::lock_guard<std::recursive_mutex> lock(mutex1);
	str.assign(first, last);
	return *this;
}
//
template <typename T_wstr>
atomic_wstring &atomic_wstring::assign(const T_wstr &str2)
{
	std::lock_guard<std::recursive_mutex> lock(mutex1);
	str.assign(str2);
	return *this;
}
//
size_t atomic_wstring::size()
{
	std::lock_guard<std::recursive_mutex> lock(mutex1);
	return str.size();
}
//
std::wstring::iterator atomic_wstring::begin()
{
	std::lock_guard<std::recursive_mutex> lock(mutex1);
	return str.begin();
}
//
std::wstring::iterator atomic_wstring::end()
{
	std::lock_guard<std::recursive_mutex> lock(mutex1);
	return str.end();
}
//
const wchar_t* atomic_wstring::c_str()
{
	std::lock_guard<std::recursive_mutex> lock(mutex1);
	return str.c_str();
}
//
void atomic_wstring::clear()
{
	std::lock_guard<std::recursive_mutex> lock(mutex1);
	str.clear();
}
////////////////
// is_file_exist
////////////////
////////////////
bool is_file_exist(std::string file)
{
	std::ifstream check;
	check.open(file);
	bool is_ok = check.good();
    if (!is_ok) {
		std::cout << "Not found: " << file << std::endl;
	}
	check.close();
	return is_ok;
}
////////////////
// m3d_util_b3m
////////////////
////////////////
void m3d_util_b3m_list(std::string lua_file = "m3d_utility.lua")
{
	// no init tex_mgr, it is dummy
	if (!is_file_exist(lua_file)) return;
	texture_mgr tex_mgr;
	lua_reader l_reader;
	l_reader.loadfile(IMM_PATH["script"]+lua_file);
	bin_m3d model_bin;
	std::vector<std::vector<std::string>> model_m3d;
	l_reader.vec2d_str_from_table("csv_model_input", model_m3d);
	std::wstring path_tex(txtutil::str_to_wstr(IMM_PATH["texture"]));
	auto it = model_m3d.begin()+1;
	int cnt = 0;
	while (it != model_m3d.end()) {
		std::ifstream fin(IMM_PATH["input"]+(*it)[1]);
		bool is_open = fin.is_open();
		fin.close();
		if (!is_open) {
			++it;
			continue;
		}
		basic_model model_bas;
		skinned_model model_ski;
		bool is_skinned = false;
		l_reader.assign_bool(is_skinned, (*it)[2]);
		bool is_tex_ok = true;
		std::string out_b3m = IMM_PATH["output"]+(*it)[0]+".b3m";
		if (is_skinned) {
			model_ski.set(nullptr, tex_mgr, IMM_PATH["input"]+(*it)[1], path_tex);
			is_tex_ok = model_bin.write_to_bin(model_ski, out_b3m);
		}
		else {
			model_bas.set(nullptr, tex_mgr, IMM_PATH["input"]+(*it)[1], path_tex);
			is_tex_ok = model_bin.write_to_bin(model_bas, out_b3m);
		}
		if (!is_tex_ok) {
			std::cout << "ERROR: " << (*it)[0]+".b3m" << " miss texture?" << std::endl;
			return;
		}
		std::string m3d_name((*it)[1].begin(), (*it)[1].end());
		std::cout << m3d_name << " to " << (*it)[0]+".b3m" <<  " exported OK" << std::endl;
		++cnt;
		++it;
	}
	std::cout << std::to_string(cnt) << " files completed." << std::endl;
}
//
void m3d_util_b3m(std::string m3d_name, const bool &is_skinned, std::string out_name = "")
{
	texture_mgr tex_mgr;
	bin_m3d model_bin;
	std::wstring path_tex(txtutil::str_to_wstr(IMM_PATH["texture"]));
	basic_model model_bas;
	skinned_model model_ski;
	std::ifstream fin(IMM_PATH["input"]+m3d_name);
	bool is_open = fin.is_open();
	fin.close();
	if (!is_open) {
		fin.open(IMM_PATH["input"]+m3d_name+".m3d");
		is_open = fin.is_open();
		fin.close();
		if (!is_open) {
			std::cout << "ERROR: filename: " << m3d_name << " not found." << std::endl;
			return;
		}
		else m3d_name += ".m3d";
	}
	std::string b3m_name(m3d_name);
	if (out_name != "") b3m_name = out_name;
	if (b3m_name.substr(b3m_name.size()-4) == ".m3d") b3m_name = b3m_name.substr(0, b3m_name.size()-4);
	if (b3m_name.substr(b3m_name.size()-4) == ".b3m") b3m_name = b3m_name.substr(0, b3m_name.size()-4);
	b3m_name += ".b3m";
	bool is_tex_ok = true;
	std::string out_b3m = IMM_PATH["output"]+b3m_name;
	if (is_skinned) {
		model_ski.set(nullptr, tex_mgr, IMM_PATH["input"]+m3d_name, path_tex);
		is_tex_ok = model_bin.write_to_bin(model_ski, out_b3m);
	}
	else {
		model_bas.set(nullptr, tex_mgr, IMM_PATH["input"]+m3d_name, path_tex);
		is_tex_ok = model_bin.write_to_bin(model_bas, out_b3m);
	}
	if (!is_tex_ok) {
		std::cout << "ERROR: " << m3d_name << " miss texture?" << std::endl;
		return;
	}
	std::cout << m3d_name << " to " << b3m_name << " exported OK" << std::endl;
}
//
void m3d_util_b3m_read(std::string b3m_name, ID3D11Device *device)
{
	texture_mgr tex_mgr;
	tex_mgr.init(device);
	bin_m3d model_bin;
	basic_model model_bas;
	skinned_model model_ski;
	std::ifstream fin(IMM_PATH["input"]+b3m_name);
	bool is_open = fin.is_open();
	fin.close();
	if (!is_open) {
		fin.open(IMM_PATH["input"]+b3m_name+".b3m");
		is_open = fin.is_open();
		fin.close();
		if (!is_open) {
			std::cout << "ERROR: filename: " << b3m_name << " not found." << std::endl;
			return;
		}
		else b3m_name += ".b3m";
	}
	bool is_skinned = model_bin.is_skinned(b3m_name);
	std::string input_b3m = IMM_PATH["input"]+b3m_name;
	if (is_skinned) {
		model_bin.read_from_bin(model_ski, input_b3m, tex_mgr);
	}
	else {
		model_bin.read_from_bin(model_bas, input_b3m, tex_mgr);
	}
	std::cout << b3m_name << " tested OK" << std::endl;
}
}
#endif