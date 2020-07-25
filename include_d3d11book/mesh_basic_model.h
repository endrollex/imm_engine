////////////////
// mesh_basic_model.h
// library from [Introduction to 3D Game Programming with DirectX 11 by Frank Luna]
////////////////
////////////////
#ifndef MESH_BASIC_MODEL_H
#define MESH_BASIC_MODEL_H
#include "mesh_load_m3d.h"
#include "mesh_skinned_data.h"
namespace imm
{
////////////////
// basic_model
////////////////
////////////////
class basic_model
{
public:
	basic_model();
	// base class must have virtual destructor
	// tex_mgr will kill ID3D11ShaderResourceView*
	virtual ~basic_model() {;}
	void set(
		ID3D11Device *device,
		texture_mgr &tex_mgr,
		const std::string &model_filename,
		const std::wstring &texture_path);
	void set(
		ID3D11Device *device,
		texture_mgr &tex_mgr,
		const std::wstring &texture_path);
	std::pair<size_t, size_t> get_VertexRange(UINT sub_id);
	size_t m_SubsetCount;
	std::vector<material> m_Mat;
	std::vector<std::string> m_Tex;
	std::vector<ID3D11ShaderResourceView*> m_DiffuseMapSRV;
	std::vector<ID3D11ShaderResourceView*> m_NormalMapSRV;
	// Keep CPU copies of the mesh data to read from.
	std::vector<vertex::pntt2> m_Vertices;
	std::vector<UINT> m_Indices;
	std::vector<mesh_geometry::subset> m_Subsets;
	mesh_geometry m_ModelMesh;
	int m_BoundType;
	int m_InteractiveType;
	bool m_IsAlpha;
	bool m_IsRotXneg90Y180;
};
////////////////
// basic_model_instance
////////////////
////////////////
struct basic_model_instance
{
	basic_model_instance();
	bool is_appear();
	basic_model *model;
	XMFLOAT4X4 world;
	XMFLOAT4X4 rot_front;
	std::string model_name;
	bool is_in_frustum;
	bool is_offline;
	bool is_transparent;
	size_t stat_ix;
};
////////////////
// skinned_model
////////////////
////////////////
class skinned_model: public basic_model
{
public:
	skinned_model() {;}
	~skinned_model() {;}
	void set(
		ID3D11Device *device,
		texture_mgr &tex_mgr,
		const std::string &model_filename,
		const std::wstring &texture_path);
	void set(
		ID3D11Device *device,
		texture_mgr &tex_mgr,
		const std::wstring &texture_path);
	std::vector<vertex::pntt_skinned> m_Vertices;
	skinned_data m_SkinnedData;
};
////////////////
// skinned_model_instance
////////////////
////////////////
struct skinned_model_instance
{
	skinned_model_instance();
	bool is_appear();
	skinned_model *model;
	XMFLOAT4X4 world;
	XMFLOAT4X4 rot_front;
	std::string model_name;
	std::string clip_name;
	std::string switch_name;
	std::vector<XMFLOAT4X4> final_transforms;
	float time_pos;
	float time_switch;
	float time_scale;
	bool is_in_frustum;
	bool is_offline;
	bool is_transparent;
	size_t stat_ix;
	bool is_switching;
	void update(float dt);
	void set_ClipName(
		const std::string &clip_name,
		const bool &is_reset_time = false,
		const float &time_scale_in = 1.0f);
	void check_set_ClipName(
		const std::string &clip_name,
		const bool &is_reset_time = false,
		const float &time_scale_in = 1.0f);
	void set_switch_ClipName(
		const std::string &clip_first,
		const std::string &clip_second,
		const size_t &last_frame, const float &time_scale_in = 1.0f);
	void set_sequence_ClipName(const std::string &clip_second, const float &time_scale_in = 1.0f);
};
////////////////
// simple_model
////////////////
////////////////
template <typename vertex_type>
class simple_model: public basic_model
{
	public:
	simple_model() {;}
	~simple_model() {;}
	std::vector<vertex_type> m_Vertices;
	std::map<std::string, UINT> m_NameSubid;
	std::map<std::string, int> m_NameBoundType;
	std::map<std::string, int> m_NameInteractiveType;
	void set_MeshBuffer(ID3D11Device *device);
	void set_SingleSubset();
	void set_MapSRV(
		texture_mgr &tex_mgr,
		std::wstring tex_path,
		std::wstring diffuse_file,
		std::wstring normal_file,
		bool is_texture = false);
};
////////////////
// simple_model_instance
////////////////
////////////////
template <typename vertex_type>
struct simple_model_instance
{
	simple_model_instance();
	bool is_appear();
	simple_model<vertex_type> *model;
	XMFLOAT4X4 world;
	XMFLOAT4X4 rot_front;
	XMFLOAT4X4 tex_transform;
	std::string model_name;
	UINT subid;
	bool is_textrue;
	bool is_in_frustum;
	bool is_offline;
	bool is_transparent;
	size_t stat_ix;
};
//
template <typename vertex_type>
simple_model_instance<vertex_type>::simple_model_instance():
	model(nullptr),
	model_name(),
	subid(0),
	is_textrue(true),
	is_in_frustum(true),
	is_offline(false),
	is_transparent(false),
	stat_ix(SIZE_MAX)
{
	XMStoreFloat4x4(&world, XMMatrixIdentity());
	XMStoreFloat4x4(&rot_front, XMMatrixIdentity());
	XMStoreFloat4x4(&tex_transform, XMMatrixIdentity());
}
//
template <typename vertex_type>
bool simple_model_instance<vertex_type>::is_appear()
{
	if (!is_in_frustum) return false;
	if (is_offline) return false;
	if (is_transparent) return false;
	return true;	
}
//
template <typename vertex_type>
void simple_model<vertex_type>::set_MeshBuffer(ID3D11Device *device)
{
	m_ModelMesh.set_Vertices(device, &m_Vertices[0], static_cast<UINT>(m_Vertices.size()));
	m_ModelMesh.set_Indices(device, &m_Indices[0], static_cast<UINT>(m_Indices.size()));
	//
	if (m_Subsets.size() == 0) this->set_SingleSubset();
	m_SubsetCount = m_Subsets.size();
	m_ModelMesh.set_SubsetTable(m_Subsets);
}
//
template <typename vertex_type>
void simple_model<vertex_type>::set_SingleSubset()
{
	m_Subsets.clear();
	m_Subsets.push_back(mesh_geometry::subset());
	m_Subsets[0].id = 0;
	m_Subsets[0].vertex_start = 0;
	m_Subsets[0].vertex_count = static_cast<UINT>(m_Vertices.size());
	m_Subsets[0].face_start = 0;
	m_Subsets[0].face_count = static_cast<UINT>(m_Indices.size()/3);
}
//
template <typename vertex_type>
void simple_model<vertex_type>::set_MapSRV(
	texture_mgr &tex_mgr,
	std::wstring tex_path,
	std::wstring diffuse_file,
	std::wstring normal_file,
	bool is_texture)
{
	ID3D11ShaderResourceView *diffuse_map_srv = nullptr;
	ID3D11ShaderResourceView *normal_map_srv = nullptr;
	if (is_texture) {
		diffuse_map_srv = tex_mgr.create_texture(tex_path, diffuse_file);
		normal_map_srv = tex_mgr.create_texture(tex_path, normal_file);
	}
	m_DiffuseMapSRV.push_back(diffuse_map_srv);
	m_NormalMapSRV.push_back(normal_map_srv);
}
//
}
#endif