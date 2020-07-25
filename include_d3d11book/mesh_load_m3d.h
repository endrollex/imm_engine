////////////////
// mesh_load_m3d.h
// library from [Introduction to 3D Game Programming with DirectX 11 by Frank Luna]
////////////////
////////////////
#ifndef MESH_LOAD_M3D_H
#define MESH_LOAD_M3D_H
#include "mesh_skinned_data.h"
#include "mesh_geometry.h"
#include "pipe_vertex.h"
#include "DDSTextureLoader.h"
namespace imm
{
////////////////
// m3d_material
////////////////
////////////////
struct m3d_material
{
	material mat;
	bool alpha_clip;
	std::string effect_type_name;
	std::wstring diffuse_map_name;
	std::wstring normal_map_name;
};
////////////////
// m3d_loader
////////////////
////////////////
class m3d_loader
{
public:
	bool load_m3d(
		const std::string &filename,
		std::vector<vertex::pntt2> &vertices,
		std::vector<UINT> &indices,
		std::vector<mesh_geometry::subset> &subsets,
		std::vector<m3d_material> &mats);
	bool load_m3d(
		const std::string &filename,
		std::vector<vertex::pntt_skinned> &vertices,
		std::vector<UINT> &indices,
		std::vector<mesh_geometry::subset> &subsets,
		std::vector<m3d_material> &mats,
		skinned_data &skin_info);
private:
	void read_Materials(std::ifstream &fin, UINT num_materials, std::vector<m3d_material> &mats);
	void read_SubsetTable(std::ifstream &fin, UINT num_subsets, std::vector<mesh_geometry::subset> &subsets);
	void read_Vertices(std::ifstream &fin, UINT num_vertices, std::vector<vertex::pntt2> &vertices);
	void read_Triangles(std::ifstream &fin, UINT num_triangles, std::vector<UINT> &indices);
	//
	void read_SkinnedVertices(std::ifstream &fin, UINT num_vertices, std::vector<vertex::pntt_skinned> &vertices);
	void read_BoneOffsets(std::ifstream &fin, UINT num_bones, std::vector<XMFLOAT4X4> &bone_offsets);
	void read_BoneHierarchy(std::ifstream &fin, UINT num_bones, std::vector<int> &bone_index_to_parent_index);
	void read_AnimationClips(std::ifstream &fin, UINT num_bones, UINT num_animation_clips, std::map<std::string, animation_clip> &animations);
	void read_BoneKeyframes(std::ifstream &fin, bone_animation &bone_anim);
};
////////////////
// Simple texture manager to avoid loading duplicate textures from file.  That can
// happen, for example, if multiple meshes reference the same texture filename.
////////////////
////////////////
class texture_mgr
{
public:
	texture_mgr();
	~texture_mgr();
	void init(ID3D11Device *device);
	ID3D11ShaderResourceView *create_texture(std::wstring tex_path, std::wstring filename);
	ID3D11ShaderResourceView *create_texture(std::wstring filename, uint8_t *buffer, const size_t &length);
private:
	texture_mgr(const texture_mgr &rhs);
	texture_mgr &operator=(const texture_mgr &rhs);
	ID3D11Device *m_D3DDevice;
	std::map<std::wstring, ID3D11ShaderResourceView*> m_TextureSRV;
};
//
}
#endif