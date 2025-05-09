////////////////
// phy_position_logic.h
// by Huang Yiting
////////////////
////////////////
#ifndef PHY_POSITION_LOGIC_H
#define PHY_POSITION_LOGIC_H
#include "phy_prepare.h"
namespace imm
{
// note if every delta time is not equal, simulation will be some distortion
////////////////
// PHY const
////////////////
////////////////
static const float PHY_FLOAT_EQUAL_1METER = 5.0f;
static const float PHY_GRAVITY_RATE = 1.5f;
static const float PHY_GRAVITY = -9.8f*PHY_FLOAT_EQUAL_1METER*PHY_GRAVITY_RATE;
// if too small velocity rebound, ignore it
static const float PHY_IGNORE_GRAVITY = 1.8f;
// if runtime stun, restrict delta time not too big
static const float PHY_MAX_DELTA_TIME = 0.034f;
static const float PHY_DIFF_JUDGE = 1.0f;
////////////////
// phy_position
////////////////
////////////////
template <typename T_app>
struct phy_position
{
	phy_position();
	void init(T_app *app_in);
	void update(
		const float &dt,
		XMFLOAT4X4 &world,
		phy_property &prop,
		phy_property &prop_land,
		const XMFLOAT3 &center,
		const float &extents_y,
		const float &land_y);
	void impulse_casual(
		const float &dt,
		const float &extents_y_A,
		const float &extents_y_B,
		XMFLOAT4X4 &world_A,
		XMFLOAT4X4 &world_B,
		phy_property &prop_A,
		phy_property &prop_B,
		const XMFLOAT3 &center_A,
		const XMFLOAT3 &center_B,
		const bool &is_touch);
	void attack_impulse(
		phy_property &prop_B,
		const XMFLOAT3 &center_Hit,
		const XMFLOAT3 &center_B,
		const XMFLOAT3 &center_A,
		const bool &is_touch,
		const float &impulse_scale,
		const size_t &ix_dmg);
	T_app *app;
};
//
template <typename T_app>
phy_position<T_app>::phy_position():
	app(nullptr)
{
	;
}
//
template <typename T_app>
void phy_position<T_app>::init(T_app *app_in)
{
	app = app_in;
}
////////////////
// phy_position::update
////////////////
////////////////
template <typename T_app>
void phy_position<T_app>::update(
	const float &dt,
	XMFLOAT4X4 &world,
	phy_property &prop,
	phy_property &prop_land,
	const XMFLOAT3 &center,
	const float &extents_y,
	const float &land_y)
{
	if (prop.is_abnormal) return;
	if (prop.intera_tp == PHY_INTERA_FIXED_AIR) return;
	bool is_fps60_dt = false;
	prop.dt += dt;
	// Game FPS must >= 60
	if (prop.dt > FPS60_1DIV) {
		prop.dt -= FPS60_1DIV;
		is_fps60_dt = true;
	}
	if (is_fps60_dt) {
		float decay = 0.5f;
		if (prop.absolute_alt > 0) {
			decay = 1.0f;
			--prop.absolute_alt;
		}
		prop.vel_absolute.x *= decay;
		prop.vel_absolute.y *= decay;
		prop.vel_absolute.z *= decay;
		// clear control move vel_indirect
		if (prop.friction_rev_give > 0.0f) {
			prop.vel_indirect.x = 0.0f;
			prop.vel_indirect.z = 0.0f;
		}
	}
	if (!prop.is_on_land) {
		prop.velocity.x += prop.acceleration.x*dt;
		prop.velocity.y += (PHY_GRAVITY+prop.acceleration.y)*dt;
		prop.velocity.z += prop.acceleration.z*dt;
		world._41 += (prop.velocity.x + prop.vel_indirect.x + prop.vel_absolute.x)*dt;
		world._42 += (prop.velocity.y + prop.vel_indirect.y)*dt;
		world._43 += (prop.velocity.z + prop.vel_indirect.z + prop.vel_absolute.z)*dt;
	}
	else {
		float bounce = prop.bounce*prop_land.bounce;
		float friction_rev = prop.friction_rev*prop_land.friction_rev;
		prop.acceleration = XMFLOAT3(0.0f, 0.0f, 0.0f);
		if (prop.velocity.y < 0.0f) prop.velocity.y = -prop.velocity.y*bounce*FPS60*dt;
		if (is_fps60_dt) {
			prop.velocity.x *= friction_rev;
			prop.velocity.z *= friction_rev;
			// braking
			if (prop.friction_rev_give > 0.0f) {
				prop.velocity.x *= prop.friction_rev_give;
				prop.velocity.z *= prop.friction_rev_give;
				prop.friction_rev_give = -1.0f;
			}
		}
		// use center compare stand
		float offset_y = world._42-center.y;
		float center_y = center.y;
		// phy_impulse_casual() produce fake velocity of PHY_INTERA_FIXED
		// but not adjust PHY_INTERA_FIXED's full stand on land
		if (prop.intera_tp & PHY_INTERA_FIXED) return;
		if ((prop.intera_tp != PHY_INTERA_MOVABLE)) {
			// extents_y incorrect when ori box
			if (app->m_Inst.m_BoundW.get_type(prop.ix) == PHY_BOUND_ORI_BOX) {
				return;
			}
		}
		// stand_adjust keep object full stand on land
		// guarantee object.intersects(land) return true, exclude if they have a little gap
		float stand_adjust = -0.01f;
		float stand = extents_y+land_y+stand_adjust;
		center_y += (prop.velocity.y + prop.vel_indirect.y)*dt;
		world._41 += (prop.velocity.x + prop.vel_indirect.x + prop.vel_absolute.x)*dt;
		world._43 += (prop.velocity.z + prop.vel_indirect.z + prop.vel_absolute.z)*dt;
		if (abs(prop.velocity.y) < PHY_IGNORE_GRAVITY) prop.velocity.y = 0.0f;
		if (center_y < stand) center_y = stand;
		world._42 = center_y+offset_y;
	}
	// too big number
	float vel_max = 1000.0f;
	if (abs(prop.velocity.x) > vel_max ||
		abs(prop.velocity.y) > vel_max ||
		abs(prop.velocity.z) > vel_max) {
		prop.is_abnormal = true;
	}
	return;
}
////////////////
// phy_position::impulse_casual
// method reference 1:
// (http://gamedevelopment.tutsplus.com/tutorials/
// how-to-create-a-custom-2d-physics-engine-the-basics-and-impulse-resolution--gamedev-6331)
// method reference 2:
// \Microsoft DirectX SDK (June 2010)\Samples\C++\Direct3D\ConfigSystem\main.cpp
// it is originally used with two spheres, but there is used with two AABB or others, inaccuracy solution
////////////////
////////////////
template <typename T_app>
void phy_position<T_app>::impulse_casual(
	const float &dt,
	const float &extents_y_A,
	const float &extents_y_B,
	XMFLOAT4X4 &world_A,
	XMFLOAT4X4 &world_B,
	phy_property &prop_A,
	phy_property &prop_B,
	const XMFLOAT3 &center_A,
	const XMFLOAT3 &center_B,
	const bool &is_touch)
{
	if (!is_touch) return;
	if ((prop_A.intera_tp & PHY_INTERA_FIXED) && (prop_B.intera_tp & PHY_INTERA_FIXED)) return;
	XMVECTOR c_A = XMLoadFloat3(&center_A);
	XMVECTOR c_B = XMLoadFloat3(&center_B);
	XMMATRIX w_A = XMLoadFloat4x4(&world_A);
	XMMATRIX w_B = XMLoadFloat4x4(&world_B);
	// AtoB is the collision normal vector.
	XMVECTOR AtoB = XMVectorSubtract(c_B, c_A);
	float relative_size = 1.001f;
	if (prop_A.p_aabb3 && prop_B.p_aabb3) {
	relative_size = abs(prop_A.avg_extent-prop_B.avg_extent)/(std::min)(prop_A.avg_extent, prop_B.avg_extent);
	//
	bool is_stand_AtoB = false;
	// check if instance stand on instance
	// notice center_A, center_B is not updated in this cycle
	auto func_stand = [&] (phy_property &prop_P, size_t indexQ) {
		bool test_stand = app->m_Inst.m_BoundW.intersects_phy_stand(indexQ, prop_P.ix);
		if (!test_stand) return;
		prop_P.stand_on = static_cast<int>(indexQ);
		prop_P.is_on_land = true;
		is_stand_AtoB = true;
		AtoB = XMVectorSetX(AtoB, 0.0f);
		AtoB = XMVectorSetZ(AtoB, 0.0f);
	};
	if (center_A.y > center_B.y) {
		float diff = center_A.y-extents_y_A-(center_B.y+extents_y_B);
		if (abs(diff) < PHY_DIFF_JUDGE) {
			// if already on land
			if (prop_A.is_on_land) return;
			func_stand(prop_A, prop_B.ix);
		}
	}
	else {
		float diff = center_B.y-extents_y_B-(center_A.y+extents_y_A);
		if (abs(diff) < PHY_DIFF_JUDGE) {
			// if already on land
			if (prop_B.is_on_land) return;
			func_stand(prop_B, prop_A.ix);
		}
	}
	//
	if (relative_size > 1.0f && (!is_stand_AtoB)) {
		XMFLOAT3 extents_A = *prop_A.p_aabb3;
		XMFLOAT3 extents_B = *prop_B.p_aabb3;
		if (prop_A.is_switch_Y_Z) {
			extents_A.y = prop_A.p_aabb3->z;
			extents_A.z = prop_A.p_aabb3->y;
		}
		if (prop_B.is_switch_Y_Z) {
			extents_B.y = prop_B.p_aabb3->z;
			extents_B.z = prop_B.p_aabb3->y;
		}
		//
		float big_x = (std::max)(extents_A.x, extents_B.x);
		if (big_x < abs(XMVectorGetX(AtoB)) ) {
			AtoB = XMVectorSetZ(AtoB, 0.0f);
			AtoB = XMVectorSetY(AtoB, 0.0f);
		}
		float big_z = (std::max)(extents_A.z, extents_B.z);
		if (big_z < abs(XMVectorGetZ(AtoB)) ) {
			AtoB = XMVectorSetX(AtoB, 0.0f);
			AtoB = XMVectorSetY(AtoB, 0.0f);
		}
		float big_y = (std::max)(extents_A.y, extents_B.y);
		if (big_y < abs(XMVectorGetY(AtoB)) ) {
			AtoB = XMVectorSetX(AtoB, 0.0f);
			AtoB = XMVectorSetZ(AtoB, 0.0f);
		}
	}}
	AtoB = XMVector3Normalize(AtoB);
	//
	XMVECTOR vel_A = XMLoadFloat3(&prop_A.velocity);
	XMVECTOR vel_B = XMLoadFloat3(&prop_B.velocity);
	// empirical formula, vel_indirect is different from velocity
	XMVECTOR vel_A_indir = XMLoadFloat3(&prop_A.vel_indirect);
	XMVECTOR vel_B_indir = XMLoadFloat3(&prop_B.vel_indirect);
	XMVECTOR vel_A_all = XMVectorAdd(vel_A, vel_A_indir);
	XMVECTOR vel_B_all = XMVectorAdd(vel_B, vel_B_indir);
	vel_A_all = XMVectorAdd(vel_A_all, XMLoadFloat3(&prop_A.vel_absolute));
	vel_B_all = XMVectorAdd(vel_B_all, XMLoadFloat3(&prop_B.vel_absolute));
	//
	if (prop_B.bring_ix != prop_A.ix) {
		XMVECTOR vel_B_bring = XMLoadFloat3(&prop_B.vel_bring);
		vel_B_all = XMVectorAdd(vel_B_all, vel_B_bring);
	}
	if (prop_A.bring_ix != prop_B.ix) {
		XMVECTOR vel_A_bring = XMLoadFloat3(&prop_A.vel_bring);
		vel_A_all = XMVectorAdd(vel_A_all, vel_A_bring);
	}
	// bounce
	float bounce = prop_A.bounce*prop_B.bounce;
	XMVECTOR vel_AB_all = XMVectorSubtract(vel_A_all, vel_B_all);
	// ignore mass
	XMVECTOR injected_impulse = XMVectorScale(
		AtoB, XMVectorGetX(XMVector3Dot(XMVectorScale(vel_AB_all, -1.0f*bounce), AtoB)));
	injected_impulse = XMVectorScale(injected_impulse, FPS60*dt);
	vel_A = XMVectorAdd(vel_A, injected_impulse);
	vel_B = XMVectorSubtract(vel_B, injected_impulse);
	// translate force as estimated velocity value
	auto func_bring_adjust = [&] (phy_property &prop_P, phy_property &prop_Q, XMVECTOR &vel_Q_all) {
		XMVECTOR bring_P = XMVectorScale(vel_Q_all, 1.5f);
		// avoid too big gravity or some force (overlapping objects)
		if (abs(XMVectorGetY(bring_P)) > 10.0f) bring_P = XMVectorSetY(bring_P, 0.0f);
		if (abs(XMVectorGetX(bring_P)) > 100.0f) bring_P = XMVectorSetX(bring_P, 0.0f);
		if (abs(XMVectorGetZ(bring_P)) > 100.0f) bring_P = XMVectorSetZ(bring_P, 0.0f);
		XMStoreFloat3(&prop_P.vel_bring, bring_P);
		prop_P.bring_ix = prop_Q.ix;
	};
	if (prop_A.intera_tp & PHY_INTERA_STATIC) {
		func_bring_adjust(prop_A, prop_B, vel_B_all);
	}
	if (prop_B.intera_tp & PHY_INTERA_STATIC) {
		func_bring_adjust(prop_B, prop_A, vel_A_all);
	}
	// penetration depth estimate as vel_absolute
	// fix the positions so that the two objects are apart, not accurate
	XMVECTOR vel_absolute_A = XMLoadFloat3(&prop_A.vel_absolute);
	XMVECTOR vel_absolute_B = XMLoadFloat3(&prop_B.vel_absolute);
	// small object need more penetration
	bool aabb3_yes = (prop_A.p_aabb3 && prop_B.p_aabb3);
	if (aabb3_yes && relative_size < 1.0f) {
		AtoB = XMVectorSetY(AtoB, XMVectorGetY(AtoB)*0.3f);
		float penetration = XMVectorGetX(XMVector3LengthEst(XMVectorSubtract(vel_A_all, vel_B_all)))*(dt/FPS60_1DIV);
		AtoB = XMVectorScale(AtoB, 30.0f+penetration);
	}
	// scene boundary
	// c_Q = XMVectorZero;
	auto func_boundary = [&] (XMVECTOR &c_P, XMVECTOR &c_Q, XMMATRIX &w_P, phy_property &prop_P) {
		XMVECTOR lenQ = XMVector3Length(c_Q);
		assert(math::float_is_equal(XMVectorGetX(lenQ), 0.0f));
		XMVECTOR offsetP = XMVectorSubtract(w_P.r[3], c_P);
		float penetration = 50.0f*dt;
		XMVECTOR c_Pxz = XMVectorSetY(c_P, 0.0f);
		XMVECTOR to_scene = XMVectorSubtract(XMVectorZero(), c_Pxz);
		to_scene = XMVector3Normalize(to_scene);
		c_P = XMVectorAdd(c_P, XMVectorScale(to_scene, penetration));
		w_P.r[3] = XMVectorAdd(c_P, offsetP);
		w_P.r[3] = XMVectorSetW(w_P.r[3], 1.0f);
		app->m_SfxSelect.effect_block_task(prop_P.ix);
	};
	if ((aabb3_yes && relative_size >= 1.0f) && (prop_B.intera_tp == PHY_INTERA_FIXED_LIMIT)) {
		func_boundary(c_A, c_B, w_A, prop_A);
	}
	if ((aabb3_yes && relative_size >= 1.0f) && (prop_A.intera_tp == PHY_INTERA_FIXED_LIMIT)) {
		func_boundary(c_B, c_A, w_B, prop_B);
	}
	// big object
	// !(relative_size < 1.0f)
	if ((prop_B.intera_tp != PHY_INTERA_FIXED_LIMIT) && (prop_A.intera_tp != PHY_INTERA_FIXED_LIMIT) && 
		(aabb3_yes && relative_size >= 1.0f)) {
		float penetration = XMVectorGetX(XMVector3LengthEst(XMVectorSubtract(vel_A_all, vel_B_all)))*(dt/FPS60_1DIV);
		penetration *= 2.0f;
		AtoB = XMVectorScale(AtoB, 20.0f+penetration);
	}
	// store result
	if (!(prop_B.intera_tp & PHY_INTERA_FIXED)) {
		if (prop_A.intera_tp == PHY_INTERA_FIXED_LIMIT) XMStoreFloat4x4(&world_B, w_B);
		else {
			vel_absolute_B = XMVectorAdd(vel_absolute_B, AtoB);
			XMStoreFloat3(&prop_B.velocity, vel_B);
			XMStoreFloat3(&prop_B.vel_absolute, vel_absolute_B);
		}
	}
	if (!(prop_A.intera_tp & PHY_INTERA_FIXED)) {
		if (prop_B.intera_tp == PHY_INTERA_FIXED_LIMIT) XMStoreFloat4x4(&world_A, w_A);
		else {
			vel_absolute_A = XMVectorSubtract(vel_absolute_A, AtoB);
			XMStoreFloat3(&prop_A.velocity, vel_A);
			XMStoreFloat3(&prop_A.vel_absolute, vel_absolute_A);
		}
	}
	assert(!XMVector4IsNaN(vel_A));
	assert(!XMVector4IsNaN(vel_B));
	return;
}
////////////////
// phy_position::attack_impulse
// A is attacker
////////////////
////////////////
template <typename T_app>
void phy_position<T_app>::attack_impulse(
	phy_property &prop_B,
	const XMFLOAT3 &center_Hit,
	const XMFLOAT3 &center_B,
	const XMFLOAT3 &center_A,
	const bool &is_touch,
	const float &impulse_scale,
	const size_t &ix_dmg)
{
	if (!is_touch) return;
	if (prop_B.intera_tp & PHY_INTERA_FIXED) return;
	if (app->m_Inst.m_Troll[ix_dmg].current_state == pose_FallDown::instance()) return;
	XMVECTOR c_Hit = XMLoadFloat3(&center_Hit);
	XMVECTOR c_B = XMLoadFloat3(&center_B);
	XMVECTOR c_A = XMLoadFloat3(&center_A);
	//
	XMVECTOR Hit_to_B = XMVectorSubtract(c_B, c_A);
	float too_far = 8.0f;
	if (XMVectorGetX(XMVector3Length(Hit_to_B)) > too_far) {
		Hit_to_B = XMVectorSubtract(c_B, c_Hit);
	}
	else {
		Hit_to_B = XMVectorSubtract(c_B, c_A);
		Hit_to_B = XMVectorSetY(Hit_to_B, 0.0f);
	}
	Hit_to_B = XMVector3Normalize(Hit_to_B);
	XMVECTOR vel_absolute = XMLoadFloat3(&prop_B.vel_absolute);
	// aimlessly
	float impulse = 3.0f;
	float impulse_step = 3.0f;
	float coef = 20.0f;
	if (impulse_scale < impulse_step) impulse = impulse_scale;
	Hit_to_B = XMVectorScale(Hit_to_B, coef * impulse);
	vel_absolute = XMVectorAdd(vel_absolute, Hit_to_B);
	XMStoreFloat3(&prop_B.vel_absolute, vel_absolute);
	if (impulse_scale > impulse_step) {
		prop_B.absolute_alt = static_cast<int>(impulse_scale);
	}
	return;
}
//
}
#endif