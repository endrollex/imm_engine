////////////////
// ai_skill_pool.cc
// by Huang Yiting
////////////////
////////////////
#include "ai_skill_pool.h"
#include "imm_ptr.h"
namespace imm
{
////////////////
// skill_pool
////////////////
////////////////
skill_pool::skill_pool()
{
	;
}
//
void skill_pool::inquiry(const std::string &model, const char &symbol, const size_t &index)
{
	if (PTR->m_Inst.m_Steering[index].dump_act.command.size() != 0) return;
	assert(PTR->m_Control.atk.ski_data.count(model));
	assert(PTR->m_Control.atk.ski_data[model].chunk.count(symbol));
	auto skill_p = &PTR->m_Control.atk.ski_data[model];
	int start_ix = skill_p->chunk[symbol];
	size_t chunk_size = 0;
	bool stop = false;
	while (!stop) {
		++chunk_size;
		PTR->m_Inst.m_Steering[index].dump_act.push_atk(ORDER_ATK_X, symbol);
		if (skill_p->next_ix[start_ix] == -1) stop = true;
		++start_ix;
		if (chunk_size > 20) stop = true;
	}
}
//
}