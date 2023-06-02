////////////////
// simple_secene
////////////////
////////////////
#ifndef UNICODE
#define UNICODE
#endif
#include "imm_app.cc"
#include "control_state.cc"
#include "control_math.cc"
#include "control_atk.cc"
#include "ai_steering.cc"
#include "phy_magic.cc"
#include "ai_skill_pool.cc"
//
#pragma comment(lib, "User32")
#pragma comment(lib, "Imm32")
#pragma comment(lib, "d3d11")
#pragma comment(lib, "D2d1")
#pragma comment(lib, "Dwrite")
#pragma comment(lib, "DirectXTK")
#pragma comment(lib, "DirectXTKAudioDX")
#pragma comment(lib, "Effects11")
#pragma comment(lib, "lua")
#pragma comment(lib, "Xinput1_3")
#pragma comment(lib, "d3d11book.lib")
////////////////
//
////////////////
////////////////
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR pCmdLine, int nCmdShow)
{
	DUMMY(pCmdLine);
	DUMMY(hInstance);
	imm::imm_app win;
	if (!win.init_win()) return 0;
	if (!win.init_d3d()) return 0;
	ShowWindow(win.get_hwnd(), nCmdShow);
	UpdateWindow(win.get_hwnd());
	if (!win.init_imm()) return 0;
	return win.run();
}
