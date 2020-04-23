////////////////
// pipe_effect.h
// library from [Introduction to 3D Game Programming with DirectX 11 by Frank Luna]
////////////////
////////////////
#ifndef PIPE_EFFECT_H
#define PIPE_EFFECT_H
#include "util_math.h"
namespace imm
{
////////////////
// basic_effect
////////////////
////////////////
class basic_effect: public effect
{
public:
	basic_effect(ID3D11Device *device, const std::wstring &filename);
	~basic_effect()                                    {;}
	void set_WorldViewProj(CXMMATRIX m)                {m_WorldViewProj->SetMatrix(reinterpret_cast<const float*>(&m));}
	void set_WorldViewProjTex(CXMMATRIX M)             {m_WorldViewProjTex->SetMatrix(reinterpret_cast<const float*>(&M));}
	void set_World(CXMMATRIX m)                        {m_World->SetMatrix(reinterpret_cast<const float*>(&m));}
	void set_WorldInvTranspose(CXMMATRIX m)            {m_WorldInvTranspose->SetMatrix(reinterpret_cast<const float*>(&m));}
	void set_ShadowTransform(CXMMATRIX M)              {m_ShadowTransform->SetMatrix(reinterpret_cast<const float*>(&M));}
	void set_TexTransform(CXMMATRIX m)                 {m_TexTransform->SetMatrix(reinterpret_cast<const float*>(&m));}
	void set_EyePosW(const XMFLOAT3 &v)                {m_EyePosW->SetRawValue(&v, 0, sizeof(XMFLOAT3));}
	void set_DirLights(const light_dir *lights)        {m_DirLights->SetRawValue(lights, 0, 3*sizeof(light_dir));}
	void set_Material(const material &mat)             {m_Mat->SetRawValue(&mat, 0, sizeof(material));}
	void set_DiffuseMap(ID3D11ShaderResourceView *tex) {m_DiffuseMap->SetResource(tex);}
	void set_CubeMap(ID3D11ShaderResourceView *tex)    {m_CubeMap->SetResource(tex);}
	void set_ShadowMap(ID3D11ShaderResourceView* tex)  {m_ShadowMap->SetResource(tex);}
	ID3DX11EffectTechnique *m_Light3Tech;
	ID3DX11EffectTechnique *m_Light3TexTech;
	ID3DX11EffectTechnique *m_Light3TexAlphaClipTech;
	ID3DX11EffectMatrixVariable *m_WorldViewProj;
	ID3DX11EffectMatrixVariable *m_WorldViewProjTex;
	ID3DX11EffectMatrixVariable *m_World;
	ID3DX11EffectMatrixVariable *m_WorldInvTranspose;
	ID3DX11EffectMatrixVariable *m_ShadowTransform;
	ID3DX11EffectMatrixVariable *m_TexTransform;
	ID3DX11EffectVectorVariable *m_EyePosW;
	ID3DX11EffectVariable *m_DirLights;
	ID3DX11EffectVariable *m_Mat;
	ID3DX11EffectShaderResourceVariable *m_DiffuseMap;
	ID3DX11EffectShaderResourceVariable *m_CubeMap;
	ID3DX11EffectShaderResourceVariable *m_ShadowMap;
};
////////////////
// normal_map_effect
////////////////
////////////////
class normal_map_effect: public effect
{
public:
	normal_map_effect(ID3D11Device *device, const std::wstring &filename);
	~normal_map_effect()                                  {;}
	void set_WorldViewProj(CXMMATRIX M)                   {m_WorldViewProj->SetMatrix(reinterpret_cast<const float*>(&M));}
	void set_WorldViewProjTex(CXMMATRIX M)                {m_WorldViewProjTex->SetMatrix(reinterpret_cast<const float*>(&M));}
	void set_World(CXMMATRIX M)                           {m_World->SetMatrix(reinterpret_cast<const float*>(&M));}
	void set_WorldInvTranspose(CXMMATRIX M)               {m_WorldInvTranspose->SetMatrix(reinterpret_cast<const float*>(&M));}
	void set_BoneTransforms(const XMFLOAT4X4 *M, int cnt) {m_BoneTransforms->SetMatrixArray(reinterpret_cast<const float*>(M), 0, cnt);}
	void set_ShadowTransform(CXMMATRIX M)                 {m_ShadowTransform->SetMatrix(reinterpret_cast<const float*>(&M));}
	void set_TexTransform(CXMMATRIX M)                    {m_TexTransform->SetMatrix(reinterpret_cast<const float*>(&M));}
	void set_EyePosW(const XMFLOAT3 &v)                   {m_EyePosW->SetRawValue(&v, 0, sizeof(XMFLOAT3));}
	void set_DirLights(const light_dir *lights)           {m_DirLights->SetRawValue(lights, 0, 3*sizeof(light_dir));}
	void set_Material(const material &mat)                {m_Mat->SetRawValue(&mat, 0, sizeof(material));}
	void set_DiffuseMap(ID3D11ShaderResourceView *tex)    {m_DiffuseMap->SetResource(tex);}
	void set_CubeMap(ID3D11ShaderResourceView *tex)       {m_CubeMap->SetResource(tex);}
	void set_NormalMap(ID3D11ShaderResourceView *tex)     {m_NormalMap->SetResource(tex);}
	void set_ShadowMap(ID3D11ShaderResourceView* tex)     {m_ShadowMap->SetResource(tex);}
	void set_SsaoMap(ID3D11ShaderResourceView* tex)       {m_SsaoMap->SetResource(tex);}
	ID3DX11EffectTechnique *m_Light3Tech;
	ID3DX11EffectTechnique *m_Light3TexTech;
	ID3DX11EffectTechnique *m_Light3TexAlphaClipTech;
	ID3DX11EffectTechnique *m_Light3TexSkinnedTech;
	ID3DX11EffectTechnique *m_Light3TexAlphaClipSkinnedTech;
	ID3DX11EffectMatrixVariable *m_WorldViewProj;
	ID3DX11EffectMatrixVariable *m_WorldViewProjTex;
	ID3DX11EffectMatrixVariable *m_World;
	ID3DX11EffectMatrixVariable *m_WorldInvTranspose;
	ID3DX11EffectMatrixVariable *m_BoneTransforms;
	ID3DX11EffectMatrixVariable *m_ShadowTransform;
	ID3DX11EffectMatrixVariable *m_TexTransform;
	ID3DX11EffectVectorVariable *m_EyePosW;
	ID3DX11EffectVariable *m_DirLights;
	ID3DX11EffectVariable *m_Mat;
	ID3DX11EffectShaderResourceVariable *m_DiffuseMap;
	ID3DX11EffectShaderResourceVariable *m_CubeMap;
	ID3DX11EffectShaderResourceVariable *m_NormalMap;
	ID3DX11EffectShaderResourceVariable *m_ShadowMap;
	ID3DX11EffectShaderResourceVariable *m_SsaoMap;
};
////////////////
// build_shadow_map_effect
////////////////
////////////////
class build_shadow_map_effect: public effect
{
public:
	build_shadow_map_effect(ID3D11Device *device, const std::wstring &filename);
	~build_shadow_map_effect()                            {;}
	void set_ViewProj(CXMMATRIX M)                        {m_ViewProj->SetMatrix(reinterpret_cast<const float*>(&M));}
	void set_WorldViewProj(CXMMATRIX M)                   {m_WorldViewProj->SetMatrix(reinterpret_cast<const float*>(&M));}
	void set_World(CXMMATRIX M)                           {m_World->SetMatrix(reinterpret_cast<const float*>(&M));}
	void set_WorldInvTranspose(CXMMATRIX M)               {m_WorldInvTranspose->SetMatrix(reinterpret_cast<const float*>(&M));}
	void set_BoneTransforms(const XMFLOAT4X4 *M, int cnt) {m_BoneTransforms->SetMatrixArray(reinterpret_cast<const float*>(M), 0, cnt);}
	void set_TexTransform(CXMMATRIX M)                    {m_TexTransform->SetMatrix(reinterpret_cast<const float*>(&M));}
	void set_EyePosW(const XMFLOAT3 &v)                   {m_EyePosW->SetRawValue(&v, 0, sizeof(XMFLOAT3));}
	void set_HeightScale(float f)                         {m_HeightScale->SetFloat(f);}
	void set_MaxTessDistance(float f)                     {m_MaxTessDistance->SetFloat(f);}
	void set_MinTessDistance(float f)                     {m_MinTessDistance->SetFloat(f);}
	void set_MinTessFactor(float f)                       {m_MinTessFactor->SetFloat(f);}
	void set_MaxTessFactor(float f)                       {m_MaxTessFactor->SetFloat(f);}
	void set_DiffuseMap(ID3D11ShaderResourceView *tex)    {m_DiffuseMap->SetResource(tex);}
	void set_NormalMap(ID3D11ShaderResourceView *tex)     {m_NormalMap->SetResource(tex);}
	ID3DX11EffectTechnique *m_BuildShadowMapTech;
	ID3DX11EffectTechnique *m_BuildShadowMapAlphaClipTech;
	ID3DX11EffectTechnique *m_BuildShadowMapSkinnedTech;
	ID3DX11EffectTechnique *m_BuildShadowMapAlphaClipSkinnedTech;
	ID3DX11EffectTechnique *m_TessBuildShadowMapTech;
	ID3DX11EffectTechnique *m_TessBuildShadowMapAlphaClipTech;
	ID3DX11EffectMatrixVariable *m_ViewProj;
	ID3DX11EffectMatrixVariable *m_WorldViewProj;
	ID3DX11EffectMatrixVariable *m_World;
	ID3DX11EffectMatrixVariable *m_WorldInvTranspose;
	ID3DX11EffectMatrixVariable *m_BoneTransforms;
	ID3DX11EffectMatrixVariable *m_TexTransform;
	ID3DX11EffectVectorVariable *m_EyePosW;
	ID3DX11EffectScalarVariable *m_HeightScale;
	ID3DX11EffectScalarVariable *m_MaxTessDistance;
	ID3DX11EffectScalarVariable *m_MinTessDistance;
	ID3DX11EffectScalarVariable *m_MinTessFactor;
	ID3DX11EffectScalarVariable *m_MaxTessFactor;
	ID3DX11EffectShaderResourceVariable *m_DiffuseMap;
	ID3DX11EffectShaderResourceVariable *m_NormalMap;
};
////////////////
// sky_effect
////////////////
////////////////
class sky_effect: public effect
{
public:
	sky_effect(ID3D11Device *device, const std::wstring &filename);
	~sky_effect()                                       {;}
	void set_WorldViewProj(CXMMATRIX M)                 {m_WorldViewProj->SetMatrix(reinterpret_cast<const float*>(&M));}
	void set_CubeMap(ID3D11ShaderResourceView *cubemap) {m_CubeMap->SetResource(cubemap);}
	ID3DX11EffectTechnique *m_SkyTech;
	ID3DX11EffectMatrixVariable *m_WorldViewProj;
	ID3DX11EffectShaderResourceVariable *m_CubeMap;
};
////////////////
// terrain_effect
////////////////
////////////////
class terrain_effect: public effect
{
public:
	terrain_effect(ID3D11Device *device, const std::wstring &filename);
	~terrain_effect()                                     {;}
	void set_ViewProj(CXMMATRIX M)                        {m_ViewProj->SetMatrix(reinterpret_cast<const float*>(&M));}
	void set_ShadowTransform(CXMMATRIX M)                 {m_ShadowTransform->SetMatrix(reinterpret_cast<const float*>(&M));}
	void set_EyePosW(const XMFLOAT3 &v)                   {m_EyePosW->SetRawValue(&v, 0, sizeof(XMFLOAT3));}
	void set_DirLights(const light_dir *lights)           {m_DirLights->SetRawValue(lights, 0, 3*sizeof(light_dir));}
	void set_Material(const material &mat)                {m_Mat->SetRawValue(&mat, 0, sizeof(material));}
	void set_MinDist(float f)                             {m_MinDist->SetFloat(f);}
	void set_MaxDist(float f)                             {m_MaxDist->SetFloat(f);}
	void set_MinTess(float f)                             {m_MinTess->SetFloat(f);}
	void set_MaxTess(float f)                             {m_MaxTess->SetFloat(f);}
	void set_TexelCellSpaceU(float f)                     {m_TexelCellSpaceU->SetFloat(f);}
	void set_TexelCellSpaceV(float f)                     {m_TexelCellSpaceV->SetFloat(f);}
	void set_WorldCellSpace(float f)                      {m_WorldCellSpace->SetFloat(f);}
	void set_TexScale(const XMFLOAT2 &v)                  {m_TexScale->SetRawValue(&v, 0, sizeof(XMFLOAT2));}
	void set_WorldFrustumPlanes(XMFLOAT4 planes[6])       {m_WorldFrustumPlanes->SetFloatVectorArray(reinterpret_cast<float*>(planes), 0, 6);}
	void set_LayerMapArray(ID3D11ShaderResourceView *tex) {m_LayerMapArray->SetResource(tex);}
	void set_BlendMap(ID3D11ShaderResourceView *tex)      {m_BlendMap->SetResource(tex);}
	void set_HeightMap(ID3D11ShaderResourceView *tex)     {m_HeightMap->SetResource(tex);}
	void set_ShadowMap(ID3D11ShaderResourceView* tex)     {m_ShadowMap->SetResource(tex);}
	ID3DX11EffectTechnique *m_Light3Tech;
	ID3DX11EffectMatrixVariable *m_ViewProj;
	ID3DX11EffectMatrixVariable *m_ShadowTransform;
	ID3DX11EffectVectorVariable *m_EyePosW;
	ID3DX11EffectVariable *m_DirLights;
	ID3DX11EffectVariable *m_Mat;
	ID3DX11EffectScalarVariable *m_ScreenWidth;
	ID3DX11EffectScalarVariable *m_ScreenHeight;
	ID3DX11EffectScalarVariable *m_MinDist;
	ID3DX11EffectScalarVariable *m_MaxDist;
	ID3DX11EffectScalarVariable *m_MinTess;
	ID3DX11EffectScalarVariable *m_MaxTess;
	ID3DX11EffectScalarVariable *m_TexelCellSpaceU;
	ID3DX11EffectScalarVariable *m_TexelCellSpaceV;
	ID3DX11EffectScalarVariable *m_WorldCellSpace;
	ID3DX11EffectVectorVariable *m_TexScale;
	ID3DX11EffectVectorVariable *m_WorldFrustumPlanes;
	ID3DX11EffectShaderResourceVariable *m_LayerMapArray;
	ID3DX11EffectShaderResourceVariable *m_BlendMap;
	ID3DX11EffectShaderResourceVariable *m_HeightMap;
	ID3DX11EffectShaderResourceVariable *m_ShadowMap;
};
////////////////
// particle_effect
////////////////
////////////////
class particle_effect: public effect
{
public:
	particle_effect(ID3D11Device *device, const std::wstring &filename);
	~particle_effect()                                {;}
	void set_ViewProj(CXMMATRIX M)                    {m_ViewProj->SetMatrix(reinterpret_cast<const float*>(&M));}
	void set_GameTime(float f)                        {m_GameTime->SetFloat(f);}
	void set_TimeStep(float f)                        {m_TimeStep->SetFloat(f);}
	void set_EyePosW(const XMFLOAT3 &v)               {m_EyePosW->SetRawValue(&v, 0, sizeof(XMFLOAT3));}
	void set_EmitPosW(const XMFLOAT3 &v)              {m_EmitPosW->SetRawValue(&v, 0, sizeof(XMFLOAT3));}
	void set_EmitDirW(const XMFLOAT3 &v)              {m_EmitDirW->SetRawValue(&v, 0, sizeof(XMFLOAT3));}
	void set_SpecialType(int i)                       {m_SpecialType->SetInt(i);}
	void set_TexArray(ID3D11ShaderResourceView *tex)  {m_TexArray->SetResource(tex);}
	void set_RandomTex(ID3D11ShaderResourceView *tex) {m_RandomTex->SetResource(tex);}
	ID3DX11EffectTechnique *m_StreamOutTech;
	ID3DX11EffectTechnique *m_DrawTech;
	ID3DX11EffectMatrixVariable *m_ViewProj;
	ID3DX11EffectScalarVariable *m_GameTime;
	ID3DX11EffectScalarVariable *m_TimeStep;
	ID3DX11EffectVectorVariable *m_EyePosW;
	ID3DX11EffectVectorVariable *m_EmitPosW;
	ID3DX11EffectVectorVariable *m_EmitDirW;
	ID3DX11EffectScalarVariable *m_SpecialType;
	ID3DX11EffectShaderResourceVariable *m_TexArray;
	ID3DX11EffectShaderResourceVariable *m_RandomTex;
};
////////////////
// debug_tex_effect
////////////////
////////////////
class debug_tex_effect: public effect
{
public:
	debug_tex_effect(ID3D11Device *device, const std::wstring &filename);
	~debug_tex_effect()                             {;}
	void set_WorldViewProj(CXMMATRIX M)             {m_WorldViewProj->SetMatrix(reinterpret_cast<const float*>(&M));}
	void set_Texture(ID3D11ShaderResourceView *tex) {m_Texture->SetResource(tex);}
	ID3DX11EffectTechnique *m_ViewArgbTech;
	ID3DX11EffectTechnique *m_ViewRedTech;
	ID3DX11EffectTechnique *m_ViewGreenTech;
	ID3DX11EffectTechnique *m_ViewBlueTech;
	ID3DX11EffectTechnique *m_ViewAlphaTech;
	ID3DX11EffectMatrixVariable *m_WorldViewProj;
	ID3DX11EffectShaderResourceVariable *m_Texture;
};
////////////////
// color_effect
////////////////
////////////////
class color_effect: public effect
{
public:
	color_effect(ID3D11Device *device, const std::wstring &filename);
	~color_effect()                     {;}
	void set_WorldViewProj(CXMMATRIX M) {m_WorldViewProj->SetMatrix(reinterpret_cast<const float*>(&M));}
	ID3DX11EffectTechnique *m_PosColorTech;
	ID3DX11EffectMatrixVariable *m_WorldViewProj;
};
////////////////
// wave_sim_effect
////////////////
////////////////
class wave_sim_effect : public effect
{
public:
	wave_sim_effect(ID3D11Device *device, const std::wstring& filename);
	~wave_sim_effect()                                     {;}
	void set_WaveConstants(float s[3])                     {m_WaveConstants->SetFloatArray(s, 0, 3);}
	void set_GameTime(float f)                             {m_GameTime->SetFloat(f);}
	void set_DisturbMag(float s)					       {m_DisturbMag->SetFloat(s);}
	void set_DisturbIndex(int row, int col);
	void set_PrevSolInput(ID3D11ShaderResourceView *srv)   {m_PrevSolInput->SetResource(srv);}
	void set_CurrSolInput(ID3D11ShaderResourceView *srv)   {m_CurrSolInput->SetResource(srv);}
	void set_RandomTex(ID3D11ShaderResourceView *tex)      {m_RandomTex->SetResource(tex);}
	void set_CurrSolOutput(ID3D11UnorderedAccessView *uav) {m_CurrSolOutput->SetUnorderedAccessView(uav);}
	void set_NextSolOutput(ID3D11UnorderedAccessView *uav) {m_NextSolOutput->SetUnorderedAccessView(uav);}
	ID3DX11EffectTechnique *m_UpdateWavesTech;
	ID3DX11EffectTechnique *m_DisturbWavesTech;
	ID3DX11EffectScalarVariable *m_WaveConstants;
	ID3DX11EffectScalarVariable *m_GameTime;
	ID3DX11EffectScalarVariable *m_DisturbMag;
	ID3DX11EffectVectorVariable *m_DisturbIndex;
	ID3DX11EffectShaderResourceVariable *m_PrevSolInput;
	ID3DX11EffectShaderResourceVariable *m_CurrSolInput;
	ID3DX11EffectShaderResourceVariable *m_RandomTex;
	ID3DX11EffectUnorderedAccessViewVariable *m_CurrSolOutput;
	ID3DX11EffectUnorderedAccessViewVariable *m_NextSolOutput;
};
////////////////
// wave_render_effect
////////////////
////////////////
class wave_render_effect : public effect
{
public:
	wave_render_effect(ID3D11Device *device, const std::wstring& filename);
	~wave_render_effect()                                   {;}
	void set_WorldViewProj(CXMMATRIX M)                     {m_WorldViewProj->SetMatrix(reinterpret_cast<const float*>(&M));}
	void set_World(CXMMATRIX M)                             {m_World->SetMatrix(reinterpret_cast<const float*>(&M));}
	void set_WorldInvTranspose(CXMMATRIX M)                 {m_WorldInvTranspose->SetMatrix(reinterpret_cast<const float*>(&M));}
	void set_TexTransform(CXMMATRIX M)                      {m_TexTransform->SetMatrix(reinterpret_cast<const float*>(&M));}
	void set_EyePosW(const XMFLOAT3& v)                     {m_EyePosW->SetRawValue(&v, 0, sizeof(XMFLOAT3));}
	void set_GridSpatialStep(float f)                       {m_GridSpatialStep->SetFloat(f);}
	void set_DisplacementTexelSize(const XMFLOAT2& v)       {m_DisplacementMapTexelSize->SetRawValue(&v, 0, sizeof(XMFLOAT2));}
	void set_DirLights(const light_dir *lights)             {m_DirLights->SetRawValue(lights, 0, 3*sizeof(light_dir));}
	void set_Material(const material &mat)                  {m_Mat->SetRawValue(&mat, 0, sizeof(material));}
	void set_DiffuseMap(ID3D11ShaderResourceView *tex)      {m_DiffuseMap->SetResource(tex);}
	void set_DisplacementMap(ID3D11ShaderResourceView *tex) {m_DisplacementMap->SetResource(tex);}
	ID3DX11EffectTechnique *m_Light3Tech;
	ID3DX11EffectTechnique *m_Light3TexTech;
	ID3DX11EffectMatrixVariable *m_WorldViewProj;
	ID3DX11EffectMatrixVariable *m_World;
	ID3DX11EffectMatrixVariable *m_WorldInvTranspose;
	ID3DX11EffectMatrixVariable *m_TexTransform;
	ID3DX11EffectVectorVariable *m_EyePosW;
	ID3DX11EffectScalarVariable *m_GridSpatialStep;
	ID3DX11EffectVectorVariable *m_DisplacementMapTexelSize;
	ID3DX11EffectVariable *m_DirLights;
	ID3DX11EffectVariable *m_Mat;
	ID3DX11EffectShaderResourceVariable *m_DiffuseMap;
	ID3DX11EffectShaderResourceVariable *m_DisplacementMap;
};
////////////////
// effects
////////////////
////////////////
class effects
{
public:
	static void init_all(ID3D11Device *device, const std::string &imm_path_shader);
	static void destroy_all();
	static basic_effect            *m_BasicFX;
	static normal_map_effect       *m_NormalMapFX;
	static build_shadow_map_effect *m_BuildShadowMapFX;
	static sky_effect              *m_SkyFX;
	static terrain_effect          *m_TerrainFX;
	static debug_tex_effect        *m_DebugTexFX;
	static color_effect            *m_PosColorFX;
	static wave_sim_effect         *m_WaveSimFX;
	static wave_render_effect      *m_WaveRenderFX;
	static particle_effect         *m_PtFireFX;
	static particle_effect         *m_PtRainFX;
	static particle_effect         *m_PtStrikeFX;
	static particle_effect         *m_PtStrike2FX;
	static particle_effect         *m_PtChargeFX;
	static particle_effect         *m_PtLightningFX;
	static particle_effect         *m_PtBrokenFX;
	static particle_effect         *m_PtBlockFX;
	static particle_effect         *m_PtMissile1;
};
}
#endif