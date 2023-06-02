////////////////
// ai_skill_pool.h
// by Huang Yiting
////////////////
////////////////
#ifndef AI_SKILL_POOL_H
#define AI_SKILL_POOL_H
#include "control_state.h"
namespace imm
{
////////////////
// skill_pool
////////////////
////////////////
struct skill_pool
{
    skill_pool();
	void inquiry(const std::string &model, const char &symbol, const size_t &index);
};
//
}
#endif