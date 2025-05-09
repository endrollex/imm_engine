////////////////
// render_particle.h
// library from [Introduction to 3D Game Programming with DirectX 11 by Frank Luna]
////////////////
////////////////
#ifndef RENDER_PARTICLE_H
#define RENDER_PARTICLE_H
#include "util_camera.h"
#include "pipe_vertex.h"
namespace imm
{
////////////////
// inst_plasam
////////////////
////////////////
struct inst_plasam
{
	inst_plasam();
	void update(const float &dt);
	XMFLOAT3 pos;
	float count_down;
	bool is_first_run;
	size_t slot;
	const XMFLOAT3 *pos_ptr;
};
////////////////
// particle
////////////////
////////////////
class particle
{
public:
	particle();
	~particle();
	size_t get_ListSize() const;
	void set_EyePos(const XMFLOAT3 &eye_pos_w);
	void set_EmitPos(const XMFLOAT3 &emit_pos_w);
	void set_EmitDir(const XMFLOAT3 &emit_dir_w);
	void init(
		ID3D11Device *device,
		particle_effect *fx,
		ID3D11ShaderResourceView *tex_array_srv,
		ID3D11ShaderResourceView *random_tex_srv,
		UINT max_particles,
		const size_t &list_size,
		const int &special_type = 0);
	void update(float dt, float game_time);
	template <typename list_plasam>
	void draw_list(ID3D11DeviceContext *dc, const camera &cam1, list_plasam &list);
private:
	void build_VB(ID3D11Device *device);
	particle (const particle &rhs);
	particle &operator=(const particle &rhs);
	UINT m_MaxParticles;
	int m_SpecialType;
	float m_GameTime;
	float m_TimeStep;
	size_t m_ListSize;
	XMFLOAT3 m_EyePosW;
	XMFLOAT3 m_EmitPosW;
	XMFLOAT3 m_EmitDirW;
	particle_effect *m_FX;
	ID3D11Buffer *m_InitVB;
	std::vector<ID3D11Buffer*> m_DrawVBList;
	std::vector<ID3D11Buffer*> m_StreamOutVBList;	
	ID3D11ShaderResourceView *m_TexArraySRV;
	ID3D11ShaderResourceView *m_RandomTexSRV;
};
//
template <typename list_plasam>
void particle::draw_list(ID3D11DeviceContext *dc, const camera &cam1, list_plasam &list)
{
	XMMATRIX view_proj = cam1.get_ViewProj();
	// Set constants.
	m_FX->set_ViewProj(view_proj);
	m_FX->set_GameTime(m_GameTime);
	m_FX->set_TimeStep(m_TimeStep);
	m_FX->set_EyePosW(m_EyePosW);
	m_FX->set_EmitDirW(m_EmitDirW);
	m_FX->set_SpecialType(m_SpecialType);
	m_FX->set_TexArray(m_TexArraySRV);
	m_FX->set_RandomTex(m_RandomTexSRV);
	// Set IA stage.
	dc->IASetInputLayout(input_layouts::m_Particle);
	dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	UINT stride = sizeof(vertex::particle);
	UINT offset = 0;
	for (auto &inst: list) {
		m_FX->set_EmitPosW(inst.pos);
		// On the first pass, use the initialization VB.  Otherwise, use
		// the VB that contains the current particle list.
		if (inst.is_first_run) {
			dc->IASetVertexBuffers(0, 1, &m_InitVB, &stride, &offset);
		}
		else {
			dc->IASetVertexBuffers(0, 1, &m_DrawVBList[inst.slot], &stride, &offset);
		}
		// Draw the current particle list using stream-out only to update them.
		// The updated vertices are streamed-out to the target VB.
		dc->SOSetTargets(1, &m_StreamOutVBList[inst.slot], &offset);
		D3DX11_TECHNIQUE_DESC tech_desc;
		m_FX->m_StreamOutTech->GetDesc(&tech_desc);
		for(UINT p = 0; p < tech_desc.Passes; ++p) {
			m_FX->m_StreamOutTech->GetPassByIndex(p)->Apply(0, dc);
			if (inst.is_first_run) {
				dc->Draw(1, 0);
				inst.is_first_run = false;
			}
			else {
				dc->DrawAuto();
			}
		}
		// done streaming-out--unbind the vertex buffer
		ID3D11Buffer *bufferArray[1] = {0};
		dc->SOSetTargets(1, bufferArray, &offset);
		// ping-pong the vertex buffers
		std::swap(m_DrawVBList[inst.slot], m_StreamOutVBList[inst.slot]);
		// Draw the updated particle system we just streamed-out.
		dc->IASetVertexBuffers(0, 1, &m_DrawVBList[inst.slot], &stride, &offset);
		m_FX->m_DrawTech->GetDesc(&tech_desc);
		for(UINT p = 0; p < tech_desc.Passes; ++p) {
			m_FX->m_DrawTech->GetPassByIndex(p)->Apply(0, dc);
			dc->DrawAuto();
		}
	}
}
}
#endif