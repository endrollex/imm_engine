////////////////
// pipe_render.h
// library from [Introduction to 3D Game Programming with DirectX 11 by Frank Luna]
////////////////
////////////////
#ifndef PIPE_RENDER_H
#define PIPE_RENDER_H
#include "util_math.h"
namespace imm
{
////////////////
// render
////////////////
////////////////
class render
{
public:
	static void init_all(ID3D11Device *device);
	static void destroy_all();
	// Rasterizer states
	static ID3D11RasterizerState *m_WireframeRS;
	static ID3D11RasterizerState *m_NoCullRS;
	static ID3D11RasterizerState *m_CullClockwiseRS;
	// Blend states
	static ID3D11BlendState *m_AlphaToCoverageBS;
	static ID3D11BlendState *m_TransparentBS;
	static ID3D11BlendState *m_NoRenderTargetWritesBS;
	// Depth/stencil states
	static ID3D11DepthStencilState *m_MarkMirrorDSS;
	static ID3D11DepthStencilState *m_DrawReflectionDSS;
	static ID3D11DepthStencilState *m_NoDoubleBlendDSS;
	static ID3D11DepthStencilState *m_LessEqualDSS;
	static ID3D11DepthStencilState *m_EqualsDSS;
};
//
}
#endif