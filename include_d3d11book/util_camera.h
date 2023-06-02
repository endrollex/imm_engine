////////////////
// util_camera.h
// library from [Introduction to 3D Game Programming with DirectX 11 by Frank Luna]
////////////////
////////////////
#ifndef UTIL_CAMERA_H
#define UTIL_CAMERA_H
#include "util_math.h"
namespace imm
{
////////////////
// camera
////////////////
////////////////
class camera
{
public:
	camera();
	~camera();
	// get/set world camera position.
	XMVECTOR get_PositionXM() const;
	XMFLOAT3 get_Position() const;
	void set_Position(float x, float y, float z);
	void set_Position(const XMFLOAT3& v);
	// get camera basis vectors.
	XMVECTOR get_RightXM() const;
	XMFLOAT3 get_Right() const;
	XMVECTOR get_UpXM() const;
	XMFLOAT3 get_Up() const;
	XMVECTOR get_LookXM() const;
	XMFLOAT3 get_Look() const;
	// get frustum properties.
	float get_NearZ() const;
	float get_FarZ() const;
	float get_Aspect() const;
	float get_FovY() const;
	float get_FovX() const;
	// get near and far plane dimensions in view space coordinates.
	float get_NearWindowWidth() const;
	float get_NearWindowHeight() const;
	float get_FarWindowWidth() const;
	float get_FarWindowHeight() const;
	// set frustum.
	void set_Lens(float fov_y, float aspect, float zn, float zf);
	// Define camera space via look_at parameters.
	void look_at(FXMVECTOR pos, FXMVECTOR target, FXMVECTOR world_up);
	void look_at(const XMFLOAT3& pos, const XMFLOAT3& target, const XMFLOAT3& up);
	// get View/Proj matrices.
	XMMATRIX get_View() const;
	XMMATRIX get_Proj() const;
	XMMATRIX get_ViewProj() const;
	// strafe/walk the camera a distance d.
	void strafe(float d);
	void walk(float d);
	void up_down(float d);
	// Rotate the camera.
	void pitch(float angle);
	void rotate_y(float angle);
	// After modifying camera position/orientation, call to rebuild the view matrix.
	void update_view_matrix();
	void reset(const int &preset);
//private:
	// Camera coordinate system with coordinates relative to world space.
	XMFLOAT3 m_Position;
	XMFLOAT3 m_Right;
	XMFLOAT3 m_Up;
	XMFLOAT3 m_Look;
	// Cache frustum properties.
	float m_NearZ;
	float m_FarZ;
	float m_Aspect;
	float m_FovY;
	float m_NearWindowHeight;
	float m_FarWindowHeight;
	int m_Preset;
	// Cache View/Proj matrices.
	XMFLOAT4X4 m_View;
	XMFLOAT4X4 m_Proj;
};
}
#endif