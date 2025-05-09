////////////////
// stru_build_inst.h
// by Huang Yiting
////////////////
////////////////
#ifndef STRU_BUILD_INST_H
#define STRU_BUILD_INST_H
#include "stru_model_mgr.h"
namespace imm
{
////////////////
// inst_build
////////////////
////////////////
template <typename T_app>
struct inst_build
{
	inst_build();
	void init(T_app *app_in);
	void remove_all();
	bool inst_dump();
	bool copy_player1();
	T_app *app;
	std::vector<size_t> vindex;
	std::set<std::string> model;
	size_t copy_cnt;
	size_t copy_cnt_max;
};
//
template <typename T_app>
inst_build<T_app>::inst_build():
	app(nullptr),
	vindex(),
	model(),
	copy_cnt(0),
	copy_cnt_max(10000)
{
	;
}
//
template <typename T_app>
void inst_build<T_app>::init(T_app *app_in)
{
	app = app_in;
}
//
template <typename T_app>
void inst_build<T_app>::remove_all()
{
	copy_cnt = 0;
}
//
template <typename T_app>
bool inst_build<T_app>::inst_dump()
{
	vindex.clear();
	model.clear();
	std::vector<std::string> csvdata;
	csvdata.push_back("name,model,scale,rotation,pos_x,pos_y,pos_z\r\n");
	size_t index = 0;
	for (auto &stat: app->m_Inst.m_Stat) {
		++index;
		if (stat.type != MODEL_SIMPLE_P) continue;
		if (stat.phy.intera_tp & PHY_INTERA_FIXED_INVIS) continue;
		vindex.push_back(index);
		if (model.find(*stat.get_ModelName()) == model.end()) model.insert(*stat.get_ModelName());
		csvdata.push_back(std::string());
		csvdata.back().append(app->m_Inst.m_IndexMap[index-1]);
		csvdata.back().append(",");
		csvdata.back().append(*stat.get_ModelName());
		csvdata.back().append(",");
		csvdata.back().append("1");
		csvdata.back().append(",");
		csvdata.back().append("0");
		csvdata.back().append(",");
		csvdata.back().append(std::to_string(stat.get_World()->_41));
		csvdata.back().append(",");
		csvdata.back().append(std::to_string(stat.get_World()->_42));
		csvdata.back().append(",");
		csvdata.back().append(std::to_string(stat.get_World()->_43));
		csvdata.back().append("\r\n");
		//
	}
	std::string file_name = IMM_PATH["dump"]+"dump1.csv";
	std::ofstream outfile (file_name, std::ofstream::binary);
	if (!outfile.is_open()) {
		outfile.close();
		return false;
	}
	for (auto &line: csvdata) {
		outfile.write(line.c_str(), line.size());
	}
	outfile.close();
	return true;
}
//
template <typename T_app>
bool inst_build<T_app>::copy_player1()
{
	++copy_cnt;
	assert(copy_cnt < copy_cnt_max);
	size_t p1ix = app->m_Control.player1;
	std::string p1name = app->m_Inst.m_IndexMap[p1ix];
	std::string copyname = p1name;
	if (copyname.size() > 6) copyname.resize(6);
	std::string namecnt = std::to_string(copy_cnt);
	for (int ix = 0; ix < 4-namecnt.size(); ++ix) namecnt = "0"+namecnt;
	copyname = copyname+namecnt;
	app->m_Inst.copy_instance(p1name, copyname);
	XMFLOAT3 copypos;
	copypos.x = app->m_Inst.m_Stat[p1ix].get_World()->_41;
	copypos.y = app->m_Inst.m_Stat[p1ix].get_World()->_42;
	copypos.z = app->m_Inst.m_Stat[p1ix].get_World()->_43;
	copypos.y += app->m_Inst.m_BoundL.extents_y(p1ix)*2.0f;
	size_t copyix = app->m_Inst.m_NameMap[copyname];
	app->m_Inst.m_Stat[copyix].set_WorldPos(copypos);
	app->m_Control.set_player1(copyix);
	return true;
}
//
}
#endif