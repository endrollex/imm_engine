////////////////
// mesh_geometry_gen.h
// library from [Introduction to 3D Game Programming with DirectX 11 by Frank Luna]
////////////////
////////////////
#ifndef MESH_GEOMETRY_GEN_H
#define MESH_GEOMETRY_GEN_H
#include <algorithm>
#include "util_math.h"
////////////////
// geometry
////////////////
////////////////
namespace imm
{
class geometry
{
public:
	struct vertex {
		vertex() {;}
		vertex(const XMFLOAT3 &p, const XMFLOAT3 &n, const XMFLOAT3 &t, const XMFLOAT2 &uv):
			position(p), normal(n), tangent_u(t), tex_c(uv) {;}
		vertex(
			float px, float py, float pz,
			float nx, float ny, float nz,
			float tx, float ty, float tz,
			float u, float v
		):
			position(px, py, pz), normal(nx, ny, nz), tangent_u(tx, ty, tz), tex_c(u, v) {;}
		XMFLOAT3 position;
		XMFLOAT3 normal;
		XMFLOAT3 tangent_u;
		XMFLOAT2 tex_c;
	};
	struct mesh_data {
		std::vector<vertex> vertices;
		std::vector<UINT> indices;
	};
	void create_box(
		float width,
		float height,
		float depth,
		mesh_data &mesh_data_o);
	void create_sphere(
		float radius,
		UINT slice_count,
		UINT stack_count,
		mesh_data &mesh_data_o);
	void create_geosphere(
		float radius,
		UINT num_subdivisions,
		mesh_data &mesh_data_o);
	void create_cylinder(
		float bottom_radius,
		float top_radius,
		float height,
		UINT slice_count,
		UINT stack_count,
		mesh_data &mesh_data_o);
	void create_grid(
		float width,
		float depth,
		UINT m,
		UINT n,
		mesh_data &mesh_data_o);
	void create_fullscreen_quad(mesh_data &mesh_data_o);
private:
	void subdivide(mesh_data &mesh_data_o);
	void build_cylinder_top_cap(
		float top_radius,
		float height,
		UINT slice_count,
		mesh_data &mesh_data_o);
	void build_cylinder_bottom_cap(
		float bottom_radius,
		float height, UINT slice_count,
		mesh_data &mesh_data_o);
};
}
#endif