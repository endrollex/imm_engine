////////////////
// m3dtob3m
////////////////
////////////////
#ifndef UNICODE
#define UNICODE
#endif
#include "imm_cmd_util.h"
//
#pragma comment(lib, "User32")
#pragma comment(lib, "Effects11")
#pragma comment(lib, "DirectXTK")
#pragma comment(lib, "lua")
#pragma comment(lib, "d3d11book.lib")
////////////////
//
////////////////
////////////////
int main(int argc, char* argv[])
{
	imm::IS_STANDALONE_M3DTOB3M = true;
	imm::IMM_PATH["script"] = "";
	imm::IMM_PATH["output"] = "";
	imm::IMM_PATH["input"] = "";
	if (argc > 1) {
		std::string argv1 = argv[1];
		if (argv1 == "/uselist") {
			imm::m3d_util_b3m();
			return 0;
		}
		if (argv1 == "/basic" || argv1 == "/skinned") {
			if (argc == 2) {
				std::cout << "ERROR: Please specify .m3d filename." << std::endl;
				return 0;
			}
			bool is_skinned = false;
			if (argv1 == "/skinned") is_skinned = true;
			std::string m3d_name = argv[2];
			imm::m3d_util_b3m(m3d_name, is_skinned);
			return 0;
		}
	}
	if (argc) {
		int bit = sizeof(int*)*8;
		std::string bit_version = " x"+std::to_string(bit);
		std::cout << "M3D to B3M Tool" << bit_version << std::endl;
		std::cout << "immature engine Utility" << std::endl;
		std::cout << std::endl;
		std::cout << "Usage: m3dtob3m <options> <.m3d_filename>" << std::endl;
		std::cout << std::endl;
		std::cout << "    /uselist        use m3d_list.csv declare which files to convert"<< std::endl;
		std::cout << "    /basic          convert signle basic .m3d file to .b3m file" << std::endl;
		std::cout << "    /skinned        convert signle skinned .m3d file to .b3m file" << std::endl;
	}
	return 0;
}