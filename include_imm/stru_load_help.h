////////////////
// stru_load_help.h
// by Huang Yiting
////////////////
////////////////
#ifndef STRU_LOAD_HELP_H
#define STRU_LOAD_HELP_H
#include "stru_scene_swatch.h"
#include "stru_lua_help.h"
#include "mesh_geometry_gen.h"
#include "mesh_basic_model.h"
#include "phy_prepare.h"
namespace imm
{
////////////////
// data_write_to_bin
////////////////
////////////////
template <typename T_data>
void data_write_to_bin(T_data &data, const std::string &data_file)
{
	std::ofstream outfile (data_file, std::ofstream::binary);
	assert(outfile.is_open());
	outfile.write(reinterpret_cast<char*>(&data), sizeof(data));
	outfile.close();
}
////////////////
// data_load_from_bin
////////////////
////////////////
template <typename T_data>
void data_load_from_bin(T_data &data, const std::string &data_file)
{
	std::ifstream infile(data_file, std::ifstream::binary);
	assert(infile.is_open());
	// get size of file
	infile.seekg(0, infile.end);
	auto size = infile.tellg();
	infile.seekg(0);
	infile.read(reinterpret_cast<char*>(&data), size);
	infile.close();
}
////////////////
// data_vector_write_to_bin
////////////////
////////////////
template <typename T_data>
void data_vector_write_to_bin(std::vector<T_data> &data, const std::string &data_file)
{
	std::ofstream outfile (data_file, std::ofstream::binary);
	assert(outfile.is_open());
	outfile.write(reinterpret_cast<char*>(&data[0]), sizeof(data[0])*data.size());
	outfile.close();
}
////////////////
// data_vector_load_from_bin
////////////////
////////////////
template <typename T_data>
void data_vector_load_from_bin(std::vector<T_data> &data, const std::string &data_file)
{
	std::ifstream infile(data_file, std::ifstream::binary);
	assert(infile.is_open());
	// get size of file
	infile.seekg(0, infile.end);
	auto size = infile.tellg();
	data.resize(size/sizeof(T_data));
	infile.seekg(0);
	infile.read(reinterpret_cast<char*>(&data[0]), size);
	infile.close();
}
////////////////
// data_check_file_exist
////////////////
////////////////
void data_check_file_exist(const std::wstring &path_file)
{
	std::ifstream infile(path_file);
	if (infile.good()) {
		infile.close();
		return;
	}
	infile.close();
	std::string err_str("file not found: ");
	err_str += txtutil::wstr_to_str(path_file);
	ERROR_MESA(err_str.c_str());
	return;
}
////////////////
// data_is_file_exist
////////////////
////////////////
bool data_is_file_exist(const std::string &path_file)
{
	std::ifstream infile(path_file);
	if (infile.good()) {
		infile.close();
		return true;
	}
	infile.close();
	return false;
}
////////////////
// model_load_geo_mesh
////////////////
////////////////
void model_load_geo_mesh(
	ID3D11Device *device,
	simple_model<vertex::pntt> &model_shape,
	const std::vector<geometry::mesh_data> &geo)
{
	size_t size = geo.size();
	if (size == 0) return;
	std::vector<mesh_geometry::subset> &subset_table = model_shape.m_Subsets;
	subset_table.resize(size);
	subset_table[0].id = 0;
	subset_table[0].vertex_start = 0;
	subset_table[0].vertex_count = static_cast<UINT>(geo[0].vertices.size());
	subset_table[0].face_start = 0;
	subset_table[0].face_count = static_cast<UINT>(geo[0].indices.size()/3);
	for (size_t ix = 1; ix < size; ++ix) {
		subset_table[ix].vertex_start = subset_table[ix-1].vertex_start+subset_table[ix-1].vertex_count;
		subset_table[ix].vertex_count = static_cast<UINT>(geo[ix].vertices.size());
		subset_table[ix].face_start = subset_table[ix-1].face_start+subset_table[ix-1].face_count;
		subset_table[ix].face_count = static_cast<UINT>(geo[ix].indices.size()/3);
	}
	//
	std::vector<vertex::pntt> &vertices = model_shape.m_Vertices;
	size_t vertices_size = 0;
	for (size_t ix = 0; ix != size; ++ix) vertices_size += geo[ix].vertices.size();
	vertices.resize(vertices_size);
	size_t k = 0;
	for (size_t ix = 0; ix != size; ++ix) {
		for(size_t i = 0; i < geo[ix].vertices.size(); ++i, ++k) {
			vertices[k].pos       = geo[ix].vertices[i].position;
			vertices[k].normal    = geo[ix].vertices[i].normal;
			vertices[k].tex       = geo[ix].vertices[i].tex_c;
			vertices[k].tangent_u = geo[ix].vertices[i].tangent_u;
		}
	}
	//
	std::vector<UINT> &indices = model_shape.m_Indices;
	for (size_t ix = 0; ix != size; ++ix) {
		indices.insert(indices.end(), geo[ix].indices.begin(), geo[ix].indices.end());
	}
	model_shape.set_MeshBuffer(device);
}
////////////////
// model_load_csv_basic
////////////////
////////////////
template <typename T_model>
void model_load_csv_basic(
	ID3D11Device *device,
	std::map<std::string, T_model> &model,
	std::vector<std::vector<std::string>> csv_model,
	std::map<std::string, rotation_xyz> &rot_front,
	lua_reader &l_reader,
	texture_mgr &tex_mgr,
	const std::string &csv_name,
	const std::string &model_path,
	const std::wstring &texture_path)
{
	// build model
	l_reader.vec2d_str_from_table(csv_name, csv_model);
	for (size_t ix = 1; ix < csv_model.size(); ++ix) {
		assert(csv_model[ix].size() > 5);
		std::string model_name = csv_model[ix][0];
		std::string subpath = csv_model[ix][1];
		std::string model_file = model_path+subpath+csv_model[ix][2];
		rot_front[model_name] = rotation_xyz(csv_model[ix][3]);
		// load model
		if (model.count(model_name)) continue;
		std::wstring texture_path_full = texture_path+txtutil::str_to_wstr(subpath);
		if (model_file.substr(model_file.size()-3) == "m3d") {
			model[model_name].set(device, tex_mgr, model_file, texture_path_full);
		}
		else {
			bin_m3d model_bin;
			model_bin.read_from_bin(model[model_name], model_file);
			model[model_name].set(device, tex_mgr, texture_path_full);
		}
		model[model_name].m_IsAlpha = (stoi(csv_model[ix][4]) != 0);
		model[model_name].m_BoundType = phy_bound_type_str(csv_model[ix][5]);
		model[model_name].m_InteractiveType = phy_interactive_type_str(csv_model[ix][6]);
		model[model_name].m_IsRotXneg90Y180 = rot_front[model_name].is_RotXneg90Y180();
	}
}
////////////////
// instance_assign_csv_basic
////////////////
////////////////
template <typename T_it>
void instance_assign_csv_basic(
	T_it it,
	const size_t &ix,
	const std::string &model_name,
	std::map<std::string, rotation_xyz> &rot_front,
	const std::vector<std::vector<std::string>> &csv_instance)
{
	XMMATRIX R = rot_front[model_name].get_Matrix();
	XMStoreFloat4x4(&(it->rot_front), R);
	float sca_f = stof(csv_instance[ix][2]);
	XMMATRIX scale = XMMatrixScaling(sca_f, sca_f, sca_f);
	XMMATRIX rot = rotation_xyz(csv_instance[ix][3]).get_Matrix();
	XMMATRIX offset = XMMatrixTranslation(
		stof(csv_instance[ix][4]),
		stof(csv_instance[ix][5]),
		stof(csv_instance[ix][6]));
	//
	R = XMMatrixMultiply(R, rot);
	R = XMMatrixMultiply(scale, R);
	R = XMMatrixMultiply(R, offset);
	XMStoreFloat4x4(&(it->world), R);
}
////////////////
// bin_m3d
////////////////
////////////////
struct bin_m3d
{
	bin_m3d();
	template <typename T_model>
	void set_bin_info_basic(const T_model &model);
	void set_bin_info(const basic_model &model);
	void set_bin_info(const skinned_model &model);
	//
	template <typename T_model>
	void write_to_bin_basic(T_model &model, std::ofstream &outfile);
	void write_to_bin(basic_model &model, const std::string &file_name);
	void write_to_bin(skinned_model &model, const std::string &file_name);
	//
	template <typename T_model>
	void read_from_bin_basic(T_model &model, const std::string &file_name, std::ifstream &infile);
	void read_from_bin(basic_model &model, const std::string &file_name);
	void read_from_bin(skinned_model &model, const std::string &file_name);
	//
	std::vector<int> bin_info;
};
//
bin_m3d::bin_m3d():
	bin_info(10, 0)
{
	;
}
//
template <typename T_model>
void bin_m3d::set_bin_info_basic(const T_model &model)
{
	// binary offset
	bin_info[0] = static_cast<int>(sizeof(model.m_Mat[0])*model.m_Mat.size());
	bin_info[1] = static_cast<int>(sizeof(model.m_Vertices[0])*model.m_Vertices.size());
	bin_info[2] = static_cast<int>(sizeof(model.m_Indices[0])*model.m_Indices.size());
	bin_info[3] = static_cast<int>(sizeof(model.m_Subsets[0])*model.m_Subsets.size());
	// vector string
	bin_info[4] = static_cast<int>(model.m_Tex.size());
}
//
void bin_m3d::set_bin_info(const basic_model &model)
{
	set_bin_info_basic(model);
}
//
void bin_m3d::set_bin_info(const skinned_model &model)
{
	set_bin_info_basic(model);
	// skinned_data
	bin_info[5] =
		static_cast<int>(sizeof(model.m_SkinnedData.m_BoneHierarchy[0])
		*model.m_SkinnedData.m_BoneHierarchy.size());
	bin_info[6] =
		static_cast<int>(sizeof(model.m_SkinnedData.m_BoneOffsets[0])
		*model.m_SkinnedData.m_BoneOffsets.size());
	bin_info[7] =
		static_cast<int>(model.m_SkinnedData.m_Animations.size());
}
//
template <typename T_model>
void bin_m3d::write_to_bin_basic(T_model &model, std::ofstream &outfile)
{
	outfile.write(reinterpret_cast<char*>(&bin_info[0]), sizeof(bin_info[0])*bin_info.size());
	outfile.write(reinterpret_cast<char*>(&model.m_Mat[0]), sizeof(model.m_Mat[0])*model.m_Mat.size());
	outfile.write(reinterpret_cast<char*>(&model.m_Vertices[0]), sizeof(model.m_Vertices[0])*model.m_Vertices.size());
	outfile.write(reinterpret_cast<char*>(&model.m_Indices[0]), sizeof(model.m_Indices[0])*model.m_Indices.size());
	outfile.write(reinterpret_cast<char*>(&model.m_Subsets[0]), sizeof(model.m_Subsets[0])*model.m_Subsets.size());
	// write vector string
	for (size_t ix = 0; ix != model.m_Tex.size(); ++ ix) {
		std::string::size_type n = model.m_Tex[ix].size();
		outfile.write(reinterpret_cast<char*>(&n), sizeof(n));
		if (n > 0) outfile.write(model.m_Tex[ix].c_str(), n);
	}
}
//
void bin_m3d::write_to_bin(basic_model &model, const std::string &file_name)
{
	set_bin_info(model);
	std::ofstream outfile (file_name, std::ofstream::binary);
	if (!outfile.is_open()) {
		std::string err_str(".bm3 write fail: ");
		err_str += file_name;
		ERROR_MESA(err_str.c_str());
	}
	write_to_bin_basic(model, outfile);
	outfile.close();
}
//
void bin_m3d::write_to_bin(skinned_model &model, const std::string &file_name)
{
	set_bin_info(model);
	std::ofstream outfile (file_name, std::ofstream::binary);
	if (!outfile.is_open()) {
		std::string err_str(".bm3 write fail: ");
		err_str += file_name;
		ERROR_MESA(err_str.c_str());
	}
	write_to_bin_basic(model, outfile);
	//
	outfile.write(
		reinterpret_cast<char*>(&model.m_SkinnedData.m_BoneHierarchy[0]),
		sizeof(model.m_SkinnedData.m_BoneHierarchy[0])*model.m_SkinnedData.m_BoneHierarchy.size());
	outfile.write(
		reinterpret_cast<char*>(&model.m_SkinnedData.m_BoneOffsets[0]),
		sizeof(model.m_SkinnedData.m_BoneOffsets[0])*model.m_SkinnedData.m_BoneOffsets.size());
	for (auto it = model.m_SkinnedData.m_Animations.begin();
		it != model.m_SkinnedData.m_Animations.end(); ++it) {
		std::string::size_type n;
		n = it->first.size();
		outfile.write(reinterpret_cast<char*>(&n), sizeof(n));
		if (n > 0) outfile.write(it->first.c_str(), n);
		n = it->second.bone_animations.size();
		outfile.write(reinterpret_cast<char*>(&n), sizeof(n));
		for (size_t ix = 0; ix != n; ++ix) {
			std::string::size_type n_key = it->second.bone_animations[ix].keyframes.size();
			outfile.write(reinterpret_cast<char*>(&n_key), sizeof(n_key));
			outfile.write(
				reinterpret_cast<char*>(&it->second.bone_animations[ix].keyframes[0]),
				sizeof(
					it->second.bone_animations[ix].keyframes[0])*
					it->second.bone_animations[ix].keyframes.size());
		}
	}
	outfile.close();
}
//
template <typename T_model>
void bin_m3d::read_from_bin_basic(T_model &model, const std::string &file_name, std::ifstream &infile)
{
	// bin_info size
	int size = static_cast<int>(sizeof(int)*bin_info.size());
	infile.read(reinterpret_cast<char*>(&bin_info[0]), size);
	// check file
	if (bin_info[8] != 0 || bin_info[9] != 0) {
		std::string err_str(".bm3 file load error: ");
		err_str += file_name;
		ERROR_MESA(err_str.c_str());
	}
	size = bin_info[0];
	model.m_Mat.resize(size/sizeof(model.m_Mat[0]));
	infile.read(reinterpret_cast<char*>(&model.m_Mat[0]), size);
	size = bin_info[1];
	model.m_Vertices.resize(size/sizeof(model.m_Vertices[0]));
	infile.read(reinterpret_cast<char*>(&model.m_Vertices[0]), size);
	size = bin_info[2];
	model.m_Indices.resize(size/sizeof(model.m_Indices[0]));
	infile.read(reinterpret_cast<char*>(&model.m_Indices[0]), size);
	size = bin_info[3];
	model.m_Subsets.resize(size/sizeof(model.m_Subsets[0]));
	infile.read(reinterpret_cast<char*>(&model.m_Subsets[0]), size);
	// read vector string
	size = bin_info[4];
	model.m_Tex.resize(size);
	for (int ix = 0; ix != size; ++ix) {
		std::string::size_type n;
		infile.read(reinterpret_cast<char*>(&n), sizeof(n));
		if (n > 0) {
			std::vector<char> v(n);
			infile.read(&v[0], n);
			model.m_Tex[ix].assign(&v[0], &v[0]+n);
		}
	}
}
//
void bin_m3d::read_from_bin(basic_model &model, const std::string &file_name)
{
	std::ifstream infile(file_name, std::ifstream::binary);
	if (!infile.is_open()) {
		std::string err_str(".bm3 file not found: ");
		err_str += file_name;
		ERROR_MESA(err_str.c_str());
	}
	read_from_bin_basic(model, file_name, infile);
	infile.close();
}
//
void bin_m3d::read_from_bin(skinned_model &model, const std::string &file_name)
{
	std::ifstream infile(file_name, std::ifstream::binary);
	if (!infile.is_open()) {
		std::wostringstream os;
		std::string err_str(".bm3 file not found: ");
		err_str += file_name;
		ERROR_MESA(err_str.c_str());
	}
	read_from_bin_basic(model, file_name, infile);
	int size = bin_info[5];
	model.m_SkinnedData.m_BoneHierarchy.resize(
		size/sizeof(model.m_SkinnedData.m_BoneHierarchy[0]));
	infile.read(reinterpret_cast<char*>(&model.m_SkinnedData.m_BoneHierarchy[0]), size);
	size = bin_info[6];
	model.m_SkinnedData.m_BoneOffsets.resize(
		size/sizeof(model.m_SkinnedData.m_BoneOffsets[0]));
	infile.read(reinterpret_cast<char*>(&model.m_SkinnedData.m_BoneOffsets[0]), size);
	//
	size = bin_info[7];
	model.m_SkinnedData.m_Animations;
	for (int ix = 0; ix != size; ++ix) {
		std::string::size_type n;
		std::string clip_name;
		infile.read(reinterpret_cast<char*>(&n), sizeof(n));
		if (n > 0) {
			std::vector<char> v(n);
			infile.read(&v[0], n);
			clip_name.assign(&v[0], &v[0]+n);
		}
		infile.read(reinterpret_cast<char*>(&n), sizeof(n));
		animation_clip clip;
		clip.bone_animations.resize(n);
		for (size_t ix2 = 0; ix2 != n; ++ix2) {
			std::string::size_type n_key;
			infile.read(reinterpret_cast<char*>(&n_key), sizeof(n_key));
			clip.bone_animations[ix2].keyframes.resize(n_key);
			infile.read(reinterpret_cast<char*>(
				&clip.bone_animations[ix2].keyframes[0]),
				sizeof(clip.bone_animations[ix2].keyframes[0])*n_key);
		}
		model.m_SkinnedData.m_Animations.insert(
				std::pair<std::string, animation_clip>(clip_name, clip));
	}
	infile.close();
}
}
#endif