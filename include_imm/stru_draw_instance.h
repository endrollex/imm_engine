////////////////
// stru_draw_instance.h
// by Huang Yiting
////////////////
////////////////
#ifndef STRU_DRAW_INSTANCE_H
#define STRU_DRAW_INSTANCE_H
#include <d3d11.h>
#include "d3dx11effect.h"
#include <vector>
#include <DirectXMath.h>
using namespace DirectX;
////////////////
// draw_inst_basic
////////////////
////////////////
namespace imm
{
template <typename fx_type, typename model_type>
void draw_inst_basic(
	ID3D11DeviceContext *D3DDC,
	ID3DX11EffectTechnique *tech,
	D3DX11_TECHNIQUE_DESC &tech_desc,
	fx_type &fx,
	std::vector<model_type> &model_inst,
	XMMATRIX &view_proj,
	XMMATRIX &shadow_transform,
	XMMATRIX &to_tex_space
)
{
	XMMATRIX world;
	XMMATRIX world_inv_transpose;
	XMMATRIX world_view_proj;
	tech->GetDesc(&tech_desc);
	for(UINT p = 0; p < tech_desc.Passes; ++p) {
		for(UINT ix = 0; ix < model_inst.size(); ++ix) {
			if (!model_inst[ix].is_appear()) continue;
			world = XMLoadFloat4x4(&model_inst[ix].world);
			world_inv_transpose = math::inverse_transpose(world);
			world_view_proj = world*view_proj;
			fx->set_World(world);
			fx->set_WorldInvTranspose(world_inv_transpose);
			fx->set_WorldViewProj(world_view_proj);
			fx->set_WorldViewProjTex(world_view_proj*to_tex_space);
			fx->set_ShadowTransform(world*shadow_transform);
			fx->set_TexTransform(XMMatrixIdentity());
			for(UINT subset = 0; subset < model_inst[ix].model->m_SubsetCount; ++subset) {
				fx->set_Material(model_inst[ix].model->m_Mat[subset]);
				fx->set_DiffuseMap(model_inst[ix].model->m_DiffuseMapSRV[subset]);
				fx->set_NormalMap(model_inst[ix].model->m_NormalMapSRV[subset]);
				tech->GetPassByIndex(p)->Apply(0, D3DDC);
				model_inst[ix].model->m_ModelMesh.draw(D3DDC, subset);
			}
		}
	}
}
////////////////
// draw_inst_basic_shadow
////////////////
////////////////
template <typename fx_type, typename model_type>
void draw_inst_basic_shadow(
	ID3D11DeviceContext *D3DDC,
	ID3DX11EffectTechnique *tech,
	D3DX11_TECHNIQUE_DESC &tech_desc,
	fx_type &fx,
	std::vector<model_type> &model_inst,
	bool is_alpha,
	XMMATRIX &view_proj
)
{
	XMMATRIX world;
	XMMATRIX world_inv_transpose;
	XMMATRIX world_view_proj;
	tech->GetDesc(&tech_desc);
	for(UINT p = 0; p < tech_desc.Passes; ++p) {
		for(UINT ix = 0; ix < model_inst.size(); ++ix) {
			if (!model_inst[ix].is_appear()) continue;
			world = XMLoadFloat4x4(&model_inst[ix].world);
			world_inv_transpose = math::inverse_transpose(world);
			world_view_proj = world*view_proj;
			fx->set_World(world);
			fx->set_WorldInvTranspose(world_inv_transpose);
			fx->set_WorldViewProj(world_view_proj);
			fx->set_TexTransform(XMMatrixIdentity());
			if (!is_alpha) tech->GetPassByIndex(p)->Apply(0, D3DDC);
			for(UINT subset = 0; subset < model_inst[ix].model->m_SubsetCount; ++subset) {
				if (is_alpha) {
					fx->set_DiffuseMap(model_inst[ix].model->m_DiffuseMapSRV[subset]);
					tech->GetPassByIndex(p)->Apply(0, D3DDC);
				}
				model_inst[ix].model->m_ModelMesh.draw(D3DDC, subset);
			}
		}
	}
}
////////////////
// draw_inst_skinned
////////////////
////////////////
template <typename fx_type, typename model_type>
void draw_inst_skinned(
	ID3D11DeviceContext *D3DDC,
	ID3DX11EffectTechnique *tech,
	D3DX11_TECHNIQUE_DESC &tech_desc,
	fx_type &fx,
	std::vector<model_type> &model_inst,
	XMMATRIX &view_proj,
	XMMATRIX &shadow_transform,
	XMMATRIX &to_tex_space,
	const std::vector<instance_stat> &stat
)
{
	XMMATRIX world;
	XMMATRIX world_inv_transpose;
	XMMATRIX world_view_proj;
	tech->GetDesc(&tech_desc);
	for(UINT p = 0; p < tech_desc.Passes; ++p) {
		for(UINT ix = 0; ix < model_inst.size(); ++ix) {
			if (!model_inst[ix].is_appear()) continue;
			world = XMLoadFloat4x4(&model_inst[ix].world);
			world_inv_transpose = math::inverse_transpose(world);
			world_view_proj = world*view_proj;
			fx->set_World(world);
			fx->set_WorldInvTranspose(world_inv_transpose);
			fx->set_WorldViewProj(world_view_proj);
			fx->set_WorldViewProjTex(world_view_proj*to_tex_space);
			fx->set_ShadowTransform(world*shadow_transform);
			fx->set_TexTransform(XMMatrixIdentity());
			fx->set_BoneTransforms(
				&model_inst[ix].final_transforms[0],
				static_cast<int>(model_inst[ix].final_transforms.size()));
			for(UINT subset = 0; subset < model_inst[ix].model->m_SubsetCount; ++subset) {
				fx->set_Material(model_inst[ix].model->m_Mat[subset]);
				if (stat[model_inst[ix].stat_ix].ex_tex_info.is_change_tex) {
					fx->set_DiffuseMap(stat[model_inst[ix].stat_ix].ex_tex_info.resource);
				}
				else fx->set_DiffuseMap(model_inst[ix].model->m_DiffuseMapSRV[subset]);
				fx->set_NormalMap(model_inst[ix].model->m_NormalMapSRV[subset]);
				tech->GetPassByIndex(p)->Apply(0, D3DDC);
				model_inst[ix].model->m_ModelMesh.draw(D3DDC, subset);
			}
		}
	}
}
////////////////
// draw_inst_skinned_shadow
////////////////
////////////////
template <typename fx_type, typename model_type>
void draw_inst_skinned_shadow(
	ID3D11DeviceContext *D3DDC,
	ID3DX11EffectTechnique *tech,
	D3DX11_TECHNIQUE_DESC &tech_desc,
	fx_type &fx,
	std::vector<model_type> &model_inst,
	bool is_alpha,
	XMMATRIX &view_proj
)
{
	XMMATRIX world;
	XMMATRIX world_inv_transpose;
	XMMATRIX world_view_proj;
	tech->GetDesc(&tech_desc);
	for(UINT p = 0; p < tech_desc.Passes; ++p) {
		for(UINT ix = 0; ix < model_inst.size(); ++ix) {
			if (!model_inst[ix].is_appear()) continue;
			world = XMLoadFloat4x4(&model_inst[ix].world);
			world_inv_transpose = math::inverse_transpose(world);
			world_view_proj = world*view_proj;
			fx->set_World(world);
			fx->set_WorldInvTranspose(world_inv_transpose);
			fx->set_WorldViewProj(world_view_proj);
			fx->set_TexTransform(XMMatrixIdentity());
			fx->set_BoneTransforms(
				&model_inst[ix].final_transforms[0],
				static_cast<int>(model_inst[ix].final_transforms.size()));
			if (!is_alpha) tech->GetPassByIndex(p)->Apply(0, D3DDC);
			for(UINT subset = 0; subset < model_inst[ix].model->m_SubsetCount; ++subset) {
				if (is_alpha) {
					fx->set_DiffuseMap(model_inst[ix].model->m_DiffuseMapSRV[subset]);
					tech->GetPassByIndex(p)->Apply(0, D3DDC);
				}
				model_inst[ix].model->m_ModelMesh.draw(D3DDC, subset);
			}
		}
	}
}
////////////////
// draw_inst_simple_pntt
////////////////
////////////////
template <typename fx_type, typename fx_type2, typename model_type>
void draw_inst_simple_pntt(
	ID3D11DeviceContext *D3DDC,
	ID3DX11EffectTechnique *tech,
	ID3DX11EffectTechnique *tech_no_tex,
	D3DX11_TECHNIQUE_DESC &tech_desc,
	fx_type &fx,
	fx_type2 &fx_no_tex,
	std::vector<model_type> &model_inst,
	XMMATRIX &view_proj,
	XMMATRIX &shadow_transform,
	XMMATRIX &to_tex_space
)
{
	XMMATRIX world;
	XMMATRIX world_inv_transpose;
	XMMATRIX world_view_proj;
	XMMATRIX tex_transform;
	for(UINT ix = 0; ix < model_inst.size(); ++ix) {
		if (!model_inst[ix].is_appear()) continue;
		world = XMLoadFloat4x4(&model_inst[ix].world);
		world_inv_transpose = math::inverse_transpose(world);
		world_view_proj = world*view_proj;
		tex_transform = XMLoadFloat4x4(&model_inst[ix].tex_transform);
		if (model_inst[ix].is_textrue) {
		tech->GetDesc(&tech_desc);
		for(UINT p = 0; p < tech_desc.Passes; ++p) {
			fx->set_World(world);
			fx->set_WorldInvTranspose(world_inv_transpose);
			fx->set_WorldViewProj(world_view_proj);
			fx->set_WorldViewProjTex(world_view_proj*to_tex_space);
			fx->set_ShadowTransform(world*shadow_transform);
			fx->set_TexTransform(tex_transform);
			fx->set_Material(model_inst[ix].model->m_Mat[model_inst[ix].subid]);
			fx->set_DiffuseMap(model_inst[ix].model->m_DiffuseMapSRV[model_inst[ix].subid]);
			fx->set_NormalMap(model_inst[ix].model->m_NormalMapSRV[model_inst[ix].subid]);
			tech->GetPassByIndex(p)->Apply(0, D3DDC);
			model_inst[ix].model->m_ModelMesh.draw_segment(D3DDC, model_inst[ix].subid);
			}
		}
		else {
		tech_no_tex->GetDesc(&tech_desc);
		for(UINT p = 0; p < tech_desc.Passes; ++p) {
			fx_no_tex->set_World(world);
			fx_no_tex->set_WorldInvTranspose(world_inv_transpose);
			fx_no_tex->set_WorldViewProj(world_view_proj);
			fx_no_tex->set_WorldViewProjTex(world_view_proj*to_tex_space);
			fx_no_tex->set_ShadowTransform(world*shadow_transform);
			fx_no_tex->set_TexTransform(tex_transform);
			fx_no_tex->set_Material(model_inst[ix].model->m_Mat[model_inst[ix].subid]);
			tech_no_tex->GetPassByIndex(p)->Apply(0, D3DDC);
			model_inst[ix].model->m_ModelMesh.draw_segment(D3DDC, model_inst[ix].subid);
			}
		}
	}
}
////////////////
// draw_inst_simple_pntt_shadow
////////////////
////////////////
template <typename fx_type, typename model_type>
void draw_inst_simple_pntt_shadow(
	ID3D11DeviceContext *D3DDC,
	ID3DX11EffectTechnique *tech,
	D3DX11_TECHNIQUE_DESC &tech_desc,
	fx_type &fx,
	std::vector<model_type> &model_inst,
	XMMATRIX &view_proj
)
{
	XMMATRIX world;
	XMMATRIX world_inv_transpose;
	XMMATRIX world_view_proj;
	tech->GetDesc(&tech_desc);
	for(UINT p = 0; p < tech_desc.Passes; ++p) {
		for(UINT ix = 0; ix < model_inst.size(); ++ix) {
			if (!model_inst[ix].is_appear()) continue;
			world = XMLoadFloat4x4(&model_inst[ix].world);
			world_inv_transpose = math::inverse_transpose(world);
			world_view_proj = world*view_proj;
			fx->set_World(world);
			fx->set_WorldInvTranspose(world_inv_transpose);
			fx->set_WorldViewProj(world_view_proj);
			fx->set_TexTransform(XMMatrixIdentity());
			tech->GetPassByIndex(p)->Apply(0, D3DDC);
			model_inst[ix].model->m_ModelMesh.draw_segment(D3DDC, model_inst[ix].subid);
		}
	}
}
//
}
#endif