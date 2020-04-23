////////////////
// debug_console.h
// echo info with a console window, test and debug for app
// by Huang Yiting
////////////////
////////////////
#ifndef DEBUG_CONSOLE_H
#define DEBUG_CONSOLE_H
#include <windows.h>
#include <Wincon.h>
#include <iostream>
#include <sstream>
#include <string>
#include <DirectXMath.h>
using namespace DirectX;
namespace imm
{
////////////////
// ECHO_MODE
////////////////
////////////////
enum ECHO_MODE
{
	ECHO_MODE_ENDL  = 0,
	ECHO_MODE_FLUSH = 1,
};
//
////////////////
// type conversions
////////////////
////////////////
std::ostream &operator<<(std::ostream &os, const XMFLOAT2 dest)
{
	os << "(" << dest.x << ", " << dest.y << ")";
	return os;
}
//
std::ostream &operator<<(std::ostream &os, const XMFLOAT3 dest)
{
	os << "(" << dest.x << ", " << dest.y << ", " << dest.z << ")";
	return os;
}
//
std::ostream &operator<<(std::ostream &os, const XMFLOAT4 dest)
{
	os << "(" << dest.x << ", " << dest.y << ", " << dest.z << ", " << dest.w << ")";
	return os;
}
//
std::ostream& operator<<(std::ostream &os, const XMFLOAT4X4 m)
{
	os << "####\n";
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) os << m(i, j) << ' ';
		if (i < 3) os << '\n';
		else os << "####" << std::flush;
	}
	return os;
}
//
std::ostream& operator<<(std::ostream &os, CXMVECTOR v)
{
	XMFLOAT4 dest;
	XMStoreFloat4(&dest, v);
	os << "(" << dest.x << ", " << dest.y << ", " << dest.z << ", " << dest.w << ")";
	return os;
}
//
std::ostream& operator<<(std::ostream &os, CXMMATRIX m)
{
	os << "####\n";
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) os << m.r[i].m128_f32[j] << ' ';
		if (i < 3) os << '\n';
		else os << "####" << std::flush;
	}
	return os;
}
////////////////
// echo_init
////////////////
////////////////
static bool DEBUG_IS_CONSOLE = false;
static FILE *DEBUG_FILE = nullptr;
void echo_init()
{
	if (!DEBUG_IS_CONSOLE) {
		assert(AllocConsole());
		freopen_s(&DEBUG_FILE, "CONOUT$", "w", stdout);
		DEBUG_IS_CONSOLE = true;
	}
}
////////////////
// echo_close
////////////////
////////////////
void echo_close()
{
	if (DEBUG_IS_CONSOLE) {
		assert(fclose(DEBUG_FILE) == 0);
		DEBUG_IS_CONSOLE = false;
	}
}
////////////////
// echo function
////////////////
////////////////
template <typename T>
void echo(const T &info, const bool &echo_mode_value = ECHO_MODE_ENDL)
{
	echo_init();
	if (echo_mode_value == ECHO_MODE_FLUSH) std::cout << info << std::flush;
	else std::cout << info << std::endl;
}
//
template <typename T>
void echo_wstr(const T &info)
{
	echo_init();
	std::wostringstream outs;
	outs << info;
	std::wcout << outs.str() << std::endl;
}
//
void echo_wostr(std::wostringstream &info)
{
	echo_init();
	std::wcout << info.str() << std::endl;
}
//
template <typename T>
void echo_message(const T &info)
{
	std::wostringstream outs;
	outs << info;
	MessageBox(0, outs.str().c_str(), L"Message", 0);}
}
#endif