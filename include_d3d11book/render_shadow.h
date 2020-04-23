////////////////
// render_shadow.h
// library from [Introduction to 3D Game Programming with DirectX 11 by Frank Luna]
////////////////
////////////////
#ifndef RENDER_SHADOW_H
#define RENDER_SHADOW_H
#include "util_math.h"
namespace imm
{
////////////////
// shadow_map
////////////////
////////////////
class shadow_map
{
public:
	shadow_map(ID3D11Device *device, UINT width, UINT height);
	~shadow_map();
	ID3D11ShaderResourceView *get_DepthMapSRV();
	void bind_dsv_and_set_null_render_target(ID3D11DeviceContext *dc);
private:
	shadow_map(const shadow_map &rhs);
	shadow_map &operator=(const shadow_map &rhs);
	UINT m_Width;
	UINT m_Height;
	ID3D11ShaderResourceView *m_DepthMapSRV;
	ID3D11DepthStencilView *m_DepthMapDSV;
	D3D11_VIEWPORT m_Viewport;
};
}
#endif