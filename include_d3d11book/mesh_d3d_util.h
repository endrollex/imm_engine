////////////////
// mesh_d3d_util.h
// library from [Introduction to 3D Game Programming with DirectX 11 by Frank Luna]
////////////////
////////////////
#ifndef MESH_D3D_UTIL_H
#define MESH_D3D_UTIL_H
#include "util_math.h"
#include "DDSTextureLoader.h"
namespace imm
{
////////////////
// create_Texture2DArraySRV
////////////////
////////////////
ID3D11ShaderResourceView *create_Texture2DArraySRV(
	ID3D11Device *device, ID3D11DeviceContext *context, std::vector<std::wstring> &filenames);
////////////////
// create_RandomTexture1DSRV
////////////////
////////////////
ID3D11ShaderResourceView *create_RandomTexture1DSRV(ID3D11Device* device);
////////////////
// create_RandomTexture1DWaveSRV
////////////////
////////////////
ID3D11ShaderResourceView *create_RandomTexture1DWaveSRV(ID3D11Device* device);
////////////////
// extract_frustum_planes
////////////////
////////////////
void extract_frustum_planes(XMFLOAT4 planes[6], CXMMATRIX M);
}
#endif