////////////////
// imm_ptr.h
// by Huang Yiting
////////////////
////////////////
#ifndef IMM_PTR_H
#define IMM_PTR_H
#include "imm_app.h"
namespace imm
{
////////////////
// PTR
////////////////
////////////////
class imm_app;
static imm_app *PTR;
void ptr_app_init(imm_app *app_in)
{
	PTR = app_in;
}
//
}
#endif