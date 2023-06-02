////////////////
// phy_prepare.h
// by Huang Yiting
////////////////
////////////////
#ifndef PHY_PREPARE_H
#define PHY_PREPARE_H
#include <DirectXMath.h>
#include <DirectXCollision.h>
#include <vector>
#include <string>
#include <algorithm>
using namespace DirectX;
namespace imm
{
////////////////
// phy_property
////////////////
////////////////
struct phy_property
{
	phy_property();
	void init(const int &inst_ix) {ix = inst_ix;}
	void clear_indirect();
	bool is_vel_zero();
	XMFLOAT3 velocity;
	XMFLOAT3 vel_indirect; // velcoity of no direct mechanical motion, for example: walk, run
	XMFLOAT3 vel_bring;
	XMFLOAT3 vel_absolute;
	XMFLOAT3 acceleration;
	XMFLOAT3 *p_aabb3;
	float mass; // not use now
	float friction_rev; // reversed coefficient of friction, 0 <= this <= 1, ignore original friction > 1
	float friction_rev_give;
	float bounce;
	float avg_extent;
	float min_extent; // not use now
	float dt;
	float drop_time;
	int stand_on;
	int ix;
	int bring_ix;
	int intera_tp;
	int absolute_alt;
	bool is_land;
	bool is_on_land;
	bool is_abnormal;
	bool is_switch_Y_Z;
	bool is_on_screen;
	bool is_drop_fix;
};
//
phy_property::phy_property():
	velocity(0.0f, 0.0f, 0.0f),
	vel_indirect(0.0f, 0.0f, 0.0f),
	vel_bring(0.0f, 0.0f, 0.0f),
	vel_absolute(0.0f, 0.0f, 0.0f),
	acceleration(0.0f, 0.0f, 0.0f),
	p_aabb3(nullptr),
	mass(0.0f),
	friction_rev(0.3f),
	friction_rev_give(-1.0f),
	bounce(0.3f),
	avg_extent(1.0f),
	min_extent(1.0f),
	dt(0.0f),
	drop_time(0.0f),
	stand_on(-1),
	ix(0),
	bring_ix(-1),
	intera_tp(0),
	absolute_alt(-1),
	is_land(false),
	is_on_land(false),
	is_abnormal(false),
	is_switch_Y_Z(false),
	is_on_screen(false),
	is_drop_fix(false)
{
	;
}
//
void phy_property::clear_indirect()
{
	vel_indirect = XMFLOAT3();
}
//
bool phy_property::is_vel_zero()
{
	bool is_zero = true;
	float vel_limit = 0.001f;
	if (XMVectorGetX(XMVector3LengthEst(XMLoadFloat3(&velocity))) > vel_limit) is_zero = false;
	if (XMVectorGetX(XMVector3LengthEst(XMLoadFloat3(&vel_indirect))) >vel_limit) is_zero = false;
	if (XMVectorGetX(XMVector3LengthEst(XMLoadFloat3(&vel_absolute))) > vel_limit) is_zero = false;
	return is_zero;
}
////////////////
// phy_set_box
////////////////
////////////////
template <typename T_vertices, typename T_pos, typename T_bound>
void phy_set_box(
	T_bound &bbox,
	const T_vertices &vertices,
	const T_pos get_pos,
	size_t ix_begin = 0,
	size_t ix_end = 0)
{
	if (ix_end == 0) ix_end = vertices.size();
	if (ix_begin > ix_end) ix_begin = 0;
	XMFLOAT3 v_minf3(+FLT_MAX, +FLT_MAX, +FLT_MAX);
	XMFLOAT3 v_maxf3(-FLT_MAX, -FLT_MAX, -FLT_MAX);
	XMVECTOR v_min = XMLoadFloat3(&v_minf3);
	XMVECTOR v_max = XMLoadFloat3(&v_maxf3);
	for (; ix_begin != ix_end; ++ix_begin) {
		XMVECTOR P = XMLoadFloat3(get_pos(vertices[ix_begin]));
		v_min = XMVectorMin(v_min, P);
		v_max = XMVectorMax(v_max, P);
	}
	XMStoreFloat3(&bbox.Center, XMVectorScale(XMVectorAdd(v_min, v_max), 0.5f));
	XMStoreFloat3(&bbox.Extents, XMVectorScale(XMVectorSubtract(v_max, v_min), 0.5f));
	return;
}
////////////////
// phy_set_sphere
////////////////
////////////////
template <typename T_vertices, typename T_pos>
void phy_set_sphere(
	BoundingSphere &sphere,
	const T_vertices &vertices,
	const T_pos get_pos,
	size_t ix_begin = 0,
	size_t ix_end = 0)
{
	if (ix_end == 0) ix_end = vertices.size();
	if (ix_begin > ix_end) ix_begin = 0;
	XMFLOAT3 v_minf3(+FLT_MAX, +FLT_MAX, +FLT_MAX);
	XMFLOAT3 v_maxf3(-FLT_MAX, -FLT_MAX, -FLT_MAX);
	XMVECTOR v_min = XMLoadFloat3(&v_minf3);
	XMVECTOR v_max = XMLoadFloat3(&v_maxf3);
	for (; ix_begin != ix_end; ++ix_begin) {
		XMVECTOR P = XMLoadFloat3(get_pos(vertices[ix_begin]));
		v_min = XMVectorMin(v_min, P);
		v_max = XMVectorMax(v_max, P);
	}
	XMStoreFloat3(&sphere.Center, XMVectorScale(XMVectorAdd(v_min, v_max), 0.5f));
	XMVECTOR extents = XMVectorScale(XMVectorSubtract(v_max, v_min), 0.5f);
	float largest = XMVectorGetX(extents);
	if (XMVectorGetY(extents) > largest) largest = XMVectorGetY(extents);
	if (XMVectorGetZ(extents) > largest) largest = XMVectorGetZ(extents);
	sphere.Radius = largest;
	return;
}
////////////////
// phy_set_box_offset
////////////////
////////////////
template <typename T_bound>
void phy_set_box_offset(T_bound &bbox, const std::vector<float> &offset, const bool &is_forward)
{
	assert(offset.size() > 5);
	if (is_forward) {
		bbox.Extents.x *= offset[0];
		bbox.Extents.y *= offset[1];
		bbox.Extents.z *= offset[2];
		bbox.Center.x += offset[3];
		bbox.Center.y += offset[4];
		bbox.Center.z += offset[5];
	}
	else {
		bbox.Extents.x /= offset[0];
		bbox.Extents.y /= offset[1];
		bbox.Extents.z /= offset[2];
		bbox.Center.x -= offset[3];
		bbox.Center.y -= offset[4];
		bbox.Center.z -= offset[5];
	}
}
////////////////
// PHY_INTERACTIVE_TYPE
////////////////
////////////////
enum PHY_INTERACTIVE_TYPE
{
	PHY_INTERA_DUMMY       = 0x0, // not use
	PHY_INTERA_FIXED       = 0x1,
	PHY_INTERA_NO_BOUND    = 0x2, // not use
	PHY_INTERA_STATIC      = 0x4,
	PHY_INTERA_INVISIBLE   = 0x8, // only attr
	PHY_INTERA_FIXED_INVIS = 0x9, // not use
	PHY_INTERA_MOVABLE     = 0x14,
	PHY_INTERA_FIXED_AIR   = 0x21,
	PHY_INTERA_FIXED_LIMIT = 0x89,
};
/*
0x00 0000 0000 DUMMY
0x01 0000 0001 FIXED
0x02 0000 0010 NO_BOUND
0x04 0000 0100 STATIC
0x08 0000 1000 INVISIBLE
0x09 0000 1001 FIXED_INVIS
0x14 0001 0100 MOVABLE
0x21 0010 0001 FIXED_AIR
0x89 1000 1001 FIXED_LIMIT
*/
//
PHY_INTERACTIVE_TYPE phy_interactive_type_str(const std::string &str)
{
	if (str == "FIXED") return PHY_INTERA_FIXED;
	if (str == "STATIC") return PHY_INTERA_STATIC;
	if (str == "FIXED_INVIS") return PHY_INTERA_FIXED_INVIS;
	if (str == "MOVABLE") return PHY_INTERA_MOVABLE;
	if (str == "FIXED_AIR") return PHY_INTERA_FIXED_AIR;
	if (str == "FIXED_LIMIT") return PHY_INTERA_FIXED_LIMIT;
	ERROR_MESA("PHY_INTERACTIVE_TYPE error");
}
////////////////
// PHY_BOUND_TYPE
////////////////
////////////////
enum PHY_BOUND_TYPE
{
	PHY_BOUND_BOX     = 0,
	PHY_BOUND_ORI_BOX = 1,
	PHY_BOUND_SPHERE  = 2,
	PHY_BOUND_NULL    = 3,
};
//
PHY_BOUND_TYPE phy_bound_type_str(const std::string &str)
{
	if (str == "BOX") return PHY_BOUND_BOX;
	if (str == "ORI_BOX") return PHY_BOUND_ORI_BOX;
	if (str == "SPHERE") return PHY_BOUND_SPHERE;
	if (str == "NULL") return PHY_BOUND_NULL;
	ERROR_MESA("PHY_BOUND_TYPE error");
}
//
////////////////
// phy_bound_alter
////////////////
////////////////
struct phy_bound_alter
{
	std::vector<float> offset;
};
////////////////
// phy_bound_mgr
// manager various types of bounding
////////////////
////////////////
template <typename T_app>
class phy_bound_mgr
{
public:
	phy_bound_mgr();
	T_app *app;
	std::vector<BoundingBox> bd0;
	std::vector<BoundingOrientedBox> bd1;
	std::vector<BoundingSphere> bd2;
	std::vector<std::pair<PHY_BOUND_TYPE, size_t>> map;
	std::map<size_t, phy_bound_alter> alter;
	bool is_altered;
	void init(T_app *app_in);
	void push_back_empty(const PHY_BOUND_TYPE &type);
	template <typename T_vertices, typename T_pos>
	void phy_set_box_back(const PHY_BOUND_TYPE &type, const T_vertices &vertices, const T_pos get_pos, size_t ix_begin = 0, size_t ix_end = 0);
	void set_box_offset(const size_t &ix, const std::vector<float> &offset);
	void set_box_offset(const size_t &ix, const std::vector<float> &offset, const bool &is_forward);
	void set_phy_value(const size_t &ix);
	void switch_box_alter_offset(const bool &is_alter, const phy_bound_mgr &mgr_in);
	void transform(const size_t &ix, phy_bound_mgr &out, CXMMATRIX &world);
	bool intersects(const size_t &ixA, const size_t &ixB);
	bool intersects(const size_t &ix, CXMVECTOR &origin, CXMVECTOR &direction, float &dist);
	template <typename T_bound>
	bool intersects(const size_t &ix, const T_bound &bound);
	bool intersects_phy_stand(const size_t &ixA, const size_t &ixB);
	bool is_equal_bound(const size_t &ixA, const size_t &ixB);
	template <typename T_object>
	ContainmentType contains(const size_t &ix, const T_object &object);
	void pick(
		const int &pos_x,
		const int &pos_y,
		const int &client_width,
		const int &clinet_height,
		CXMMATRIX cam_proj,
		CXMMATRIX cam_view,
		int &out_ix);
	//
	const XMFLOAT3& center(const size_t &ix);
	float extents_y(const size_t &ix);
	float extents_x(const size_t &ix);
	float extents_z(const size_t &ix);
	XMVECTOR direction(const size_t &ix);
	PHY_BOUND_TYPE get_type(const size_t &ix);
	void remove_all();
};
//
template <typename T_app>
phy_bound_mgr<T_app>::phy_bound_mgr():
	bd0(),
	bd1(),
	bd2(),
	map(),
	alter(),
	is_altered(true)
{
	;
}
//
template <typename T_app>
void phy_bound_mgr<T_app>::init(T_app *app_in)
{
	app = app_in;
}
//
template <typename T_app>
void phy_bound_mgr<T_app>::push_back_empty(const PHY_BOUND_TYPE &type)
{
	switch(type) {
	case PHY_BOUND_BOX:
		bd0.emplace_back();
		map.emplace_back(type, bd0.size()-1);
		return;
	case PHY_BOUND_ORI_BOX:
		bd1.emplace_back();
		map.emplace_back(type, bd1.size()-1);
		return;
	case PHY_BOUND_SPHERE:
		bd2.emplace_back();
		map.emplace_back(type, bd2.size()-1);
		return;
	case PHY_BOUND_NULL:
		assert(false);
		map.emplace_back(type, 0);
		return;
	}
	assert(false);
}
template <typename T_app>
template <typename T_vertices, typename T_pos>
void phy_bound_mgr<T_app>::phy_set_box_back(
	const PHY_BOUND_TYPE &type,
	const T_vertices &vertices,
	const T_pos get_pos,
	size_t ix_begin,
	size_t ix_end
)
{
	switch(type) {
	case PHY_BOUND_BOX:
		phy_set_box(bd0.back(), vertices, get_pos, ix_begin, ix_end);
		break;
	case PHY_BOUND_ORI_BOX:
		phy_set_box(bd1.back(), vertices, get_pos, ix_begin, ix_end);
		bd1.back().Orientation = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
		break;
	case PHY_BOUND_SPHERE:
		// not implement
		assert(false);
		break;
	}
}
//
template <typename T_app>
void phy_bound_mgr<T_app>::set_box_offset(const size_t &ix, const std::vector<float> &offset)
{
	alter[ix].offset = offset;
	switch(map[ix].first) {
	case PHY_BOUND_BOX:
		phy_set_box_offset(bd0[map[ix].second], offset, true);
		break;
	case PHY_BOUND_ORI_BOX:
		phy_set_box_offset(bd1[map[ix].second], offset, true);
		break;
	default:
		assert(false);
	}
}
//
template <typename T_app>
void phy_bound_mgr<T_app>::set_box_offset(const size_t &ix, const std::vector<float> &offset, const bool &is_forward)
{
	switch(map[ix].first) {
	case PHY_BOUND_BOX:
		phy_set_box_offset(bd0[map[ix].second], offset, is_forward);
		break;
	case PHY_BOUND_ORI_BOX:
		phy_set_box_offset(bd1[map[ix].second], offset, is_forward);
		break;
	default:
		assert(false);
	}
}
template <typename T_app>
void phy_bound_mgr<T_app>::set_phy_value(const size_t &ix)
{
	XMFLOAT3 extent;
	float min_extent;
	float sum_extent;
	//
	auto p_inst = app->m_Inst.m_Stat[ix].p_inst;
	switch(app->m_Inst.m_Stat[ix].type) {
	case MODEL_BASIC:
		app->m_Inst.m_Stat[ix].phy.intera_tp = 
			((basic_model_instance*)p_inst)->model->m_InteractiveType;
		break;
	case MODEL_SKINNED:
		app->m_Inst.m_Stat[ix].phy.intera_tp = 
			((skinned_model_instance*)p_inst)->model->m_InteractiveType;
		break;
	case MODEL_SIMPLE_P:
		app->m_Inst.m_Stat[ix].phy.intera_tp = 
			((simple_model_instance<vertex::pntt>*)p_inst)->model->m_NameInteractiveType.at(
			((simple_model_instance<vertex::pntt>*)p_inst)->model_name);
		break;
	}
	//
	switch(map[ix].first) {
	case PHY_BOUND_BOX:
		// phy_impulse_casual() calculate PHY_BOUND_BOX collision normal vector
		// PHY_BOUND_ORI_BOX, PHY_BOUND_SPHERE not use p_aabb3
		app->m_Inst.m_Stat[ix].phy.p_aabb3 = &bd0[map[ix].second].Extents;
		extent = bd0[map[ix].second].Extents;
		sum_extent = extent.x + extent.y + extent.z;
		app->m_Inst.m_Stat[ix].phy.avg_extent = sum_extent/3.0f;
		min_extent = (std::min)(extent.x, extent.y);
		min_extent = (std::min)(min_extent, extent.z);
		app->m_Inst.m_Stat[ix].phy.min_extent = min_extent;
		break;
	case PHY_BOUND_ORI_BOX:
		app->m_Inst.m_Stat[ix].phy.is_switch_Y_Z = 
			((skinned_model_instance*)p_inst)->model->m_IsRotXneg90Y180;
		app->m_Inst.m_Stat[ix].phy.p_aabb3 = &bd1[map[ix].second].Extents;
		extent = bd1[map[ix].second].Extents;
		sum_extent = extent.x + extent.y + extent.z;
		app->m_Inst.m_Stat[ix].phy.avg_extent = sum_extent/3.0f;
		min_extent = (std::min)(extent.x, extent.y);
		min_extent = (std::min)(min_extent, extent.z);
		app->m_Inst.m_Stat[ix].phy.min_extent = min_extent;
		break;
	case PHY_BOUND_SPHERE:
		app->m_Inst.m_Stat[ix].phy.p_aabb3 = nullptr;
		app->m_Inst.m_Stat[ix].phy.avg_extent = bd2[map[ix].second].Radius;
		app->m_Inst.m_Stat[ix].phy.min_extent = bd2[map[ix].second].Radius;
		break;
	default:
		assert(false);
	}
	//
	app->m_Inst.m_Stat[ix].phy.init(static_cast<int>(ix));
}
//
template <typename T_app>
void phy_bound_mgr<T_app>::switch_box_alter_offset(const bool &is_alter, const phy_bound_mgr &mgr_in)
{
	if (is_alter) {
		assert(!is_altered);
		for (auto &alt: mgr_in.alter) {
			set_box_offset(alt.first, alt.second.offset, true);
		}
		is_altered = true;
	}
	else {
		assert(is_altered);
		for (auto &alt: mgr_in.alter) {
			set_box_offset(alt.first, alt.second.offset, false);
		}
		is_altered = false;
	}
}
//
template <typename T_app>
void phy_bound_mgr<T_app>::transform(const size_t &ix, phy_bound_mgr &out, CXMMATRIX &world)
{
	switch(map[ix].first) {
	case PHY_BOUND_BOX: bd0[map[ix].second].Transform(out.bd0[map[ix].second], world); return;
	case PHY_BOUND_ORI_BOX: bd1[map[ix].second].Transform(out.bd1[map[ix].second], world); return;
	case PHY_BOUND_SPHERE: bd2[map[ix].second].Transform(out.bd2[map[ix].second], world); return;
	}
}
//
template <typename T_app>
bool phy_bound_mgr<T_app>::intersects(const size_t &ixA, const size_t &ixB)
{
	switch(map[ixA].first) {
	case PHY_BOUND_BOX:
		switch(map[ixB].first) {
		case PHY_BOUND_BOX: return bd0[map[ixA].second].Intersects(bd0[map[ixB].second]);
		case PHY_BOUND_ORI_BOX: return bd0[map[ixA].second].Intersects(bd1[map[ixB].second]);
		case PHY_BOUND_SPHERE: return bd0[map[ixA].second].Intersects(bd2[map[ixB].second]);
		}
	case PHY_BOUND_ORI_BOX:
		switch(map[ixB].first) {
		case PHY_BOUND_BOX: return bd1[map[ixA].second].Intersects(bd0[map[ixB].second]);
		case PHY_BOUND_ORI_BOX: return bd1[map[ixA].second].Intersects(bd1[map[ixB].second]);
		case PHY_BOUND_SPHERE: return bd1[map[ixA].second].Intersects(bd2[map[ixB].second]);
		}
	case PHY_BOUND_SPHERE:
		switch(map[ixB].first) {
		case PHY_BOUND_BOX: return bd2[map[ixA].second].Intersects(bd0[map[ixB].second]);
		case PHY_BOUND_ORI_BOX: return bd2[map[ixA].second].Intersects(bd1[map[ixB].second]);
		case PHY_BOUND_SPHERE: return bd2[map[ixA].second].Intersects(bd2[map[ixB].second]);
		}
	}
	assert(false);
	return false;
}
//
template <typename T_app>
bool phy_bound_mgr<T_app>::intersects(const size_t &ix, CXMVECTOR &origin, CXMVECTOR &direction, float &dist)
{
	switch(map[ix].first) {
	case PHY_BOUND_BOX: return bd0[map[ix].second].Intersects(origin, direction, dist);
	case PHY_BOUND_ORI_BOX: return bd1[map[ix].second].Intersects(origin, direction, dist);
	case PHY_BOUND_SPHERE: return bd2[map[ix].second].Intersects(origin, direction, dist);
	}
	assert(false);
	return false;
}
//
template <typename T_app>
template <typename T_bound>
bool phy_bound_mgr<T_app>::intersects(const size_t &ix, const T_bound &bound)
{
	switch(map[ix].first) {
	case PHY_BOUND_BOX: return bd0[map[ix].second].Intersects(bound);
	case PHY_BOUND_ORI_BOX: return bd1[map[ix].second].Intersects(bound);
	case PHY_BOUND_SPHERE: return bd2[map[ix].second].Intersects(bound);
	}
	assert(false);
	return false;
}
//
template <typename T_app>
bool phy_bound_mgr<T_app>::intersects_phy_stand(const size_t &ixA, const size_t &ixB)
{
	BoundingBox boxA;
	XMFLOAT3 centerB;
	XMVECTOR CB;
	ContainmentType contype = DISJOINT;
	switch(map[ixB].first) {
	case PHY_BOUND_BOX: centerB = bd0[map[ixB].second].Center; return true; break;
	case PHY_BOUND_ORI_BOX: centerB = bd1[map[ixB].second].Center; break;
	case PHY_BOUND_SPHERE: centerB = bd2[map[ixB].second].Center; break;
	}
	switch(map[ixA].first) {
	case PHY_BOUND_BOX:
		boxA = bd0[map[ixA].second];
		centerB.y = boxA.Center.y;
		CB = XMLoadFloat3(&centerB);
		contype = boxA.Contains(CB);
		return (contype != DISJOINT);
	case PHY_BOUND_ORI_BOX: return true;
	case PHY_BOUND_SPHERE: return true;
	}
	return true;
}
//
template <typename T_app>
bool phy_bound_mgr<T_app>::is_equal_bound(const size_t &ixA, const size_t &ixB)
{
	if (get_type(ixA) != get_type(ixB)) return false;
	float diff = 0.001;
	if (extents_y(ixA) - extents_y(ixB) > diff) return false;
	if (extents_x(ixA) - extents_x(ixB) > diff) return false;
	if (extents_z(ixA) - extents_z(ixB) > diff) return false;
	if (get_type(ixA) == PHY_BOUND_ORI_BOX) {
		if (!XMVector3Equal(direction(ixA), direction(ixB))) return false;
	}
	return true;
}
//
template <typename T_app>
template <typename T_object>
ContainmentType phy_bound_mgr<T_app>::contains(const size_t &ix, const T_object &object)
{
	switch(map[ix].first) {
	case PHY_BOUND_BOX: return bd0[map[ix].second].Contains(object);
	case PHY_BOUND_ORI_BOX: return bd1[map[ix].second].Contains(object);
	case PHY_BOUND_SPHERE: return bd2[map[ix].second].Contains(object);
	}
	assert(false);
	return ContainmentType::DISJOINT;
}
//
template <typename T_app>
void phy_bound_mgr<T_app>::pick(
	const int &pos_x,
	const int &pos_y,
	const int &client_width,
	const int &clinet_height,
	CXMMATRIX cam_proj,
	CXMMATRIX cam_view,
	int &out_ix)
{
	// Compute picking ray in view space.
	float vx = (+2.0f*pos_x/client_width  - 1.0f)/cam_proj.r[0].m128_f32[0];
	float vy = (-2.0f*pos_y/clinet_height + 1.0f)/cam_proj.r[1].m128_f32[1];
	// Ray definition in view space.
	XMVECTOR ray_origin = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
	XMVECTOR ray_dir = XMVectorSet(vx, vy, 1.0f, 0.0f);
	// Tranform ray to world.
	XMVECTOR det_view = XMMatrixDeterminant(cam_view);
	XMMATRIX inv_view = XMMatrixInverse(&det_view, cam_view);
	ray_origin = XMVector3TransformCoord(ray_origin, inv_view);
	ray_dir = XMVector3TransformNormal(ray_dir, inv_view);
	// Make the ray direction unit length for the intersection tests.
	ray_dir = XMVector3Normalize(ray_dir);
	int picked_box = -1;
	float tmin = FLT_MAX;
	// Find the nearest ray/box intersection.
	for (size_t ix = 0; ix != map.size(); ++ix) {
		float t = 0.0f;
		if (!app->m_Inst.m_Stat[ix].is_invoke_physics()) continue;
		if (intersects(ix, ray_origin, ray_dir, t)) {
			if (t < tmin) {tmin = t; picked_box = static_cast<int>(ix);}
		}
	}
	out_ix = picked_box;
}
//
template <typename T_app>
const XMFLOAT3& phy_bound_mgr<T_app>::center(const size_t &ix)
{
	switch(map[ix].first) {
	case PHY_BOUND_BOX: return bd0[map[ix].second].Center;
	case PHY_BOUND_ORI_BOX: return bd1[map[ix].second].Center;
	case PHY_BOUND_SPHERE: return bd2[map[ix].second].Center;
	}
	assert(false);
	return bd0[map[ix].second].Center;
}
//
template <typename T_app>
float phy_bound_mgr<T_app>::extents_y(const size_t &ix)
{
	switch(map[ix].first) {
	case PHY_BOUND_BOX:
		return bd0[map[ix].second].Extents.y;
	case PHY_BOUND_ORI_BOX:
		if (app->m_Inst.m_Stat[ix].phy.is_switch_Y_Z) return bd1[map[ix].second].Extents.z;
		return bd1[map[ix].second].Extents.y;
	case PHY_BOUND_SPHERE:
		return bd2[map[ix].second].Radius;
	}
	assert(false);
	return 0.0f;
}
//
template <typename T_app>
float phy_bound_mgr<T_app>::extents_x(const size_t &ix)
{
	switch(map[ix].first) {
	case PHY_BOUND_BOX: return bd0[map[ix].second].Extents.x;
	case PHY_BOUND_ORI_BOX: return bd1[map[ix].second].Extents.x;
	case PHY_BOUND_SPHERE: return bd2[map[ix].second].Radius;
	}
	assert(false);
	return 0.0f;
}
//
template <typename T_app>
float phy_bound_mgr<T_app>::extents_z(const size_t &ix)
{
	switch(map[ix].first) {
	case PHY_BOUND_BOX:
		return bd0[map[ix].second].Extents.z;
	case PHY_BOUND_ORI_BOX:
		if (app->m_Inst.m_Stat[ix].phy.is_switch_Y_Z) return bd1[map[ix].second].Extents.y;
		return bd1[map[ix].second].Extents.z;
	case PHY_BOUND_SPHERE:
		return bd2[map[ix].second].Radius;
	}
	assert(false);
	return 0.0f;
}
//
template <typename T_app>
XMVECTOR phy_bound_mgr<T_app>::direction(const size_t &ix)
{
	XMFLOAT3 corners[8];
	bd0[map[ix].second].GetCorners(corners);
	switch(map[ix].first) {
	case PHY_BOUND_BOX:
		bd0[map[ix].second].GetCorners(corners);
		assert(false);
		break;
	case PHY_BOUND_ORI_BOX:
		bd1[map[ix].second].GetCorners(corners);
		break;
	case PHY_BOUND_SPHERE:
		corners;
		assert(false);
		break;
	}
	XMVECTOR cor1 = XMLoadFloat3(&corners[7]);
	XMVECTOR cor2 = XMLoadFloat3(&corners[4]);
	XMVECTOR dir1 = XMVectorSubtract(cor2, cor1);
	dir1 = XMVector3Normalize(dir1);
	return dir1;
}
template <typename T_app>
PHY_BOUND_TYPE phy_bound_mgr<T_app>::get_type(const size_t &ix)
{
	return map[ix].first;
}
//
template <typename T_app>
void phy_bound_mgr<T_app>::remove_all()
{
	bd0.clear();
	bd1.clear();
	bd2.clear();
	bd0.reserve(VECTOR_RESERVE);
	bd1.reserve(VECTOR_RESERVE);
	bd2.reserve(VECTOR_RESERVE);
	map.clear();
	alter.clear();
	is_altered = true;
}
//
}
#endif