////////////////
// mesh_skinned_data.h
// library from [Introduction to 3D Game Programming with DirectX 11 by Frank Luna]
////////////////
////////////////
#ifndef MESH_SKINNED_DATA_H
#define MESH_SKINNED_DATA_H
#include "util_math.h"
namespace imm
{
////////////////
// keyframe
// A keyframe defines the bone transformation at an instant in time.
////////////////
////////////////
struct keyframe
{
	keyframe();
	~keyframe();
	float time_pos;
	XMFLOAT3 translation;
	XMFLOAT3 scale;
	XMFLOAT4 rotation_quat;
};
////////////////
// bone_animation
// A bone_animation is defined by a list of keyframes.  For time
// values inbetween two keyframes, we interpolate between the
// two nearest keyframes that bound the time.
// We assume an animation always has two keyframes.
////////////////
////////////////
struct bone_animation
{
	float get_start_time() const;
	float get_end_time() const;
	void interpolate(float t, XMFLOAT4X4 &M) const;
	std::vector<keyframe> keyframes;
};
////////////////
// animation_clip
////////////////
////////////////
struct animation_clip
{
	float get_clip_start_time() const;
	float get_clip_end_time() const;
	void interpolate(float t, std::vector<XMFLOAT4X4> &bone_transforms) const;
	std::vector<bone_animation> bone_animations;
};
////////////////
// skinned_data
////////////////
////////////////
class skinned_data
{
public:
	UINT m_BoneCount() const;
	float get_clip_start_time(const std::string &clip_name) const;
	float get_clip_end_time(const std::string &clip_name) const;
	void set(
		std::vector<int> &bone_hierarchy,
		std::vector<XMFLOAT4X4> &bone_offsets,
		std::map<std::string, animation_clip> &animations);
	// In a real project, you'd want to cache the result if there was a chance
	// that you were calling this several times with the same clip_name at
	// the same time_pos.
	void get_final_transforms(
		const std::string &clip_name, float time_pos, std::vector<XMFLOAT4X4> &final_transforms) const;
	bool check_clip_name(const std::string &clip_name);
	void create_clip_to_clip_anim(
		const std::string &clip_first,
		const std::string &clip_second,
		const size_t &last_frame);
	// Gives parent_index of ith bone.
	std::vector<int> m_BoneHierarchy;
	std::vector<XMFLOAT4X4> m_BoneOffsets;
	std::map<std::string, animation_clip> m_Animations;
};
//
}
#endif