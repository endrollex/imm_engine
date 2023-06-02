////////////////
// m3dtob3m
////////////////
////////////////
#ifndef UNICODE
#define UNICODE
#endif
#include "imm_cmd_util.h"
#include "imm_base.h"
#include <DirectXColors.h>
#include <cassert>
//
#pragma comment(lib, "User32")
#pragma comment(lib, "Imm32")
#pragma comment(lib, "d3d11")
#pragma comment(lib, "D2d1")
#pragma comment(lib, "Effects11")
#pragma comment(lib, "DirectXTK")
#pragma comment(lib, "lua")
#pragma comment(lib, "d3d11book.lib")
////////////////
//
////////////////
////////////////
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
//
template <typename T_clock>
void cout_spend_time(T_clock &time1)
{
	auto time2 = std::chrono::steady_clock::now();
	std::chrono::duration<double> diff = time2-time1;
	std::cout<< "spend time: "  << diff.count() << " s"<< std::endl;
}
//
int main(int argc, char* argv[])
{
	imm_app win;
	if (!win.init_win()) return 0;
	if (!win.init_d3d()) return 0;
	//
	imm::IS_STANDALONE_M3DTOB3M = true;
	imm::IMM_PATH["script"] = "";
	imm::IMM_PATH["output"] = "";
	imm::IMM_PATH["input"] = "";
	imm::IMM_PATH["texture"] = "";
	if (argc > 1) {
		auto time1 = std::chrono::steady_clock::now();
		std::string argv1 = argv[1];
		if (argv1 == "/lua") {
			if (argc == 2) imm::m3d_util_b3m_list();
			else imm::m3d_util_b3m_list(argv[2]);
			cout_spend_time(time1);
			return 0;
		}
		if (argv1 == "/basic" || argv1 == "/skinned") {
			if (argc == 2) {
				std::cout << "ERROR: Please specify .m3d filename." << std::endl;
				return 0;
			}
			std::string out_name = "";
			if (argc > 3) out_name = argv[3];
			bool is_skinned = false;
			if (argv1 == "/skinned") is_skinned = true;
			std::string m3d_name = argv[2];
			imm::m3d_util_b3m(m3d_name, is_skinned, out_name);
			cout_spend_time(time1);
			return 0;
		}
		if (argv1 == "/test") {
			if (argc == 2) {
				std::cout << "ERROR: Please specify .b3m filename." << std::endl;
				return 0;
			}
			std::string m3d_name = argv[2];
			imm::m3d_util_b3m_read(m3d_name, win.m_D3DDevice);
			cout_spend_time(time1);
			return 0;
		}
	}
	if (argc) {
		int bit = sizeof(int*)*8;
		std::string bit_version = " x"+std::to_string(bit);
		std::cout << "M3D to B3M Tool" << bit_version << std::endl;
		std::cout << "immature engine Utility" << std::endl;
		std::cout << std::endl;
		std::cout << "Usage: m3dtob3m <option> <file1> <file2>" << std::endl;
		std::cout << std::endl;
		std::cout << "    /lua            use <m3d_utility.lua> declare which files to convert"<< std::endl;
		std::cout << "    /basic          convert signle basic <input.m3d> to <output.b3m>" << std::endl;
		std::cout << "    /skinned        convert signle skinned <input.m3d> to <output.b3m>" << std::endl;
		std::cout << "    /test           test <input.b3m>" << std::endl;
	}
	return 0;
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
