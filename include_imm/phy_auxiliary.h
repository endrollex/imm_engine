////////////////
// phy_auxiliary.h
// by Huang Yiting
////////////////
////////////////
#ifndef PHY_AUXILIARY_H
#define PHY_AUXILIARY_H
#include "pipe_vertex.h"
#include "util_camera.h"
#include <DirectXColors.h>
namespace imm
{
////////////////
// phy_wireframe
////////////////
////////////////
template <typename T_app>
struct phy_wireframe
{
	phy_wireframe();
	~phy_wireframe();
	void remove_buffer();
	void remove_all();
	void init(T_app *appin);
	void rebuild_buffer();
	void draw();
	void switch_drawing();
	bool is_drawing;
	std::map<size_t, ID3D11Buffer*> box_collision;
	std::vector<ID3D11Buffer*> box_hit;
	std::map<size_t, ID3D11Buffer*> box_ai_bound;
	ID3D11Buffer *box_ib;
	T_app *app;
private:
	phy_wireframe(const phy_wireframe &rhs);
	phy_wireframe &operator=(const phy_wireframe &rhs);
};
//
template <typename T_app>
phy_wireframe<T_app>::phy_wireframe():
	is_drawing(false),
	box_ib(nullptr),
	app(nullptr)
{
	;
}
//
template <typename T_app>
phy_wireframe<T_app>::~phy_wireframe()
{
	remove_buffer();
	RELEASE_COM(box_ib);
}
//
template <typename T_app>
void phy_wireframe<T_app>::remove_buffer()
{
	for (auto &vb: box_collision) RELEASE_COM(vb.second);
	for (auto &vb: box_hit) RELEASE_COM(vb);
	for (auto &vb: box_ai_bound) RELEASE_COM(vb.second);
}
//
template <typename T_app>
void phy_wireframe<T_app>::remove_all()
{
	remove_buffer();
	box_collision.clear();
	box_hit.clear();
	box_ai_bound.clear();
}
//
template <typename T_app>
void phy_wireframe<T_app>::init(T_app *appin)
{
	app = appin;
	// Create the index buffer
	UINT indices[] = {
		// front face
		0, 1, 2,
		0, 2, 3,
		// back face
		4, 6, 5,
		4, 7, 6,
		// left face
		4, 5, 1,
		4, 1, 0,
		// right face
		3, 2, 6,
		3, 6, 7,
		// top face
		1, 5, 6,
		1, 6, 2,
		// bottom face
		4, 0, 3,
		4, 3, 7
	};
	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(UINT)*36;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	ibd.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA iinit_data;
	iinit_data.pSysMem = indices;
	HR(app->m_D3DDevice->CreateBuffer(&ibd, &iinit_data, &box_ib));
}
//
template <typename T_app>
void phy_wireframe<T_app>::rebuild_buffer()
{
	remove_all();
	// box_collision
	XMFLOAT4 color;
	for (size_t ix = 0; ix != app->m_Inst.m_BoundL.map.size(); ++ix) {
		XMFLOAT3 corners[8];
		if (app->m_Inst.m_BoundL.map[ix].first == PHY_BOUND_SPHERE) continue;
		if (app->m_Inst.m_BoundL.map[ix].first == PHY_BOUND_BOX) {
			app->m_Inst.m_BoundL.bd0[app->m_Inst.m_BoundL.map[ix].second].GetCorners(corners);
		}
		if (app->m_Inst.m_BoundL.map[ix].first == PHY_BOUND_ORI_BOX) {
			app->m_Inst.m_BoundL.bd1[app->m_Inst.m_BoundL.map[ix].second].GetCorners(corners);
		}
		//
		if (app->m_Inst.m_Stat[ix].property & INST_IS_ATTACH) color = XMFLOAT4(Colors::Orange);
		else color = XMFLOAT4(Colors::Yellow);
		vertex::pos_color vertices[] = {
			{corners[0], color},
			{corners[1], color},
			{corners[2], color},
			{corners[3], color},
			{corners[4], color},
			{corners[5], color},
			{corners[6], color},
			{corners[7], color}
		};
		box_collision[ix] = nullptr;
		D3D11_BUFFER_DESC vbd;
		vbd.Usage = D3D11_USAGE_IMMUTABLE;
		vbd.ByteWidth = sizeof(vertex::pos_color)*8;
		vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vbd.CPUAccessFlags = 0;
		vbd.MiscFlags = 0;
		vbd.StructureByteStride = 0;
		D3D11_SUBRESOURCE_DATA vinit_data;
		vinit_data.pSysMem = vertices;
		HR(app->m_D3DDevice->CreateBuffer(&vbd, &vinit_data, &box_collision[ix]));
	}
	// box_hit
	color = XMFLOAT4(Colors::Red);
	for (size_t ix = 0; ix != app->m_Hit.bbox_l.size(); ++ix) {
		XMFLOAT3 corners[8];
		app->m_Hit.bbox_l[ix].GetCorners(corners);
		vertex::pos_color vertices[] = {
			{corners[0], color},
			{corners[1], color},
			{corners[2], color},
			{corners[3], color},
			{corners[4], color},
			{corners[5], color},
			{corners[6], color},
			{corners[7], color}
		};
		box_hit.push_back(nullptr);
		D3D11_BUFFER_DESC vbd;
		vbd.Usage = D3D11_USAGE_IMMUTABLE;
		vbd.ByteWidth = sizeof(vertex::pos_color)*8;
		vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vbd.CPUAccessFlags = 0;
		vbd.MiscFlags = 0;
		vbd.StructureByteStride = 0;
		D3D11_SUBRESOURCE_DATA vinit_data;
		vinit_data.pSysMem = vertices;
		HR(app->m_D3DDevice->CreateBuffer(&vbd, &vinit_data, &box_hit.back()));
	}
	// ai_bound
	color = XMFLOAT4(Colors::Blue);
	for (auto &bound: app->m_Inst.m_Probe.geometry) {
		XMFLOAT3 corners[8];
		// AABB or OBB of m_Probe can be draw
		bound.second.AlertL.GetCorners(corners);
		//bound.second.OblongL.GetCorners(corners);
		//bound.second.ObshortL.GetCorners(corners);
		vertex::pos_color vertices[] = {
			{corners[0], color},
			{corners[1], color},
			{corners[2], color},
			{corners[3], color},
			{corners[4], color},
			{corners[5], color},
			{corners[6], color},
			{corners[7], color}
		};
		box_ai_bound[bound.first] = nullptr;
		D3D11_BUFFER_DESC vbd;
		vbd.Usage = D3D11_USAGE_IMMUTABLE;
		vbd.ByteWidth = sizeof(vertex::pos_color)*8;
		vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vbd.CPUAccessFlags = 0;
		vbd.MiscFlags = 0;
		vbd.StructureByteStride = 0;
		D3D11_SUBRESOURCE_DATA vinit_data;
		vinit_data.pSysMem = vertices;
		HR(app->m_D3DDevice->CreateBuffer(&vbd, &vinit_data, &box_ai_bound[bound.first]));
	}
}
//
template <typename T_app>
void phy_wireframe<T_app>::draw()
{
	if (!is_drawing) return;
	color_effect *color_fx = effects::m_PosColorFX;
	ID3DX11EffectTechnique *tech = color_fx->m_PosColorTech;
	UINT stride = sizeof(vertex::pos_color);
	UINT offset = 0;
	app->m_D3DDC->IASetInputLayout(input_layouts::m_PosColor);
	app->m_D3DDC->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	app->m_D3DDC->RSSetState(render::m_WireframeRS);
	app->m_D3DDC->IASetIndexBuffer(box_ib, DXGI_FORMAT_R32_UINT, 0);
	XMMATRIX view_proj = app->m_Cam.get_ViewProj();
	// Draw collision box
	for (size_t ix = 0; ix != app->m_Inst.m_BoundL.map.size(); ++ix) {
		if (app->m_Inst.m_BoundL.map[ix].first == PHY_BOUND_SPHERE) continue;
		if (!box_collision.count(ix)) continue;
		app->m_D3DDC->IASetVertexBuffers(0, 1, &box_collision[ix], &stride, &offset);
		XMFLOAT4X4 *inst_world = app->m_Inst.m_Stat[ix].get_World();
		XMMATRIX world = XMLoadFloat4x4(inst_world);
		XMMATRIX world_view_proj = XMMatrixMultiply(world, view_proj);
		color_fx->set_WorldViewProj(world_view_proj);
		D3DX11_TECHNIQUE_DESC tech_desc;
		tech->GetDesc(&tech_desc);
		for(UINT p = 0; p < tech_desc.Passes; ++p) {
			tech->GetPassByIndex(p)->Apply(0, app->m_D3DDC);
			// 36 indices for the box.
			app->m_D3DDC->DrawIndexed(36, 0, 0);
		}
	}
	// Draw hit box
	for (auto it_map = app->m_Hit.map_box_active.begin(); it_map != app->m_Hit.map_box_active.end(); ++it_map) {
		XMFLOAT4X4 *inst_world = app->m_Inst.m_Stat[it_map->first].get_World();
		XMMATRIX world = XMLoadFloat4x4(inst_world);
		for (auto it_box = it_map->second.begin(); it_box != it_map->second.end(); ++it_box) {
			app->m_D3DDC->IASetVertexBuffers(0, 1, &box_hit[it_box->second], &stride, &offset);
			size_t bone_ix =
				app->m_Hit.atk_model[*app->m_Inst.m_Stat[it_map->first].get_ModelName()].box[it_box->first].bone_ix;
			XMMATRIX bone_trans = XMLoadFloat4x4(
				app->m_Inst.m_Stat[it_map->first].get_FinalTransform(bone_ix)
			);
			XMMATRIX world_view_proj = XMMatrixMultiply(XMMatrixMultiply(bone_trans, world), view_proj);
			color_fx->set_WorldViewProj(world_view_proj);
			D3DX11_TECHNIQUE_DESC tech_desc;
			tech->GetDesc(&tech_desc);
			for(UINT p = 0; p < tech_desc.Passes; ++p) {
				tech->GetPassByIndex(p)->Apply(0, app->m_D3DDC);
				// 36 indices for the box.
				app->m_D3DDC->DrawIndexed(36, 0, 0);
			}
		}
	}
	// Draw ai bound
	for (auto &bound: app->m_Inst.m_Probe.geometry) {
		if (!bound.second.is_active) continue;
		size_t ix = bound.first;
		assert(box_ai_bound.count(ix));
		app->m_D3DDC->IASetVertexBuffers(0, 1, &box_ai_bound[ix], &stride, &offset);
		XMFLOAT4X4 *inst_world = app->m_Inst.m_Stat[ix].get_World();
		XMMATRIX world = XMLoadFloat4x4(inst_world);
		XMMATRIX world_view_proj = XMMatrixMultiply(world, view_proj);
		color_fx->set_WorldViewProj(world_view_proj);
		D3DX11_TECHNIQUE_DESC tech_desc;
		tech->GetDesc(&tech_desc);
		for(UINT p = 0; p < tech_desc.Passes; ++p) {
			tech->GetPassByIndex(p)->Apply(0, app->m_D3DDC);
			// 36 indices for the box.
			app->m_D3DDC->DrawIndexed(36, 0, 0);
		}
	}
	app->m_D3DDC->RSSetState(0);
}
//
template <typename T_app>
void phy_wireframe<T_app>::switch_drawing()
{
	is_drawing = !is_drawing;
	if (is_drawing) rebuild_buffer();
}
//
}
#endif