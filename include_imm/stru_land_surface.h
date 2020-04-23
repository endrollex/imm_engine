////////////////
// stru_land_surface.h
// by Huang Yiting
////////////////
////////////////
#ifndef STRU_LAND_SURFACE_H
#define STRU_LAND_SURFACE_H
#include "stru_load_help.h"
#include <DirectXMath.h>
#include <DirectXCollision.h>
namespace imm
{
////////////////
// land_surface
////////////////
////////////////
template <typename T_app>
struct land_surface
{
	land_surface();
	~land_surface();
	void init(T_app *app_in);
	void build_geometry_buffers();
	T_app *app;
	ID3D11Buffer* VB;
	ID3D11Buffer* IB;
	ID3D11Buffer* instanced_buffer;
	BoundingBox bbox;
	basic_model_instance basic;
};
//
template <typename T_app>
land_surface<T_app>::land_surface():
	app(nullptr),
	VB(nullptr),
	IB(nullptr),
	instanced_buffer(nullptr),
	bbox(),
	basic()
{
	;
}
//
template <typename T_app>
land_surface<T_app>::~land_surface()
{
	RELEASE_COM(VB);
	RELEASE_COM(IB);
	RELEASE_COM(instanced_buffer);
}
//
template <typename T_app>
void land_surface<T_app>::init(T_app *app_in)
{
	app = app_in;
}
//
template <typename T_app>
void land_surface<T_app>::build_geometry_buffers()
{
	//implement in the furture if need land surface
	;
}
//
}
#endif