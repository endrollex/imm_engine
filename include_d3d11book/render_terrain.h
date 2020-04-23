////////////////
// render_terrain.h
// library from [Introduction to 3D Game Programming with DirectX 11 by Frank Luna]
////////////////
////////////////
#ifndef RENDER_TERRAIN_H
#define RENDER_TERRAIN_H
#include "mesh_d3d_util.h"
#include "pipe_vertex.h"
#include "util_camera.h"
#include "render_shadow.h"
#include "DDSTextureLoader.h"
#include <DirectXPackedVector.h>
using namespace DirectX::PackedVector;
#include <algorithm>
namespace imm
{
////////////////
// terrain
////////////////
////////////////
class terrain
{
public:
	struct init_info
	{
		std::wstring height_map_filename;
		std::wstring layer_map_filename0;
		std::wstring layer_map_filename1;
		std::wstring layer_map_filename2;
		std::wstring layer_map_filename3;
		std::wstring layer_map_filename4;
		std::wstring blend_map_filename;
		float height_scale;
		UINT heightmap_width;
		UINT heightmap_height;
		float cell_spacing;
		float tex_scale;
	};
	terrain();
	~terrain();
	float get_Width() const;
	float get_Depth() const;
	float get_Height(float x, float z) const;
	XMMATRIX get_World() const;
	void set_World(CXMMATRIX M);
	void init(
		ID3D11Device *device,
		ID3D11DeviceContext *dc,
		const init_info &init_info1);
	void draw(
		ID3D11DeviceContext *dc,
		const camera &cam1,
		light_dir lights[3],
		shadow_map *shadow,
		XMMATRIX &shadow_transform);
	bool is_initialized();
private:
	void load_heightmap();
	void smooth();
	bool in_bounds(int i, int j);
	float average(int i, int j);
	void calc_AllPatchBoundsY();
	void calc_PatchBoundsY(UINT i, UINT j);
	void build_QuadPatchVB(ID3D11Device *device);
	void build_QuadPatchIB(ID3D11Device *device);
	void build_HeightmapSRV(ID3D11Device *device);
public:
	init_info m_Info;
private:
	// Divide heightmap into patches such that each patch has CellsPerPatch cells
	// and CellsPerPatch+1 vertices.  Use 64 so that if we tessellate all the way
	// to 64, we use all the data from the heightmap.
	static const int m_CellsPerPatch = 64;
	ID3D11Buffer *m_QuadPatchVB;
	ID3D11Buffer *m_QuadPatchIB;
	ID3D11ShaderResourceView *m_LayerMapArraySRV;
	ID3D11ShaderResourceView *m_BlendMapSRV;
	ID3D11ShaderResourceView *m_HeightMapSRV;
	UINT m_NumPatchVertices;
	UINT m_NumPatchQuadFaces;
	UINT m_NumPatchVertRows;
	UINT m_NumPatchVertCols;
	XMFLOAT4X4 m_World;
	material m_Mat;
	std::vector<XMFLOAT2> m_PatchBoundsY;
	std::vector<float> m_Heightmap;
};
}
#endif