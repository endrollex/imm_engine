////////////////
// control_math.h
// by Huang Yiting
////////////////
////////////////
#ifndef CONTROL_MATH_H
#define CONTROL_MATH_H
#include <DirectXMath.h>
using namespace DirectX;
#include "control_state.h"
#include <string>
namespace imm {namespace math2
{
////////////////
// static const
////////////////
////////////////
static const XMVECTORF32 VECTOR_AXIS_X = {1.0f, 0.0f, 0.0f, 0.0f};
static const XMVECTORF32 VECTOR_AXIS_Y = {0.0f, 1.0f, 0.0f, 0.0f};
static const XMVECTORF32 VECTOR_AXIS_Z = {0.0f, 0.0f, 1.0f, 0.0f};
static const XMVECTORF32 VECTOR_FRONT_FACING = {0.0f, 0.0f, -1.0f, 0.0f};
static const XMFLOAT3 VECTOR3_NULL = {0.0f, 10.0f, 0.0f};
////////////////
// functions
////////////////
////////////////
void mouse_inst_move(const size_t &index, const float &speed);
void mouse_move_toward_hit(
	CXMVECTOR &hit_pos,
	const size_t &index,
	const float &speed);
void mouse_move_toward_hit_ai_avoid(
	const bool &is_avoid_force,
	const size_t &index,
	const float &speed,
	const bool &is_rot);
void mouse_face_rot_y(
	XMMATRIX &W,
	XMMATRIX &RF,
	CXMVECTOR &direction);
void ai_face_rot_y(
	XMMATRIX &W,
	XMMATRIX &RF,
	const float &rot_radians);
void pad_face_rot_y(
	XMMATRIX &W,
	XMMATRIX &RF,
	XMVECTOR &direction,
	const float &rot_cam,
	const bool &is_keyboard);
void mouse_hit_plane_y(XMVECTOR &hit_pos_out);
void mouse_hit_terrain(XMVECTOR &hit_pos_out);
void pad_move_toward(const size_t &index, const float &speed);
bool key_move_wasd(const size_t &index, const float &speed);
void set_inst_speed(const size_t &index, const float &speed, const bool &is_reset_list);
void set_inst_face_to_inst2(const size_t &index, const size_t &index2);
void set_face_to_face(const size_t &index, const size_t &index2);
void ai_move_pos(const size_t &index, const float &speed);
void ai_move_pos_stop(const size_t &index);
//
}}
#endif