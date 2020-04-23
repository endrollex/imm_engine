////////////////
// pipe_vertex.h
// library from [Introduction to 3D Game Programming with DirectX 11 by Frank Luna]
////////////////
////////////////
#ifndef PIPE_VERTEX_H
#define PIPE_VERTEX_H
#include "pipe_effect.h"
namespace imm
{
////////////////
// Basic 32-byte vertex structure.
////////////////
////////////////
namespace vertex
{
struct basic32
{
	vertex::basic32():
		pos(0.0f, 0.0f, 0.0f), normal(0.0f, 0.0f, 0.0f), tex(0.0f, 0.0f) {;}
	vertex::basic32(const XMFLOAT3 &p, const XMFLOAT3 &n, const XMFLOAT2 &uv):
		pos(p), normal(n), tex(uv) {;}
	vertex::basic32(float px, float py, float pz, float nx, float ny, float nz, float u, float v):
		pos(px, py, pz), normal(nx, ny, nz), tex(u,v) {;}
	XMFLOAT3 pos;
	XMFLOAT3 normal;
	XMFLOAT2 tex;
};
//
struct pntt
{
	XMFLOAT3 pos;
	XMFLOAT3 normal;
	XMFLOAT2 tex;
	XMFLOAT3 tangent_u;
};
//
struct pntt2
{
	XMFLOAT3 pos;
	XMFLOAT3 normal;
	XMFLOAT2 tex;
	XMFLOAT4 tangent_u;
};
//
struct terrain
{
	XMFLOAT3 pos;
	XMFLOAT2 tex;
	XMFLOAT2 bounds_y;
};
//
struct particle
{
	XMFLOAT3 init_pos;
	XMFLOAT3 init_vel;
	XMFLOAT2 size;
	float age;
	unsigned int type;
};
//
struct pntt_skinned
{
	XMFLOAT3 pos;
	XMFLOAT3 normal;
	XMFLOAT2 tex;
	XMFLOAT4 tangent_u;
	XMFLOAT3 weights;
	BYTE bone_indices[4];
};
//
struct pos_color
{
	XMFLOAT3 pos;
	XMFLOAT4 color;
};
} // namespace vertex
////////////////
// input_layout_desc
////////////////
////////////////
class input_layout_desc
{
public:
	// Init like const int A::a[4] = {0, 1, 2, 3}; in .cpp file.
	static const D3D11_INPUT_ELEMENT_DESC m_Pos[1];
	static const D3D11_INPUT_ELEMENT_DESC m_Basic32[3];
	static const D3D11_INPUT_ELEMENT_DESC m_PNTT[4];
	static const D3D11_INPUT_ELEMENT_DESC m_PNTT2[4];
	static const D3D11_INPUT_ELEMENT_DESC m_PNTTSkinned[6];
	static const D3D11_INPUT_ELEMENT_DESC m_Terrain[3];
	static const D3D11_INPUT_ELEMENT_DESC m_Particle[5];
	static const D3D11_INPUT_ELEMENT_DESC m_PosColor[2];
};
////////////////
// input_layouts
////////////////
////////////////
class input_layouts
{
public:
	static void init_all(ID3D11Device *device);
	static void destroy_all();
	static ID3D11InputLayout *m_Pos;
	static ID3D11InputLayout *m_Basic32;
	static ID3D11InputLayout *m_PNTT;
	static ID3D11InputLayout *m_PNTT2;
	static ID3D11InputLayout *m_PNTTSkinned;
	static ID3D11InputLayout *m_Terrain;
	static ID3D11InputLayout *m_Particle;
	static ID3D11InputLayout *m_PosColor;
};
}
#endif