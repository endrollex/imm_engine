////////////////
// phy_octree_tri.h
// library from [Introduction to 3D Game Programming with DirectX 11 by Frank Luna]
////////////////
////////////////
#ifndef PHY_OCTREE_TRI_H
#define PHY_OCTREE_TRI_H
#include <windows.h>
#include <DirectXMath.h>
#include <DirectXCollision.h>
#include <vector>
using namespace DirectX;
namespace imm
{
struct octree_node;
////////////////
// octree_tri
////////////////
////////////////
class octree_tri
{
public:
	octree_tri();
	~octree_tri();
	void build(const std::vector<XMFLOAT3>& vertices, const std::vector<UINT>& indices);
	bool ray_octree_intersect(FXMVECTOR ray_pos, FXMVECTOR ray_dir);
private:
	BoundingBox build_AABB();
	void build_Octree(octree_node* parent, const std::vector<UINT>& indices);
	bool ray_octree_intersect(octree_node* parent, FXMVECTOR ray_pos, FXMVECTOR ray_dir);
private:
	octree_node* m_Root;
	std::vector<XMFLOAT3> m_Vertices;
};
////////////////
// octree_node
////////////////
////////////////
struct octree_node
{
	BoundingBox bounds;
	// This will be empty except for leaf nodes.
	std::vector<UINT> indices;
	octree_node* children[8];
	bool is_leaf;
	octree_node() {
		for (int i = 0; i < 8; ++i) children[i] = 0;
		bounds.Center  = XMFLOAT3(0.0f, 0.0f, 0.0f);
		bounds.Extents = XMFLOAT3(0.0f, 0.0f, 0.0f);
		is_leaf = false;
	}
	~octree_node() {
		for (int i = 0; i < 8; ++i) {delete children[i]; children[i] = nullptr;}
	}
	// subdivides the bounding box of this node into eight subboxes (vMin[i], vMax[i]) for i = 0:7.
	void subdivide(BoundingBox box[8]) {
		XMFLOAT3 half_extent(
			0.5f*bounds.Extents.x,
			0.5f*bounds.Extents.y,
			0.5f*bounds.Extents.z);
		// "Top" four quadrants.
		box[0].Center  = XMFLOAT3(
			bounds.Center.x + half_extent.x,
			bounds.Center.y + half_extent.y,
			bounds.Center.z + half_extent.z);
		box[0].Extents = half_extent;
		box[1].Center  = XMFLOAT3(
			bounds.Center.x - half_extent.x,
			bounds.Center.y + half_extent.y,
			bounds.Center.z + half_extent.z);
		box[1].Extents = half_extent;
		box[2].Center  = XMFLOAT3(
			bounds.Center.x - half_extent.x,
			bounds.Center.y + half_extent.y,
			bounds.Center.z - half_extent.z);
		box[2].Extents = half_extent;
		box[3].Center  = XMFLOAT3(
			bounds.Center.x + half_extent.x,
			bounds.Center.y + half_extent.y,
			bounds.Center.z - half_extent.z);
		box[3].Extents = half_extent;
		// "Bottom" four quadrants.
		box[4].Center  = XMFLOAT3(
			bounds.Center.x + half_extent.x,
			bounds.Center.y - half_extent.y,
			bounds.Center.z + half_extent.z);
		box[4].Extents = half_extent;
		box[5].Center  = XMFLOAT3(
			bounds.Center.x - half_extent.x,
			bounds.Center.y - half_extent.y,
			bounds.Center.z + half_extent.z);
		box[5].Extents = half_extent;
		box[6].Center  = XMFLOAT3(
			bounds.Center.x - half_extent.x,
			bounds.Center.y - half_extent.y,
			bounds.Center.z - half_extent.z);
		box[6].Extents = half_extent;
		box[7].Center  = XMFLOAT3(
			bounds.Center.x + half_extent.x,
			bounds.Center.y - half_extent.y,
			bounds.Center.z - half_extent.z);
		box[7].Extents = half_extent;
	}
};
//
octree_tri::octree_tri()
	: m_Root(0)
{
	;
}
//
octree_tri::~octree_tri()
{
	delete m_Root;
	m_Root = nullptr;
}
//
void octree_tri::build(const std::vector<XMFLOAT3>& vertices, const std::vector<UINT>& indices)
{
	// Cache a copy of the vertices.
	m_Vertices = vertices;
	// build AABB to contain the scene mesh.
	BoundingBox scene_bounds = build_AABB();
	// Allocate the root node and set its AABB to contain the scene mesh.
	m_Root = new octree_node();
	m_Root->bounds = scene_bounds;
	build_Octree(m_Root, indices);
}
//
bool octree_tri::ray_octree_intersect(FXMVECTOR ray_pos, FXMVECTOR ray_dir)
{
	return ray_octree_intersect(m_Root, ray_pos, ray_dir);
}
//
BoundingBox octree_tri::build_AABB()
{
	XMVECTOR vmin = XMVectorReplicate(+FLT_MAX);
	XMVECTOR vmax = XMVectorReplicate(-FLT_MAX);
	for (size_t i = 0; i < m_Vertices.size(); ++i) {
		XMVECTOR P = XMLoadFloat3(&m_Vertices[i]);
		vmin = XMVectorMin(vmin, P);
		vmax = XMVectorMax(vmax, P);
	}
	BoundingBox bounds;
	XMVECTOR C = 0.5f*(vmin + vmax);
	XMVECTOR E = 0.5f*(vmax - vmin);
	XMStoreFloat3(&bounds.Center, C);
	XMStoreFloat3(&bounds.Extents, E);
	return bounds;
}
//
void octree_tri::build_Octree(octree_node* parent, const std::vector<UINT>& indices)
{
	size_t tri_count = indices.size() / 3;
	if (tri_count < 60) {
		parent->is_leaf = true;
		parent->indices = indices;
	}
	else {
		parent->is_leaf = false;
		BoundingBox subbox[8];
		parent->subdivide(subbox);
		for (int i = 0; i < 8; ++i) {
			// Allocate a new subnode.
			parent->children[i] = new octree_node();
			parent->children[i]->bounds = subbox[i];
			// Find triangles that intersect this node's bounding box.
			std::vector<UINT> intersected_triangle_indices;
			for (size_t j = 0; j < tri_count; ++j) {
				UINT i0 = indices[j*3+0];
				UINT i1 = indices[j*3+1];
				UINT i2 = indices[j*3+2];
				XMVECTOR v0 = XMLoadFloat3(&m_Vertices[i0]);
				XMVECTOR v1 = XMLoadFloat3(&m_Vertices[i1]);
				XMVECTOR v2 = XMLoadFloat3(&m_Vertices[i2]);
				if (subbox[i].Intersects(v0, v1, v2)) {
					intersected_triangle_indices.push_back(i0);
					intersected_triangle_indices.push_back(i1);
					intersected_triangle_indices.push_back(i2);
				}
			}
			// Recurse.
			build_Octree(parent->children[i], intersected_triangle_indices);
		}
	}
}
//
bool octree_tri::ray_octree_intersect(octree_node* parent, FXMVECTOR ray_pos, FXMVECTOR ray_dir)
{
	// Recurs until we find a leaf node (all the triangles are in the leaves).
	if (!parent->is_leaf) {
		for (int i = 0; i < 8; ++i) {
			// Recurse down this node if the ray hit the child's box.
			float t;
			if (parent->children[i]->bounds.Intersects(ray_pos, ray_dir, t)) {
				// If we hit a triangle down this branch, we can bail out that we hit a triangle.
				if (ray_octree_intersect(parent->children[i], ray_pos, ray_dir))
					return true;
			}
		}
		// If we get here. then we did not hit any triangles.
		return false;
	}
	else {
		size_t tri_count = parent->indices.size() / 3;
		for (size_t i = 0; i < tri_count; ++i) {
			UINT i0 = parent->indices[i*3+0];
			UINT i1 = parent->indices[i*3+1];
			UINT i2 = parent->indices[i*3+2];
			XMVECTOR v0 = XMLoadFloat3(&m_Vertices[i0]);
			XMVECTOR v1 = XMLoadFloat3(&m_Vertices[i1]);
			XMVECTOR v2 = XMLoadFloat3(&m_Vertices[i2]);
			float t;
			if (TriangleTests::Intersects(ray_pos, ray_dir, v0, v1, v2, t)) return true;
		}
		return false;
	}
}
}
#endif