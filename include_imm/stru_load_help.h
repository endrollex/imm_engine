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
		std::wstring tex_location = texture_path+txtutil::str_to_wstr(subpath);
		if (model_file.substr(model_file.size()-3) == "m3d") {
			model[model_name].set(device, tex_mgr, model_file, tex_location);
		}
		else {
			bin_m3d model_bin;
			model_bin.read_from_bin(model[model_name], model_file, tex_mgr);
			model[model_name].set(device, tex_mgr, tex_location);
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
	bool set_bin_info_basic(const T_model &model);
	bool set_bin_info(const basic_model &model);
	bool set_bin_info(const skinned_model &model);
	//
	template <typename T_model>
	void write_to_bin_basic(T_model &model, std::ofstream &outfile);
	template <typename T_model>
	void write_to_bin_tex(T_model &model, std::ofstream &outfile);
	bool write_to_bin(basic_model &model, const std::string &file_name);
	bool write_to_bin(skinned_model &model, const std::string &file_name);
	//
	template <typename T_model>
	void read_from_bin_basic(T_model &model, const std::string &file_name, std::ifstream &infile);
	template <typename T_model>
	void read_from_bin_tex(T_model &model, texture_mgr &tex_mgr, std::ifstream &infile);
	void read_from_bin(basic_model &model, const std::string &file_name, texture_mgr &tex_mgr);
	void read_from_bin(skinned_model &model, const std::string &file_name, texture_mgr &tex_mgr);
	bool is_skinned(const std::string &file_name);
	//
	std::vector<size_t> bin_info;
	size_t tex_begin;
	size_t key1_pos;
	size_t key1;
};
//
bin_m3d::bin_m3d():
	bin_info(256, 0),
	tex_begin(32),
	key1_pos(8),
	key1(450543252)
{
	;
}
//
template <typename T_model>
bool bin_m3d::set_bin_info_basic(const T_model &model)
{
	bool is_tex_ok = true;
	// binary offset
	bin_info[0] = sizeof(model.m_Mat[0])*model.m_Mat.size();
	bin_info[1] = sizeof(model.m_Vertices[0])*model.m_Vertices.size();
	bin_info[2] = sizeof(model.m_Indices[0])*model.m_Indices.size();
	bin_info[3] = sizeof(model.m_Subsets[0])*model.m_Subsets.size();
	// vector string
	bin_info[4] = model.m_Tex.size();
	bin_info[key1_pos] = key1;
	// tex
	std::ifstream infile;
	size_t tex_pos = tex_begin;
	size_t len = model.m_Tex.size();
	for (size_t ix = 0; ix != len; ++ix) {
		std::string tex_path = IMM_PATH["texture"]+model.m_Tex[ix];
		infile.open(tex_path, std::ifstream::binary);
		is_tex_ok = infile.is_open();
		if (!is_tex_ok) return false;
		size_t length = 0;
		infile.seekg(0, infile.end);
		length = infile.tellg();
		infile.close();
		bin_info[tex_pos] = length;
		tex_pos++;
	}
	return is_tex_ok;
}
//
bool bin_m3d::set_bin_info(const basic_model &model)
{
	bool is_tex_ok = set_bin_info_basic(model);
	return is_tex_ok;
}
//
bool bin_m3d::set_bin_info(const skinned_model &model)
{
	bool is_tex_ok = set_bin_info_basic(model);
	if (!is_tex_ok) return false;
	// skinned_data
	bin_info[5] =
		sizeof(model.m_SkinnedData.m_BoneHierarchy[0])*model.m_SkinnedData.m_BoneHierarchy.size();
	bin_info[6] =
		sizeof(model.m_SkinnedData.m_BoneOffsets[0])*model.m_SkinnedData.m_BoneOffsets.size();
	bin_info[7] =
		model.m_SkinnedData.m_Animations.size();
	return true;
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
template <typename T_model>
void bin_m3d::write_to_bin_tex(T_model &model, std::ofstream &outfile)
{
	std::ifstream infile;
	size_t tex_pos = tex_begin;
	size_t len = model.m_Tex.size();
	for (size_t ix = 0; ix != len; ++ix) {
		std::string tex_path = IMM_PATH["texture"]+model.m_Tex[ix];
		infile.open(tex_path, std::ifstream::binary);
		assert(infile.is_open());
		size_t length = bin_info[tex_pos];
		uint8_t *buffer = new uint8_t[length];
		infile.read((char*)buffer, length);
		outfile.write((char*)buffer, length);
		infile.close();
		delete[] buffer;
		tex_pos++;
	}
}
//
bool bin_m3d::write_to_bin(basic_model &model, const std::string &file_name)
{
	bool is_tex_ok = set_bin_info(model);
	if (!is_tex_ok) return false;
	std::ofstream outfile (file_name, std::ofstream::binary);
	if (!outfile.is_open()) {
		std::string err_str(".bm3 write fail: ");
		err_str += file_name;
		ERROR_MESA(err_str.c_str());
	}
	write_to_bin_basic(model, outfile);
	write_to_bin_tex(model, outfile);
	outfile.close();
	return is_tex_ok;
}
//
bool bin_m3d::write_to_bin(skinned_model &model, const std::string &file_name)
{
	bool is_tex_ok = set_bin_info(model);
	if (!is_tex_ok) return false;
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
	write_to_bin_tex(model, outfile);
	outfile.close();
	return is_tex_ok;
}
//
template <typename T_model>
void bin_m3d::read_from_bin_basic(T_model &model, const std::string &file_name, std::ifstream &infile)
{
	// bin_info size
	size_t size = sizeof(size_t)*bin_info.size();
	infile.read(reinterpret_cast<char*>(&bin_info[0]), size);
	// check file
	if (bin_info[key1_pos] != key1) {
		std::string err_str(".b3m file load error: ");
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
template <typename T_model>
void bin_m3d::read_from_bin_tex(T_model &model, texture_mgr &tex_mgr, std::ifstream &infile)
{
	size_t ix = tex_begin;
	size_t size_info = bin_info.size();
	while (bin_info[ix] != 0 && ix < size_info) {
		size_t length = bin_info[ix];
		uint8_t *buffer = new uint8_t[length];
		infile.read((char*)buffer, length);
		std::wstring tex_name(txtutil::str_to_wstr(model.m_Tex[ix-tex_begin]));
		auto ptr = tex_mgr.create_texture(tex_name, buffer, length);
		assert(ptr != nullptr);
		delete[] buffer;
		ix++;
	}
	size_t siez_tex = model.m_Tex.size();
	size_t size_info_tex = ix-tex_begin;
	assert(siez_tex == size_info_tex);
}
//
void bin_m3d::read_from_bin(basic_model &model, const std::string &file_name, texture_mgr &tex_mgr)
{
	std::ifstream infile(file_name, std::ifstream::binary);
	if (!infile.is_open()) {
		std::string err_str(".bm3 file not found: ");
		err_str += file_name;
		ERROR_MESA(err_str.c_str());
	}
	read_from_bin_basic(model, file_name, infile);
	read_from_bin_tex(model, tex_mgr, infile);
	infile.close();
}
//
void bin_m3d::read_from_bin(skinned_model &model, const std::string &file_name, texture_mgr &tex_mgr)
{
	std::ifstream infile(file_name, std::ifstream::binary);
	if (!infile.is_open()) {
		std::wostringstream os;
		std::string err_str(".bm3 file not found: ");
		err_str += file_name;
		ERROR_MESA(err_str.c_str());
	}
	read_from_bin_basic(model, file_name, infile);
	size_t size = bin_info[5];
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
	read_from_bin_tex(model, tex_mgr, infile);
	infile.close();
}
//
bool bin_m3d::is_skinned(const std::string &file_name)
{
	std::ifstream infile(file_name, std::ifstream::binary);
	if (!infile.is_open()) {
		std::string err_str(".bm3 file not found: ");
		err_str += file_name;
		ERROR_MESA(err_str.c_str());
	}
	//
	size_t size = sizeof(size_t)*bin_info.size();
	infile.read(reinterpret_cast<char*>(&bin_info[0]), size);
	// check file
	if (bin_info[key1_pos] != key1) {
		std::string err_str(".bm3 file load error: ");
		err_str += file_name;
		ERROR_MESA(err_str.c_str());
	}
	infile.close();
	if (bin_info[5] != 0) return true;
	return false;
}
}
#endif