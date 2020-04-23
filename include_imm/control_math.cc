////////////////
// control_math.cc
// by Huang Yiting
////////////////
////////////////
#include "control_math.h"
#include "imm_ptr.h"
namespace imm {namespace math2
{
//
void mouse_inst_move(const size_t &index, const float &speed)
{
	if (!PTR->m_Inst.m_Stat[index].phy.is_on_land) return;
	XMVECTOR hit_pos;
	if (PTR->m_Inst.m_IsTerrainUse) mouse_hit_terrain(hit_pos);
	else mouse_hit_plane_y(hit_pos);
	mouse_move_toward_hit(hit_pos, index, speed);
	PTR->m_Control.map_stop[index].set_destination(speed, hit_pos, PTR->m_Inst.m_BoundW.extents_y(index));
}
//
void mouse_move_toward_hit(
	CXMVECTOR &hit_pos,
	const size_t &index,
	const float &speed)
{
	XMFLOAT4X4 &world = *PTR->m_Inst.m_Stat[index].get_World();
	XMFLOAT4X4 &rot_front = *PTR->m_Inst.m_Stat[index].get_RotFront();
	XMFLOAT3 *phy_vel_indirect = &PTR->m_Inst.m_Stat[index].phy.vel_indirect;
	XMVECTOR vel_indirect = XMLoadFloat3(phy_vel_indirect);
	XMMATRIX W = XMLoadFloat4x4(&world);
	XMMATRIX RF = XMLoadFloat4x4(&rot_front);
	vel_indirect = XMVectorSubtract(hit_pos, W.r[3]);
	vel_indirect = XMVectorSetY(vel_indirect, 0.0f);
	vel_indirect = XMVector3Normalize(vel_indirect);
	// use vel_indirect as front direction
	mouse_face_rot_y(W, RF, vel_indirect);
	// when update stop, use object own speed
	vel_indirect = XMVectorScale(vel_indirect, speed);
	XMStoreFloat3(phy_vel_indirect, vel_indirect);
	XMStoreFloat4x4(&world, W);
}
//
void mouse_move_toward_hit_ai_avoid(
	const bool &is_avoid_force,
	const size_t &index,
	const float &speed,
	const bool &is_rot = false)
{
	XMFLOAT4X4 &world = *PTR->m_Inst.m_Stat[index].get_World();
	XMFLOAT4X4 &rot_front = *PTR->m_Inst.m_Stat[index].get_RotFront();
	XMFLOAT3 *phy_vel_indirect = &PTR->m_Inst.m_Stat[index].phy.vel_indirect;
	XMVECTOR vel_indirect = XMLoadFloat3(phy_vel_indirect);
	XMMATRIX W = XMLoadFloat4x4(&world);
	XMMATRIX RF = XMLoadFloat4x4(&rot_front);
	vel_indirect = XMVectorSetY(vel_indirect, 0.0f);
	vel_indirect = XMVector3Normalize(vel_indirect);
	vel_indirect = XMVectorScale(vel_indirect, speed);
	if (is_avoid_force) {
		XMVECTOR avoidance = XMLoadFloat3(&PTR->m_Control.map_stop[index].avoidance);
		vel_indirect = XMVectorAdd(vel_indirect, avoidance);
		vel_indirect = XMVector3Normalize(vel_indirect);
		vel_indirect = XMVectorScale(vel_indirect, speed);
	}
	//
	if (is_rot) {
		int irand = rand()%2;
		if (irand) ai_face_rot_y(W, RF, 90.0f);
		else ai_face_rot_y(W, RF, -90.0f);
	}
	//
	XMVECTOR vel_indirect_normal = XMVector3Normalize(vel_indirect);
	// use vel_indirect as front direction
	mouse_face_rot_y(W, RF, vel_indirect_normal);
	XMStoreFloat3(phy_vel_indirect, vel_indirect);
	XMStoreFloat4x4(&world, W);
}
//
void mouse_face_rot_y(
	XMMATRIX &W,
	XMMATRIX &RF,
	CXMVECTOR &direction)
{
	// Decompose the matrix into its individual parts.
	XMVECTOR scale, rot_quat, translation;
	XMMatrixDecompose(&scale, &rot_quat, &translation, W);
	XMMATRIX S, R, T;
	S = XMMatrixScalingFromVector(scale);
	float rot_radians = atan2(-XMVectorGetX(direction), -XMVectorGetZ(direction));
	R = XMMatrixRotationY(rot_radians);
	//
	R = XMMatrixMultiply(RF, R);
	T = XMMatrixTranslationFromVector(translation);
	W = XMMatrixMultiply(XMMatrixMultiply(S, R), T);
}
//
void ai_face_rot_y(
	XMMATRIX &W,
	XMMATRIX &RF,
	const float &rot_radians)
{
	// Decompose the matrix into its individual parts.
	XMVECTOR scale, rot_quat, translation;
	XMMatrixDecompose(&scale, &rot_quat, &translation, W);
	XMMATRIX S, R, T;
	S = XMMatrixScalingFromVector(scale);
	R = XMMatrixRotationY(rot_radians);
	//
	R = XMMatrixMultiply(RF, R);
	T = XMMatrixTranslationFromVector(translation);
	W = XMMatrixMultiply(XMMatrixMultiply(S, R), T);
}
//
void pad_face_rot_y(
	XMMATRIX &W,
	XMMATRIX &RF,
	XMVECTOR &direction,
	const float &rot_cam,
	const bool &is_keyboard = false)
{
	// Decompose the matrix into its individual parts.
	XMVECTOR scale, rot_quat, translation;
	XMMatrixDecompose(&scale, &rot_quat, &translation, W);
	XMMATRIX S, R, T;
	S = XMMatrixScalingFromVector(scale);
	R = XMMatrixRotationY(rot_cam);
	if (!is_keyboard) {
		// rot_quat uses as temp var
		float rot_pad = PTR->m_Control.pad.L_radians();
		rot_quat =  XMQuaternionRotationAxis(VECTOR_AXIS_Y, rot_pad);
		direction = XMVector3Rotate(direction, rot_quat);
		R = XMMatrixMultiply(XMMatrixRotationY(rot_pad), R);
	}
	R = XMMatrixMultiply(RF, R);
	T = XMMatrixTranslationFromVector(translation);
	W = XMMatrixMultiply(XMMatrixMultiply(S, R), T);
}
//
void mouse_hit_plane_y(XMVECTOR &hit_pos_out)
{
	CXMMATRIX cam_proj = PTR->m_Cam.get_Proj();
	CXMMATRIX cam_view = PTR->m_Cam.get_View();
	// Compute picking ray in view space.
	int mouse_x = PTR->m_Control.mouse_down.x;
	int mouse_y = PTR->m_Control.mouse_down.y;
	float vx = (+2.0f*mouse_x/PTR->m_ClientWidth - 1.0f)/cam_proj.r[0].m128_f32[0];
	float vy = (-2.0f*mouse_y/PTR->m_ClientHeight + 1.0f)/cam_proj.r[1].m128_f32[1];
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
	// avoid ray_dir.y == 0
	while (abs(XMVectorGetY(ray_dir)) < 0.001f) {
		ray_dir = XMVectorSetY(ray_dir, XMVectorGetY(ray_dir)-0.01f);
		ray_dir = XMVector3Normalize(ray_dir);
	}	
	// Ray hit y=value plane at plane_p
	XMFLOAT4X4 *world = PTR->m_Inst.m_Stat[PTR->m_Inst.m_PlaneLandIx].get_World();
	float plane_y = world->_42;
	hit_pos_out = ray_origin;
	float ratio_y = -XMVectorGetY(ray_origin)/XMVectorGetY(ray_dir)+(plane_y/XMVectorGetY(ray_dir));
	hit_pos_out = XMVectorSetY(hit_pos_out, plane_y);
	hit_pos_out = XMVectorSetX(hit_pos_out, XMVectorGetX(ray_origin)+ratio_y*XMVectorGetX(ray_dir));
	hit_pos_out = XMVectorSetZ(hit_pos_out, XMVectorGetZ(ray_origin)+ratio_y*XMVectorGetZ(ray_dir));
}
//
void mouse_hit_terrain(XMVECTOR &hit_pos_out)
{
	CXMMATRIX cam_proj = PTR->m_Cam.get_Proj();
	CXMMATRIX cam_view = PTR->m_Cam.get_View();
	// Compute picking ray in view space.
	int mouse_x = PTR->m_Control.mouse_down.x;
	int mouse_y = PTR->m_Control.mouse_down.y;
	float vx = (+2.0f*mouse_x/PTR->m_ClientWidth - 1.0f)/cam_proj.r[0].m128_f32[0];
	float vy = (-2.0f*mouse_y/PTR->m_ClientHeight + 1.0f)/cam_proj.r[1].m128_f32[1];
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
	// Avoid ray_dir.y == 0
	while (abs(XMVectorGetY(ray_dir)) < 0.001f) {
		ray_dir = XMVectorSetY(ray_dir, XMVectorGetY(ray_dir)-0.01f);
		ray_dir = XMVector3Normalize(ray_dir);
	}
	// Ray hit y=value plane at plane_p
	hit_pos_out = ray_origin;
	size_t steps = 50;
	float terrain_height = PTR->m_Scene.terrain1.m_Info.height_scale;
	float terrain_half_y = terrain_height*0.5f;
	float plane_y = terrain_half_y;
	float delt_y = terrain_height / steps;
	float comp_y = delt_y;
	float less_y = FLT_MAX;
	float less_y2 = FLT_MAX;
	XMVECTOR near_pos = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	//
	for (size_t ix = 0; ix != steps+2; ++ix) {
		float ratio_y = -XMVectorGetY(ray_origin)/XMVectorGetY(ray_dir)+(plane_y/XMVectorGetY(ray_dir));
		hit_pos_out = XMVectorSetY(hit_pos_out, plane_y);
		hit_pos_out = XMVectorSetX(hit_pos_out, XMVectorGetX(ray_origin)+ratio_y*XMVectorGetX(ray_dir));
		hit_pos_out = XMVectorSetZ(hit_pos_out, XMVectorGetZ(ray_origin)+ratio_y*XMVectorGetZ(ray_dir));
		assert(!isnan(XMVectorGetX(hit_pos_out)));
		assert(!isnan(XMVectorGetZ(hit_pos_out)));
		float height = PTR->m_Scene.terrain1.get_Height(XMVectorGetX(hit_pos_out), XMVectorGetZ(hit_pos_out));
		ContainmentType contain = PTR->m_Inst.m_CamFrustumW.Contains(hit_pos_out);
		bool in_frustum = (contain != 0);
		float diff_y = abs(plane_y-height);
		// Check height valid and frustum contains
		if (height > -terrain_height && in_frustum) {
			if (diff_y < comp_y) {
				return;
			}
			if (diff_y < less_y) {
				less_y = diff_y;
				hit_pos_out = XMVectorSetY(hit_pos_out, height);
				near_pos = hit_pos_out;
			}
		}
		// When can not found a point in frustum
		if (height > -terrain_height && !in_frustum) {
			if (diff_y < less_y2 && less_y >= terrain_height) {
				less_y2 = diff_y;
				hit_pos_out = XMVectorSetY(hit_pos_out, height);
				near_pos = hit_pos_out;
			}
		}
		if (ix%2 == 0) plane_y = terrain_half_y + delt_y*(ix/2);
		else plane_y = terrain_half_y - delt_y*((ix+1)/2);
	}
	hit_pos_out = near_pos;
	//
	if (!(less_y < terrain_height || less_y2 < terrain_height)) {
#if defined(DEBUG) | defined(_DEBUG)
		ERROR_MESA("error: control_math.cc: mouse_hit_terrain()");
#endif
	}
}
//
void pad_move_toward(const size_t &index, const float &speed)
{
	XMVECTOR vel_indirect = PTR->m_Cam.get_LookXM();
	float cam_radians = atan2(-XMVectorGetX(vel_indirect), -XMVectorGetZ(vel_indirect));
	XMFLOAT4X4 &world = *(PTR->m_Inst.m_Stat[index].get_World());
	XMFLOAT4X4 &rot_front = *(PTR->m_Inst.m_Stat[index].get_RotFront());
	XMMATRIX W = XMLoadFloat4x4(&world);
	XMMATRIX RF = XMLoadFloat4x4(&rot_front);
	pad_face_rot_y(W, RF, vel_indirect, cam_radians);
	vel_indirect = XMVectorSetY(vel_indirect, 0.0f);
	vel_indirect = XMVector3Normalize(vel_indirect);
	vel_indirect = XMVectorScale(vel_indirect, speed);
	XMStoreFloat4x4(&world, W);
	XMStoreFloat3(&PTR->m_Inst.m_Stat[index].phy.vel_indirect, vel_indirect);
}
//
bool key_move_wasd(const size_t &index, const float &speed)
{
	float offset_radians = 0.0f;
	if (!(GetKeyState(KEY_P1_W) & 0x8000) &&
		!(GetKeyState(KEY_P1_A) & 0x8000) &&
		!(GetKeyState(KEY_P1_S) & 0x8000) &&
		!(GetKeyState(KEY_P1_D) & 0x8000)) {
		PTR->m_Inst.m_Stat[index].phy.vel_indirect = XMFLOAT3(0.0f, 0.0f, 0.0f);
		return false;
	}
	//
	if (GetKeyState(KEY_P1_A) & 0x8000) offset_radians = -XM_PI*0.5f;
	if (GetKeyState(KEY_P1_S) & 0x8000) offset_radians = XM_PI;
	if (GetKeyState(KEY_P1_D) & 0x8000) offset_radians = XM_PI*0.5f;
	if ((GetKeyState(KEY_P1_W) & 0x8000) && (GetKeyState(KEY_P1_A) & 0x8000)) offset_radians = -XM_PI*0.25f;
	if ((GetKeyState(KEY_P1_W) & 0x8000) && (GetKeyState(KEY_P1_D) & 0x8000)) offset_radians = XM_PI*0.25f;
	if ((GetKeyState(KEY_P1_S) & 0x8000) && (GetKeyState(KEY_P1_A) & 0x8000)) offset_radians = -XM_PI*0.75f;
	if ((GetKeyState(KEY_P1_S) & 0x8000) && (GetKeyState(KEY_P1_D) & 0x8000)) offset_radians = XM_PI*0.75f;
	//
	XMVECTOR vel_indirect = PTR->m_Cam.get_LookXM();
	XMVECTOR offset_rotation = XMQuaternionRotationAxis(VECTOR_AXIS_Y, offset_radians);
	vel_indirect = XMVector3Rotate(vel_indirect, offset_rotation);
	float cam_radians = atan2(-XMVectorGetX(vel_indirect), -XMVectorGetZ(vel_indirect));
	//
	XMFLOAT4X4 &world = *(PTR->m_Inst.m_Stat[index].get_World());
	XMFLOAT4X4 &rot_front = *(PTR->m_Inst.m_Stat[index].get_RotFront());
	XMMATRIX W = XMLoadFloat4x4(&world);
	XMMATRIX RF = XMLoadFloat4x4(&rot_front);
	pad_face_rot_y(W, RF, vel_indirect, cam_radians, true);
	vel_indirect = XMVectorSetY(vel_indirect, 0.0f);
	vel_indirect = XMVector3Normalize(vel_indirect);
	vel_indirect = XMVectorScale(vel_indirect, speed);
	XMStoreFloat4x4(&world, W);
	XMStoreFloat3(&PTR->m_Inst.m_Stat[index].phy.vel_indirect, vel_indirect);
	return true;
}
//
void set_inst_speed(const size_t &index, const float &speed, const bool &is_reset_list = true)
{
	if (is_reset_list) {
		PTR->m_Control.map_speed[index].reset();
	}
	//
	XMFLOAT4X4 &world = *PTR->m_Inst.m_Stat[index].get_World();
	XMFLOAT4X4 &rot_front = *PTR->m_Inst.m_Stat[index].get_RotFront();
	XMMATRIX W = XMLoadFloat4x4(&world);
	XMMATRIX RF = XMLoadFloat4x4(&rot_front);
	// Decompose the matrix into its individual parts.
	XMVECTOR scale, rot_quat, translation;
	XMMatrixDecompose(&scale, &rot_quat, &translation, W);
	XMMATRIX WR = XMMatrixRotationQuaternion(rot_quat);
	XMVECTOR det_RF = XMMatrixDeterminant(RF);
	XMMATRIX R = XMMatrixMultiply(XMMatrixInverse(&det_RF, RF), WR);
	XMVECTOR vel_indirect = XMVector3Rotate(VECTOR_FRONT_FACING, XMQuaternionRotationMatrix(R));
	//
	vel_indirect = XMVector3Normalize(vel_indirect);
	vel_indirect = XMVectorScale(vel_indirect, speed);
	XMStoreFloat3(&PTR->m_Inst.m_Stat[index].phy.vel_indirect, vel_indirect);
}
//
void set_inst_face_to_inst2(const size_t &index, const size_t &index2)
{
	XMFLOAT4X4 &world = *PTR->m_Inst.m_Stat[index].get_World();
	XMFLOAT4X4 &world2 = *PTR->m_Inst.m_Stat[index2].get_World();
	XMFLOAT4X4 &rot_front = *PTR->m_Inst.m_Stat[index].get_RotFront();
	XMMATRIX W = XMLoadFloat4x4(&world);
	XMMATRIX W2 = XMLoadFloat4x4(&world2);
	XMMATRIX RF = XMLoadFloat4x4(&rot_front);
	XMVECTOR direction = XMVectorSubtract(W2.r[3], W.r[3]);
	direction = XMVectorSetY(direction, 0.0f);
	direction = XMVector3Normalize(direction);
	mouse_face_rot_y(W, RF, direction);
	XMStoreFloat4x4(&world, W);
}
//
void set_face_to_face(const size_t &index, const size_t &index2)
{
	XMFLOAT4X4 &world = *PTR->m_Inst.m_Stat[index].get_World();
	XMFLOAT4X4 &world2 = *PTR->m_Inst.m_Stat[index2].get_World();
	XMFLOAT4X4 &rot_front = *PTR->m_Inst.m_Stat[index].get_RotFront();
	XMMATRIX W = XMLoadFloat4x4(&world);
	XMMATRIX W2 = XMLoadFloat4x4(&world2);
	XMMATRIX RF = XMLoadFloat4x4(&rot_front);
	XMVECTOR direction = XMVectorSubtract(W2.r[3], W.r[3]);
	direction = XMVectorSetY(direction, 0.0f);
	direction = XMVector3Normalize(direction);
	mouse_face_rot_y(W, RF, direction);
	XMStoreFloat4x4(&world, W);
	//
	if (PTR->m_Inst.m_Stat[index2].property & INST_IS_CONTROLLABLE) {
		XMFLOAT4X4 &rot_front2 = *PTR->m_Inst.m_Stat[index2].get_RotFront();
		XMMATRIX RF2 = XMLoadFloat4x4(&rot_front2);
		XMVECTOR direction2 = XMVectorSubtract(W.r[3], W2.r[3]);
		direction2 = XMVectorSetY(direction2, 0.0f);
		direction2 = XMVector3Normalize(direction2);
		mouse_face_rot_y(W2, RF2, direction2);
		XMStoreFloat4x4(&world2, W2);
	}
}
//
void ai_move_pos(const size_t &index, const float &speed)
{
	XMFLOAT3 *pos = &PTR->m_Inst.m_Steering[index].desired_pos;
	if (PTR->m_Inst.m_IsTerrainUse) {
		pos->y = PTR->m_Scene.terrain1.get_Height(pos->x, pos->z);
	}
	else {
		pos->y = PTR->m_Inst.m_Stat[PTR->m_Inst.m_PlaneLandIx].get_World()->_42;
	}
	XMVECTOR hit_pos = XMLoadFloat3(pos);
	mouse_move_toward_hit(hit_pos, index, speed);
	PTR->m_Control.map_stop[index].set_destination(speed, hit_pos, PTR->m_Inst.m_BoundW.extents_y(index));
}
//
void ai_move_pos_stop(const size_t &index)
{
	PTR->m_Control.map_stop[index].interrupt(PTR, index);
}
//
}}