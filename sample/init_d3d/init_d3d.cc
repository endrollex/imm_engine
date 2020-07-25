////////////////
// init_d3d
////////////////
////////////////
#ifndef UNICODE
#define UNICODE
#endif
#include "imm_base.h"
#include <DirectXColors.h>
#pragma comment(lib, "User32")
#pragma comment(lib, "Imm32")
#pragma comment(lib, "d3d11")
#pragma comment(lib, "D2d1")
#pragma comment(lib, "DirectXTK")
#pragma comment(lib, "Effects11")
#pragma comment(lib, "d3d11book.lib")
using namespace imm;
class imm_app: public base_win<imm_app>
{
public:
	void update_scene(float dt) {DUMMY(dt);}
	void draw_scene();
	imm_app();
	~imm_app();
	bool init_imm();
};
////////////////
//
////////////////
////////////////
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR pCmdLine, int nCmdShow)
{
	DUMMY(hInstance);
	DUMMY(pCmdLine);
	imm_app win;
	if (!win.init_win()) return 0;
	if (!win.init_d3d()) return 0;
	ShowWindow(win.get_hwnd(), nCmdShow);
	UpdateWindow(win.get_hwnd());
	if (!win.init_imm()) return 0;
	return win.run();
}
//
void imm_app::draw_scene()
{
	assert(m_D3DDC);
	assert(m_SwapChain);
	m_D3DDC->ClearRenderTargetView(m_RenderTargetView, reinterpret_cast<const float*>(&Colors::Blue));
	m_D3DDC->ClearDepthStencilView(m_DepthStencilView, D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL, 1.0f, 0);
	HR(m_SwapChain->Present(0, 0));
}
//
imm_app::imm_app()
{
	;
}
//
imm_app::~imm_app()
{
	m_D3DDC->ClearState();
}
//
bool imm_app::init_imm()
{
	return true;
}

