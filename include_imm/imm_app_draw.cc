////////////////
// imm_app_draw.cc
////////////////
////////////////
void imm_app::draw_scene()
{
	// ID3D11DeviceContext is not thread-safe, avoid loading conflict
	if (m_Cmd.is_draw_loading_1frame) {
		draw_scene_d3d_slient();
		draw_scene_d2d();		
		m_Cmd.is_draw_loading_1frame = false;
	}
	else if (m_Cmd.is_wait_loading) {
		m_Cmd.loading_time_min -= m_Timer.delta_time();
		if (m_Cmd.loading_time_min < 0.0f) {
			m_Cmd.is_wait_loading = false;
		}
		if (!m_Cmd.is_preparing) {
			draw_scene_d3d_slient();
			draw_scene_d2d();
		}
	}
	else {
		if (m_Cmd.is_preparing) return;
		if (!m_Cmd.is_waiting_for_something()) draw_scene_d3d();
		else draw_scene_d3d_slient();
		draw_scene_d2d();
	}
	// Synchronize presentation
	if (m_IsSyncInterval) m_SwapChain->Present1(1, 0, &m_DXGIPresentPara);
	else m_SwapChain->Present(0, 0);
}
//
void imm_app::build_shadow_transform()
{
	// Only the first "main" light casts a shadow.
	XMVECTOR light_dir = XMLoadFloat3(&m_Scene.dir_lights[0].direction);
	XMVECTOR light_pos = -2.0f*m_Scene.bounds.Radius*m_Scene.shadow_light_pos_far*light_dir;
	XMVECTOR target_pos = XMLoadFloat3(&m_Scene.bounds.Center);
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	XMMATRIX V = XMMatrixLookAtLH(light_pos, target_pos, up);
	// Transform bounding sphere to light space.
	XMFLOAT3 sphere_center_ls;
	XMStoreFloat3(&sphere_center_ls, XMVector3TransformCoord(target_pos, V));
	// Ortho frustum in light space encloses scene.
	float l = sphere_center_ls.x - m_Scene.bounds.Radius;
	float b = sphere_center_ls.y - m_Scene.bounds.Radius;
	float n = sphere_center_ls.z - m_Scene.bounds.Radius;
	float r = sphere_center_ls.x + m_Scene.bounds.Radius;
	float t = sphere_center_ls.y + m_Scene.bounds.Radius;
	float f = sphere_center_ls.z + m_Scene.bounds.Radius;
	XMMATRIX P = XMMatrixOrthographicOffCenterLH(l, r, b, t, n, f);
	// Transform NDC space [-1,+1]^2 to texture space [0,1]^2
	XMMATRIX T(
		0.5f, 0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.5f, 0.5f, 0.0f, 1.0f);
	XMMATRIX S = V*P*T;
	XMStoreFloat4x4(&m_LightView, V);
	XMStoreFloat4x4(&m_LightProj, P);
	XMStoreFloat4x4(&m_ShadowTransform, S);
}
//
void imm_app::draw_scene_d3d_shadow()
{
	XMMATRIX view = XMLoadFloat4x4(&m_LightView);
	XMMATRIX proj = XMLoadFloat4x4(&m_LightProj);
	XMMATRIX view_proj = XMMatrixMultiply(view, proj);
	auto fx_shadow = effects::m_BuildShadowMapFX;
	fx_shadow->set_EyePosW(m_Cam.get_Position());
	fx_shadow->set_ViewProj(view_proj);
	ID3DX11EffectTechnique *tech_shadow = fx_shadow->m_BuildShadowMapTech;
	ID3DX11EffectTechnique *tech_shadow_alpha = fx_shadow->m_BuildShadowMapAlphaClipTech;
	ID3DX11EffectTechnique *tech_shadow_skinned = fx_shadow->m_BuildShadowMapSkinnedTech;
	ID3DX11EffectTechnique *tech_shadow_skinned_alpha = fx_shadow->m_BuildShadowMapAlphaClipSkinnedTech;
	D3DX11_TECHNIQUE_DESC tech_desc;
	m_D3DDC->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	// Draw simple pntt
	m_D3DDC->IASetInputLayout(input_layouts::m_PNTT);
	draw_inst_simple_pntt_shadow(
		m_D3DDC,
		tech_shadow,
		tech_desc,
		fx_shadow,
		m_Inst.m_Model.m_InstPNTT,
		view_proj
	);
	// Draw basic
	m_D3DDC->IASetInputLayout(input_layouts::m_PNTT2);
	draw_inst_basic_shadow(
		m_D3DDC,
		tech_shadow,
		tech_desc,
		fx_shadow,
		m_Inst.m_Model.m_InstBasic,
		false,
		view_proj
	);
	// Draw basic alpha
	draw_inst_basic_shadow(
		m_D3DDC,
		tech_shadow_alpha,
		tech_desc,
		fx_shadow,
		m_Inst.m_Model.m_InstBasicAlpha,
		true,
		view_proj
	);
	// Draw skinned
	m_D3DDC->IASetInputLayout(input_layouts::m_PNTTSkinned);
	draw_inst_skinned_shadow(
		m_D3DDC,
		tech_shadow_skinned,
		tech_desc,
		fx_shadow,
		m_Inst.m_Model.m_InstSkinned,
		false,
		view_proj
	);
	// Draw skinned alpha
	draw_inst_skinned_shadow(
		m_D3DDC,
		tech_shadow_skinned_alpha,
		tech_desc,
		fx_shadow,
		m_Inst.m_Model.m_InstSkinnedAlpha,
		true,
		view_proj
	);
}
//
void imm_app::draw_scene_d3d()
{
	// Render the scene to the shadow map.
	m_Smap->bind_dsv_and_set_null_render_target(m_D3DDC);
	draw_scene_d3d_shadow();
	m_D3DDC->RSSetState(0);
	// Restore the back and depth buffer and viewport to the OM stage.
	ID3D11RenderTargetView* render_targets[1] = {m_RenderTargetView};
	m_D3DDC->OMSetRenderTargets(1, render_targets, m_DepthStencilView);
	m_D3DDC->RSSetViewports(1, &m_ScreenViewport);
	m_D3DDC->ClearRenderTargetView(m_RenderTargetView, reinterpret_cast<const float*>(&Colors::Black));
	m_D3DDC->ClearDepthStencilView(m_DepthStencilView, D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL, 1.0f, 0);
	//
	auto *fx_basic = effects::m_BasicFX;
	auto *fx_normal = effects::m_NormalMapFX;
	// Set per frame constants.
	fx_basic->set_DirLights(m_Scene.dir_lights);
	fx_basic->set_EyePosW(m_Cam.get_Position());
	fx_basic->set_CubeMap(m_Scene.skybox->get_CubeMapSRV());
	fx_basic->set_ShadowMap(m_Smap->get_DepthMapSRV());
	fx_normal->set_DirLights(m_Scene.dir_lights);
	fx_normal->set_EyePosW(m_Cam.get_Position());
	fx_normal->set_CubeMap(m_Scene.skybox->get_CubeMapSRV());
	fx_normal->set_ShadowMap(m_Smap->get_DepthMapSRV());
	ID3DX11EffectTechnique *tech_simple = fx_basic->m_Light3Tech;
	ID3DX11EffectTechnique *tech_basic = fx_normal->m_Light3TexTech;
	ID3DX11EffectTechnique *tech_basic_alpha = fx_normal->m_Light3TexAlphaClipTech;
	ID3DX11EffectTechnique *tech_skinned = fx_normal->m_Light3TexSkinnedTech;
	ID3DX11EffectTechnique *tech_skinned_alpha = fx_normal->m_Light3TexAlphaClipSkinnedTech;
	D3DX11_TECHNIQUE_DESC tech_desc;
	XMMATRIX view_proj = m_Cam.get_ViewProj();
	// Transform NDC space [-1,+1]^2 to texture space [0,1]^2
	XMMATRIX to_tex_space(
		0.5f, 0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.5f, 0.5f, 0.0f, 1.0f);
	XMMATRIX shadow_transform = XMLoadFloat4x4(&m_ShadowTransform);
	m_D3DDC->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	// Draw simple pntt
	m_D3DDC->IASetInputLayout(input_layouts::m_PNTT);
	draw_inst_simple_pntt(
		m_D3DDC,
		tech_basic,
		tech_simple,
		tech_desc,
		fx_normal,
		fx_basic,
		m_Inst.m_Model.m_InstPNTT,
		view_proj,
		shadow_transform,
		to_tex_space
	);
	// Draw basic
	m_D3DDC->IASetInputLayout(input_layouts::m_PNTT2);
	draw_inst_basic(
		m_D3DDC,
		tech_basic,
		tech_desc,
		fx_normal,
		m_Inst.m_Model.m_InstBasic,
		view_proj,
		shadow_transform,
		to_tex_space
	);
	// Draw basic alpha
	m_D3DDC->RSSetState(render::m_NoCullRS);
	draw_inst_basic(
		m_D3DDC,
		tech_basic_alpha,
		tech_desc,
		fx_normal,
		m_Inst.m_Model.m_InstBasicAlpha,
		view_proj,
		shadow_transform,
		to_tex_space
	);
	// Draw skinned alpha
	m_D3DDC->IASetInputLayout(input_layouts::m_PNTTSkinned);
	draw_inst_skinned(
		m_D3DDC,
		tech_skinned_alpha,
		tech_desc,
		fx_normal,
		m_Inst.m_Model.m_InstSkinnedAlpha,
		view_proj,
		shadow_transform,
		to_tex_space,
		m_Inst.m_Stat
	);
	//m_D3DDC->RSSetState(0);
	// Draw skinned
	draw_inst_skinned(
		m_D3DDC,
		tech_skinned,
		tech_desc,
		fx_normal,
		m_Inst.m_Model.m_InstSkinned,
		view_proj,
		shadow_transform,
		to_tex_space,
		m_Inst.m_Stat
	);
	// Draw the others
	m_Scene.draw_d3d_atmosphere(shadow_transform);
	m_UiMgr.draw_d3d();
	// Unbind shadow map and AmbientMap as a shader input because we are going to render
	// To it next frame.  These textures can be at any slot, so clear all slots.
	ID3D11ShaderResourceView *null_srv[16] = {0};
	m_D3DDC->PSSetShaderResources(0, 16, null_srv);
}
//
void imm_app::draw_scene_d3d_slient()
{
	assert(m_D3DDC);
	assert(m_SwapChain);
	m_D3DDC->ClearRenderTargetView(m_RenderTargetView, reinterpret_cast<const float*>(&m_Cmd.back_color));
	m_D3DDC->ClearDepthStencilView(m_DepthStencilView, D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL, 1.0f, 0);
	if (m_Cmd.is_wait_loading) {
		m_Cmd.draw_d3d_loading();
	}
}
//
void imm_app::draw_scene_d2d()
{
	assert(m_D2DDC);
	m_D2DDC->BeginDraw();
	m_D2DDC->SetTransform(D2D1::IdentityMatrix());
	if (m_Cmd.is_wait_loading) {
		m_Cmd.draw_d2d_loading();
	}
	else {
		m_Cmd.draw_d2d();
		m_UiMgr.draw_d2d();
	}
	HR(m_D2DDC->EndDraw());
}
