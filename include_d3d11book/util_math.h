////////////////
// util_math.h
// some library from [Introduction to 3D Game Programming with DirectX 11 by Frank Luna]
////////////////
////////////////
#ifndef UTIL_MATH_H
#define UTIL_MATH_H
#include <cassert>
#include <windows.h>
#include <cfloat>
#include <cmath>
#include <cstdlib>
#include <fstream>
#include <vector>
#include <map>
#include <codecvt>
#include <string>
#include <d3d11.h>
#include "d3dx11effect.h"
#include <DirectXMath.h>
using namespace DirectX;
namespace imm
{
////////////////
// DEBUG, IUnknown
////////////////
////////////////
#if defined(DEBUG) | defined(_DEBUG)
	#ifndef HR
	#define HR(x) {HRESULT hr_get = (x); if (FAILED(hr_get))\
		{std::string hrs(std::to_string(hr_get)); MessageBoxA(0, hrs.c_str(), "HRESULT", MB_OK); assert(false); abort();}}
	#endif
#else
	#ifndef HR
	#define HR(x) (x)
	#endif
#endif
#define RELEASE_COM(x) {if (x) {x->Release(); x = nullptr;}}
#define SAFE_DELETE(x) {delete x; x = nullptr;}
// do nothing
#define DUMMY(x) (x)
// error messagebox
#define ERROR_MESA(x) {MessageBoxA(0, x, "ERROR", MB_OK); assert(false); abort();}
////////////////
// static
////////////////
////////////////
static std::map<std::string, std::string> IMM_PATH;
static const bool ALWAYS_TRUE = true;
static const size_t VECTOR_RESERVE = 1000;
// for calclate UI size with factor
static const float UI_RESOLUTION_WIDTH = 1366.0f;
static const float UI_RESOLUTION_HEIGHT = 768.0f;
static const float FLOAT_4_ZERO[] = {0.0f, 0.0f, 0.0f, 0.0f};
// frame rate, Blender's default setting is 24.0f
static const float FRAME_RATE = 24.0f;
static const float FRAME_RATE_1DIV = 1.0f/FRAME_RATE;
static const float FPS60 = 60.0f;
static const float FPS60_1DIV = 1.0f/60.0f;
static const float FPS_MIN_REQ_1DIV = 1.0f/20.0f;
static const float TIME_1_MINITE = 60.0f;
static const float TIME_59_SECONDS = 59.0F;
static const float GAME_HP_BAR = 20.0f;
static const float AI_DT_LOGIC = 0.05f;
static const float AI_DT_PHY_FAST = 0.05f;
static const float AI_DT_PHY_SLOW = 0.1f;
static const float AI_DT_PHY_2SLOW = 0.5f;
static const float ATK_IMPULSE_DMG2 = 5.0f;
static const float PHY_DELTA_TIME_MAX = 1.0f/15.0f;
static const std::wstring SCENE_FIRST = L"_00";
static const std::wstring SCENE_SECOND = L"_01";
static std::string EMPTY_STRING = "";
static bool IS_STANDALONE_M3DTOB3M = false;
////////////////
// timer
////////////////
////////////////
class timer
{
public:
	timer();
	float total_time() const;
	float delta_time() const {return (float)m_DeltaTime;}
	double delta_time_test() const;
	void reset();
	void start();
	void stop();
	void tick();
//private:
	double m_SecPerCount;
	double m_DeltaTime;
	__int64 m_BaseTime;
	__int64 m_PausedTime;
	__int64 m_StopTime;
	__int64 m_PrevTime;
	__int64 m_CurrTime;
	__int64 m_CurrTest;
	bool m_Stopped;
};
////////////////
// math
////////////////
////////////////
struct math
{
	// Returns random float in [0, 1).
	static float calc_randf();
	// Returns random float in [a, b).
	static float calc_randf(const float &a, const float &b);
	template <typename T>
	static T calc_min(const T &a, const T &b){return a < b ? a : b;}
	template <typename T>
	static T calc_max(const T &a, const T &b) {return a > b ? a : b;}
	template <typename T>
	static T calc_lerp(const T &a, const T &b, float t) {return a+(b-a)*t;}
	template <typename T>
	static T calc_clamp(const T &x, const T &low, const T &high) {return x < low ? low : (x > high ? high : x);}
	// Returns the polar angle of the point (x,y) in [0, 2*PI).
	static float angle_from_xy(const float &x, const float &y);
	static bool float_is_equal(const float &a, const float &b);
	static XMMATRIX inverse_transpose(CXMMATRIX m);
	static XMVECTOR rand_unit_vec3();
	static XMVECTOR rand_hemisphere_unit_vec3(XMVECTOR n);
};
////////////////
// light
// Note: Make sure structure alignment agrees with HLSL structure padding rules.
// Elements are packed into 4D vectors with the restriction that an element
// cannot straddle a 4D vector boundary.
////////////////
////////////////
struct light_dir
{
	light_dir() {ZeroMemory(this, sizeof(this));}
	XMFLOAT4 ambient;
	XMFLOAT4 diffuse;
	XMFLOAT4 specular;
	XMFLOAT3 direction;
	float pad; // Pad the last float so we can set an array of lights if we wanted.
};
//
struct light_point
{
	light_point() {ZeroMemory(this, sizeof(this));}
	XMFLOAT4 ambient;
	XMFLOAT4 diffuse;
	XMFLOAT4 specular;
	// Packed into 4D vector: (Position, Range)
	XMFLOAT3 position;
	float range;
	// Packed into 4D vector: (A0, A1, A2, Pad)
	XMFLOAT3 att;
	float pad; // Pad the last float so we can set an array of lights if we wanted.
};
//
struct light_spot
{
	light_spot() {ZeroMemory(this, sizeof(this));}
	XMFLOAT4 ambient;
	XMFLOAT4 diffuse;
	XMFLOAT4 specular;
	// Packed into 4D vector: (Position, Range)
	XMFLOAT3 position;
	float range;
	// Packed into 4D vector: (Direction, Spot)
	XMFLOAT3 direction;
	float spot;
	// Packed into 4D vector: (Att, Pad)
	XMFLOAT3 att;
	float pad; // Pad the last float so we can set an array of lights if we wanted.
};
//
struct material
{
	material() {ZeroMemory(this, sizeof(this));}
	XMFLOAT4 ambient;
	XMFLOAT4 diffuse;
	XMFLOAT4 specular; // w = SpecPower
	XMFLOAT4 reflect;
};
////////////////
// txtutil
////////////////
////////////////
struct txtutil
{
	static std::wstring str_to_wstr(const std::string &str);
	static std::string wstr_to_str(const std::wstring &wstr);
};
////////////////
// effect
////////////////
////////////////
class effect
{
public:
	effect(ID3D11Device *device, const std::wstring &filename);
	virtual ~effect() {RELEASE_COM(m_FX);}
private:
	effect(const effect &rhs);
	effect &operator=(const effect &rhs);
protected:
	ID3DX11Effect *m_FX;
};
//
}
#endif