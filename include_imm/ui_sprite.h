////////////////
// ui_sprite.h
// by Huang Yiting
////////////////
////////////////
#ifndef UI_SPRITE_H
#define UI_SPRITE_H
#include "SpriteBatch.h"
#include "SpriteFont.h"
#include "CommonStates.h"
#include "D2d1.h"
namespace imm
{
////////////////
// ui_rect
////////////////
////////////////
struct ui_rect
{
	enum type {
		background  = 0,
		button      = 1, // be careful choose ui_rect's type, if a fake button appear, pad may be stuck
		text_pure   = 2,
		text_layout = 3,
		sprite      = 4,
	};
	ui_rect();
	type tp;
	std::string brush_ix;
	std::vector<std::string> brush_sel; // button has two brush, for represents selected or not
	D2D1_RECT_F rc;
	std::wstring text;
	std::string id_str;
	std::string parent_str;
	std::string group;
	std::string dwrite_ix;
	XMFLOAT4 margin;
	int parent;
	bool active;
};
//
ui_rect::ui_rect():
	tp(type::background),
	parent(-1),
	active(false)
{
	;
}
////////////////
// sprite_simple
////////////////
////////////////
struct sprite_simple
{
	sprite_simple();
	~sprite_simple();
	void init(ID3D11Device *device, ID3D11DeviceContext *device_context);
	void build_buffer(const std::vector<std::vector<std::string>> &get_dds);
	bool buffer_check(const std::string &name);
	void draw_d3d(
		const std::map<std::string, size_t> &map_sprite_rect,
		const std::vector<ui_rect> &rect,
		const std::map<std::string, std::string> &map_sprite_name);
	void on_resize(const float &scale_in);
	std::unique_ptr<SpriteBatch> sprite_batch;
	std::unique_ptr<SpriteFont> sprite_font;
	std::unique_ptr<CommonStates> states;
	texture_mgr tex_mgr;
	std::map<std::string, std::wstring> map_name;
	std::map<std::string, XMFLOAT2> map_pos;
	std::map<std::string, float> map_height;
	float scale;
	bool is_built;
private:
	sprite_simple(const sprite_simple &rhs);
	sprite_simple &operator=(const sprite_simple &rhs);
};
//
sprite_simple::sprite_simple():
	sprite_batch(nullptr),
	sprite_font(nullptr),
	states(nullptr),
	tex_mgr(),
	scale(1.0f),
	is_built(false)
{
	;
}
//
sprite_simple::~sprite_simple()
{
	;
}
//
void sprite_simple::init(ID3D11Device *device, ID3D11DeviceContext *device_context)
{
	sprite_batch = std::unique_ptr<SpriteBatch>(new SpriteBatch(device_context));
	states = std::unique_ptr<CommonStates>(new CommonStates(device));
	tex_mgr.init(device);
}
void sprite_simple::build_buffer(const std::vector<std::vector<std::string>> &get_dds)
{
	D3D11_TEXTURE2D_DESC desc;
	std::wstring tex_path = txtutil::str_to_wstr(IMM_PATH["texture"]+"avatar\\");
	for (size_t ix = 0; ix != get_dds.size(); ++ix) {
		ID3D11Resource *res = nullptr;
		ID3D11Texture2D *texture2d = nullptr;
		std::wstring dds_name = txtutil::str_to_wstr(get_dds[ix][1]);
		map_name[get_dds[ix][0]] = dds_name;
		tex_mgr.create_texture(tex_path, dds_name);
		// dds: 1024*1024, 1024*2048, 2048*2048
		tex_mgr.m_TextureSRV[dds_name]->GetResource(&res);
		HR(res->QueryInterface(&texture2d));
		texture2d->GetDesc(&desc);
		map_height[get_dds[ix][0]] = static_cast<float>(desc.Height);
		RELEASE_COM(res);
		RELEASE_COM(texture2d);
	}
	if (tex_mgr.m_TextureSRV.size() > 0) is_built = true;
}
//
bool sprite_simple::buffer_check(const std::string &name)
{
	if (map_name.find(name) == map_name.end()) return false;
	return (tex_mgr.m_TextureSRV.find(map_name[name]) != tex_mgr.m_TextureSRV.end());
}
//
void sprite_simple::draw_d3d(
	const std::map<std::string, size_t> &map_sprite_rect,
	const std::vector<ui_rect> &rect,
	const std::map<std::string, std::string> &map_sprite_name)
{
	if (!is_built) return;
	sprite_batch->Begin(SpriteSortMode_Deferred, states->NonPremultiplied());
	// Draw sprite according UI rect active
	for (auto &map: map_sprite_rect) {
		if (rect[map.second].active) {
			const std::string *s_name = &map_sprite_name.at(map.first);
			assert(buffer_check(*s_name));
			sprite_batch->Draw(
				tex_mgr.m_TextureSRV[map_name[*s_name]].Get(),
				map_pos[*s_name],
				nullptr,
				Colors::White,
				0,
				XMFLOAT2(0.0f, 0.0f),
				scale);
		}
	}
	sprite_batch->End();
}
//
void sprite_simple::on_resize(const float &scale_in)
{
	scale = scale_in;
	// ensure map_pos do on_resize at the same time in be called on_resize function
}
//
}
#endif