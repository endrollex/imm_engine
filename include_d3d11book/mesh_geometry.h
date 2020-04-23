////////////////
// mesh_geometry.h
// library from [Introduction to 3D Game Programming with DirectX 11 by Frank Luna]
////////////////
////////////////
#ifndef MESH_GEOMETRY_H
#define MESH_GEOMETRY_H
#include "util_math.h"
namespace imm
{
////////////////
// mesh_geometry
////////////////
////////////////
class mesh_geometry
{
public:
	struct subset
	{
		subset();
		UINT id;
		UINT vertex_start;
		UINT vertex_count;
		UINT face_start;
		UINT face_count;
	};
public:
	mesh_geometry();
	~mesh_geometry();
	template <typename vertex_type>
	void set_Vertices(ID3D11Device *device, const vertex_type *vertices, UINT count);
	void set_Indices(ID3D11Device *device, const UINT *indices, UINT count);
	void set_SubsetTable(std::vector<subset> &subset_table);
	void draw(ID3D11DeviceContext *dc, UINT subsetid);
	void draw_segment(ID3D11DeviceContext *dc, UINT subsetid);
private:
	mesh_geometry(const mesh_geometry &rhs);
	mesh_geometry &operator=(const mesh_geometry &rhs);
	ID3D11Buffer *m_VB;
	ID3D11Buffer *m_IB;
	DXGI_FORMAT m_IndexBufferFormat; // Always 32-bit
	UINT m_VertexStride;
	std::vector<subset> m_SubsetTable;
};
//
template <typename vertex_type>
void mesh_geometry::set_Vertices(ID3D11Device *device, const vertex_type *vertices, UINT count)
{
	RELEASE_COM(m_VB);
	m_VertexStride = sizeof(vertex_type);
	D3D11_BUFFER_DESC vbd;
	vbd.Usage               = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth           = sizeof(vertex_type) * count;
	vbd.BindFlags           = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags      = 0;
	vbd.MiscFlags           = 0;
	vbd.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA vinit_data;
	vinit_data.pSysMem = vertices;
	if (device) HR(device->CreateBuffer(&vbd, &vinit_data, &m_VB));
}
}
#endif