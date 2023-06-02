////////////////
// stru_wave.h
// by Huang Yiting
////////////////
////////////////
#ifndef STRU_WAVE_H
#define STRU_WAVE_H
#include "render_wave.h"
#include "mesh_geometry_gen.h"
#include "pipe_render.h"
#include "pipe_vertex.h"
#include "util_camera.h"
#include "phy_prepare.h"
#include "stru_lua_help.h"
namespace imm
{
////////////////
// state_liquid
////////////////
////////////////
struct state_liquid
{
	state_liquid();
	~state_liquid();
	state_liquid(const state_liquid &rhs);
	void init(
		ID3D11Device* device,
		const XMFLOAT4X4 &world_in,
		const UINT &num_rows,
		const UINT &num_cols,
		const float &spatial_step,
		const float &time_step,
		const float &speed,
		const float &damping);
	void update(ID3D11DeviceContext *context, float dt, float total_time);
	template <typename T_bound>
	void intersects(const T_bound &box_in, const float &dt, const size_t &index);
	void frustum_culling(const BoundingFrustum &frustum);
	void draw(ID3D11DeviceContext *context, const camera &cam1);
	gpu_wave wave;
	material wave_mat;
	ID3D11Buffer *wave_vb;
	ID3D11Buffer *wave_ib;
	ID3D11ShaderResourceView *wave_map_srv;
	XMFLOAT4X4 wave_world;
	XMFLOAT4X4 wave_tex_transform;
	XMFLOAT2 wave_tex_offset;
	UINT wave_index_count;
	bool is_disappear;
	float time_intersects;
	float offset_x;
	float offset_y;
	BoundingBox bbox_w;
	std::vector<POINT> collision;
	std::map<size_t, POINT> last_pos;
	// wave parameter
	UINT pa_num_rows;
	UINT pa_num_cols;
	float pa_spatial_step;
	float pa_time_step;
	float pa_speed;
	float pa_damping;
private:
	state_liquid &operator=(const state_liquid &rhs);
};
//
state_liquid::state_liquid():
	wave(),
	wave_vb(nullptr),
	wave_ib(nullptr),
	wave_map_srv(nullptr),
	wave_tex_offset(0.0f, 0.0f),
	wave_index_count(0),
	is_disappear(false),
	time_intersects(0.0f),
	offset_x(0.0f),
	offset_y(0.0f),
	bbox_w(),
	pa_num_rows(512),
	pa_num_cols(512),
	pa_spatial_step(0.5f),
	pa_time_step(0.03f),
	pa_speed(4.0f),
	pa_damping(0.2f)
{
	XMStoreFloat4x4(&wave_world, XMMatrixIdentity());
	XMStoreFloat4x4(&wave_tex_transform, XMMatrixIdentity());
	wave_mat.ambient  = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	wave_mat.diffuse  = XMFLOAT4(0.27f, 0.55f, 0.79f, 0.5f);
	wave_mat.specular = XMFLOAT4(0.4f, 0.4f, 0.4f, 16.0f);
}
//
state_liquid::~state_liquid()
{
	RELEASE_COM(wave_vb);
	RELEASE_COM(wave_ib);
	RELEASE_COM(wave_map_srv);
}
//
state_liquid::state_liquid(const state_liquid &rhs)
{
	// Ensure do not use copy constructor
	DUMMY(rhs);
	assert(false);
}
//
void state_liquid::init(
	ID3D11Device* device,
	const XMFLOAT4X4 &world_in,
	const UINT &num_rows,
	const UINT &num_cols,
	const float &spatial_step,
	const float &time_step,
	const float &speed,
	const float &damping)
{
	wave_world = world_in;
	pa_num_rows = num_rows;
	pa_num_cols = num_cols;
	pa_spatial_step = spatial_step;
	pa_time_step = time_step;
	pa_speed = speed;
	pa_damping = damping;
	wave.init(device, pa_num_rows, pa_num_cols, pa_spatial_step, pa_time_step, pa_speed, pa_damping);
	//
	geometry::mesh_data grid;
	geometry geo_gen;
	geo_gen.create_grid(wave.width(), wave.depth(), wave.row_count(), wave.column_count(), grid);
	wave_index_count = static_cast<UINT>(grid.indices.size());
	// Extract the vertex elements we are interested.
	std::vector<vertex::basic32> vertices(grid.vertices.size());
	for(UINT i = 0; i < grid.vertices.size(); ++i) {
		vertices[i].pos    = grid.vertices[i].position;
		vertices[i].normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
		vertices[i].tex    = grid.vertices[i].tex_c;
	}
	D3D11_BUFFER_DESC vbd;
	vbd.Usage          = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth      = static_cast<UINT>(sizeof(vertex::basic32) * grid.vertices.size());
	vbd.BindFlags      = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags      = 0;
	D3D11_SUBRESOURCE_DATA vinit_data;
	vinit_data.pSysMem = &vertices[0];
	HR(device->CreateBuffer(&vbd, &vinit_data, &wave_vb));
	D3D11_BUFFER_DESC ibd;
	ibd.Usage          = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth      = sizeof(UINT) * wave_index_count;
	ibd.BindFlags      = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags      = 0;
	D3D11_SUBRESOURCE_DATA iinit_data;
	iinit_data.pSysMem = &grid.indices[0];
	HR(device->CreateBuffer(&ibd, &iinit_data, &wave_ib));
	//
	BoundingBox bbox_l;
	phy_set_box(bbox_l, grid.vertices, [](const geometry::vertex &x) {return &x.position;});
	XMMATRIX world = XMLoadFloat4x4(&wave_world);
	bbox_l.Transform(bbox_w, world);
	offset_x = pa_num_rows*pa_spatial_step*0.5f;
	offset_y = pa_num_cols*pa_spatial_step*0.5f;
}
//
void state_liquid::update(ID3D11DeviceContext *context, float dt, float total_time)
{
	if (is_disappear) return;
	// Generate a collision wave.
	if (collision.size() > 0) {
		float r = math::calc_randf(0.7f, 1.0f);
		wave.disturb(context, collision[0].y, collision[0].x, r);
		collision.clear();
	}
	wave.update(context, dt, total_time);
}
//
template <typename T_bound>
void state_liquid::intersects(const T_bound &box_in, const float &dt, const size_t &index)
{
	if (is_disappear) return;
	time_intersects += dt;
	if (time_intersects < pa_time_step*10.0f) return;
	if (bbox_w.Intersects(box_in)) {
		float x = (box_in.Center.x+offset_x)*1.0f/pa_spatial_step;
		float y = (-box_in.Center.z+offset_y)*1.0f/pa_spatial_step;
		POINT pos;
		pos.x = static_cast<LONG>(x);
		pos.y = static_cast<LONG>(y);
		if (last_pos[index].x != pos.x || last_pos[index].y != pos.y) {
			collision.emplace_back(pos);
			last_pos[index] = pos;
		}
	}
	else {
		last_pos[index].x = pa_num_rows;
		last_pos[index].y = pa_num_cols;
	}
	time_intersects = 0;
}
//
void state_liquid::frustum_culling(const BoundingFrustum &frustum)
{
	if (frustum.Intersects(bbox_w)) is_disappear = false;
	else is_disappear = true;
}
//
void state_liquid::draw(ID3D11DeviceContext *context, const camera &cam1)
{
	if (is_disappear) return;
	UINT stride = sizeof(vertex::basic32);
	UINT offset = 0;
	ID3DX11EffectTechnique* wave_tech = effects::m_WaveRenderFX->m_Light3Tech;
	//ID3DX11EffectTechnique* wave_tech = effects::m_WaveRenderFX->m_Light3TexTech;
	D3DX11_TECHNIQUE_DESC tech_desc;
	XMMATRIX view_proj = cam1.get_ViewProj();
	XMMATRIX world = XMLoadFloat4x4(&wave_world);
	XMMATRIX world_inv_trans = math::inverse_transpose(world);
	XMMATRIX world_view_proj = world*view_proj;
	context->IASetVertexBuffers(0, 1, &wave_vb, &stride, &offset);
	context->IASetIndexBuffer(wave_ib, DXGI_FORMAT_R32_UINT, 0);
	// Set per object constants.
	effects::m_WaveRenderFX->set_World(world);
	effects::m_WaveRenderFX->set_WorldInvTranspose(world_inv_trans);
	effects::m_WaveRenderFX->set_WorldViewProj(world_view_proj);
	effects::m_WaveRenderFX->set_TexTransform(XMLoadFloat4x4(&wave_tex_transform));
	effects::m_WaveRenderFX->set_Material(wave_mat);
	effects::m_WaveRenderFX->set_DiffuseMap(wave_map_srv);
	effects::m_WaveRenderFX->set_DisplacementMap(wave.get_displacement_map());
	effects::m_WaveRenderFX->set_GridSpatialStep(0.5f);
	effects::m_WaveRenderFX->set_DisplacementTexelSize(XMFLOAT2(1.0f/wave.column_count(), 1.0f/wave.row_count()));
	wave_tech->GetDesc(&tech_desc);
	for(UINT p = 0; p < tech_desc.Passes; ++p) {
		context->OMSetBlendState(render::m_TransparentBS, FLOAT_4_ZERO, 0xffffffff);
		wave_tech->GetPassByIndex(p)->Apply(0, context);
		context->DrawIndexed(wave_index_count, 0, 0);
		// Restore default blend state
		context->OMSetBlendState(0, FLOAT_4_ZERO, 0xffffffff);
	}
	// Unbind displacement map from vertex shader, as we will be binding it as a compute
	// shader resource in the next update.
	ID3D11ShaderResourceView* null_srv[1] = {0};
	context->VSSetShaderResources(0, 1, null_srv);
}
//
////////////////
// envi_liquid
////////////////
////////////////
struct envi_liquid
{
	envi_liquid();
	~envi_liquid();
	void reload(ID3D11Device* device, const std::string &scene);
	void update(ID3D11DeviceContext *context, float dt, float total_time);
	template <typename T_bound>
	void intersects(const T_bound &box_in, const float &dt, const size_t &index);
	void frustum_culling(const BoundingFrustum &frustum);
	void draw(ID3D11DeviceContext *context, light_dir lights[3], const camera &cam1);
	std::vector<state_liquid> liquid;
};
//
envi_liquid::envi_liquid()
{
	;
}
//
envi_liquid::~envi_liquid()
{
	;
}
//
void envi_liquid::reload(ID3D11Device* device, const std::string &scene_ix)
{
	liquid.clear();
	std::string concrete = IMM_PATH["script"]+"scene"+scene_ix+"\\concrete_instance.lua";
	lua_reader l_reader;
	l_reader.loadfile(concrete);
	std::string table_name = "csv_envi_liquid";
	if (l_reader.is_nil(table_name)) return;
	std::vector<std::vector<std::string>> get_init;
	l_reader.vec2d_str_from_table(table_name, get_init);
	// resize ensure element of vector not move
	if (get_init.size() > 0) liquid.resize(get_init.size()-1);
	for (size_t ix = 1; ix < get_init.size(); ++ix) {
		XMMATRIX offset = XMMatrixTranslation(
			stof(get_init[ix][0]),
			stof(get_init[ix][1]),
			stof(get_init[ix][2]));
		XMFLOAT4X4 world;
		XMStoreFloat4x4(&world, offset);
		liquid[ix-1].init(
			device,
			world,
			stoi(get_init[ix][3]),
			stoi(get_init[ix][4]),
			stof(get_init[ix][5]),
			stof(get_init[ix][6]),
			stof(get_init[ix][7]),
			stof(get_init[ix][8]));
	}
}
//
void envi_liquid::update(ID3D11DeviceContext *context, float dt, float total_time)
{
	for (auto &liq: liquid) liq.update(context, dt, total_time);
}
//
template <typename T_bound>
void envi_liquid::intersects(const T_bound &box_in, const float &dt, const size_t &index)
{
	for (auto &liq: liquid) liq.intersects(box_in, dt, index);
}
//
void envi_liquid::frustum_culling(const BoundingFrustum &frustum)
{
	for (auto &liq: liquid) liq.frustum_culling(frustum);
}
//
void envi_liquid::draw(ID3D11DeviceContext *context, light_dir lights[3], const camera &cam1)
{
	context->IASetInputLayout(input_layouts::m_Basic32);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	effects::m_WaveRenderFX->set_DirLights(lights);
	effects::m_WaveRenderFX->set_EyePosW(cam1.get_Position());	
	for (auto &liq: liquid) liq.draw(context, cam1);
}
//
}
#endif