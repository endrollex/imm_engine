////////////////
// render_wave.h
// library from [Introduction to 3D Game Programming with DirectX 11 by Frank Luna]
////////////////
////////////////
#ifndef RENDER_WAVE_H
#define RENDER_WAVE_H
#include "pipe_effect.h"
#include "mesh_d3d_util.h"
namespace imm
{
////////////////
// gpu_wave
////////////////
////////////////
class gpu_wave
{
public:
	gpu_wave();
	~gpu_wave();
	UINT row_count() const;
	UINT column_count() const;
	UINT vertex_count() const;
	UINT triangle_count() const;
	float width() const;
	float depth() const;
	ID3D11ShaderResourceView* get_displacement_map();
	// How many groups do we need to dispatch to cover the wave grid.
	// Note that m_NumRows and m_NumCols should be divisible by 16
	// so there is no remainder when we divide into thread groups.
	void init(ID3D11Device* device, UINT m, UINT n, float dx, float dt, float speed, float damping);
	void update(ID3D11DeviceContext* dc, float dt, float total_time);
	void disturb(ID3D11DeviceContext* dc, UINT i, UINT j, float magnitude);
private:
	void build_wave_simulation_views(ID3D11Device* device);
	UINT m_NumRows;
	UINT m_NumCols;
	UINT m_VertexCount;
	UINT m_TriangleCount;
	// Simulation constants we can precompute.
	float m_K[3];
	float m_TimeStep;
	float m_SpatialStep;
	float m_TimeBase;
	ID3D11ShaderResourceView *m_WavesPrevSolSRV;
	ID3D11ShaderResourceView *m_WavesCurrSolSRV;
	ID3D11ShaderResourceView *m_WavesNextSolSRV;
	ID3D11ShaderResourceView *m_RandomTexSRV;
	ID3D11UnorderedAccessView *m_WavesPrevSolUAV;
	ID3D11UnorderedAccessView *m_WavesCurrSolUAV;
	ID3D11UnorderedAccessView *m_WavesNextSolUAV;
	gpu_wave(const gpu_wave &rhs);
	gpu_wave &operator=(const gpu_wave &rhs);
};
}
//
#endif