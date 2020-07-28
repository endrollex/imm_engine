////////////////
// util_basic.h
// library from [Introduction to 3D Game Programming with DirectX 11 by Frank Luna]
////////////////
////////////////
#ifndef UTIL_BASIC_H
#define UTIL_BASIC_H
#include "debug_console.h"
#include "d3d11book.h"
using namespace DirectX;
namespace imm
{
////////////////
// sfx
////////////////
////////////////
struct sfx
{
	static std::string Attack;
	static std::string Prepare01;
	static std::string Lightning;
	static std::string Click;
	static std::string Empty;
	static std::string AttackGuard;
	static std::string Breaking;
	static std::string Whoosh;
};
//
std::string sfx::Attack      = "attack";
std::string sfx::Prepare01   = "magic_spell";
std::string sfx::Lightning   = "magic_lightning";
std::string sfx::Click       = "click";
std::string sfx::Empty       = "empty";
std::string sfx::AttackGuard = "attack_guard";
std::string sfx::Breaking    = "breaking";
std::string sfx::Whoosh      = "whoosh";
//
////////////////
// IMM_PATH_init
////////////////
////////////////
void IMM_PATH_init()
{
	IMM_PATH["shader"]  = "shader\\";
	IMM_PATH["model"]   = "asset\\";
	IMM_PATH["texture"] = "asset\\";
	IMM_PATH["media"]   = "asset\\media\\";
	IMM_PATH["script"]  = "script\\";
	IMM_PATH["output"]  = "script\\m3d_input_output\\";
	IMM_PATH["input"]   = "script\\m3d_input_output\\";
	IMM_PATH["text"]    = "script\\text_en\\";
	IMM_PATH["dump"]    = "script\\scene_dump\\";
}
}
#endif