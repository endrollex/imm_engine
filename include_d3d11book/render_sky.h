////////////////
// render_sky.h
// library from [Introduction to 3D Game Programming with DirectX 11 by Frank Luna]
////////////////
////////////////
#ifndef RENDER_SKY_H
#define RENDER_SKY_H
#include "util_camera.h"
#include "mesh_geometry_gen.h"
#include "pipe_vertex.h"
#include "DDSTextureLoader.h"
namespace imm
{
////////////////
// sky
////////////////
////////////////
class sky
{
public:
	sky();
	~sky();
	sky(ID3D11Device* device, const std::wstring& cubemap_filename, float sky_sphere_radius);
	ID3D11ShaderResourceView* get_CubeMapSRV();
	void draw(ID3D11DeviceContext* dc, const camera& cam1);
private:
	sky (const sky& rhs);
	sky &operator=(const sky& rhs);
	ID3D11Buffer* m_VB;
	ID3D11Buffer* m_IB;
	ID3D11ShaderResourceView* m_CubeMapSRV;
	UINT m_ISize;
};
//
}
#endif