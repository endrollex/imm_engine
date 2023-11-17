////////////////
// imm_base.h
// an abstract base class, from which specific window classes are derived
// by Huang Yiting
////////////////
////////////////
#ifndef IMM_BASE_H
#define IMM_BASE_H
#include <Windowsx.h>
#include <DXGI1_2.h>
#include <string>
#include <d2d1_1.h>
#include "util_basic.h"
#include <imm.h>
namespace imm
{
////////////////
// base_win
////////////////
////////////////
template <class DERIVED_TYPE>
class base_win
{
public:
	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	base_win();
	virtual ~base_win();
	BOOL init_win();
	bool init_d3d();
	HWND get_hwnd() const {return m_hwnd;}
	float aspect_ratio() const {return static_cast<float>(m_ClientWidth)/static_cast<float>(m_ClientHeight);}
	int run();
	virtual void on_resize();
	virtual void update_scene(float dt) = 0;
	virtual void draw_scene() = 0;
	virtual LRESULT handle_message(UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual void handle_message_WM_SIZE(WPARAM wParam, LPARAM lParam);
	// Convenience overrides for handling mouse input.
	virtual void on_mouse_down(WPARAM btn_state, int x, int y) {DUMMY(x); DUMMY(y); DUMMY(btn_state);}
	virtual void on_mouse_up(WPARAM btn_state, int x, int y) {DUMMY(x); DUMMY(y); DUMMY(btn_state);}
	virtual void on_mouse_move(WPARAM btn_state, int x, int y) {DUMMY(x); DUMMY(y); DUMMY(btn_state);}
	virtual void on_mouse_wheel(WPARAM btn_state, int x, int y) {DUMMY(x); DUMMY(y); DUMMY(btn_state);}
	virtual void on_input_char(WPARAM w_param, LPARAM l_param) {DUMMY(w_param); DUMMY(l_param);}
	virtual void on_input_keydown(WPARAM w_param, LPARAM l_param) {DUMMY(w_param); DUMMY(l_param);}
	virtual void on_input_keyup(WPARAM w_param, LPARAM l_param) {DUMMY(w_param); DUMMY(l_param);}
	virtual void game_suspend(const bool &is_stop);
	virtual PCWSTR class_name() const {return L"Immature Engine Class";}
	void calc_frmae_stats();
	//
	HWND m_hwnd;
	bool m_Paused;
	bool m_Minimized;
	bool m_Maximized;
	bool m_FullScreen;
	bool m_Resizing;
	bool m_Enable4xMsaa;
	bool m_IsInteropD2D;
	bool m_IsLockFrameRate;
	UINT m_4xMsaaQuality;
	int m_ClientWidth;
	int m_ClientHeight;
	float m_AspectRatio;
	double m_FrameDeltaLock;
	FLOAT m_DpiX;
	FLOAT m_DpiY;
	std::wstring m_WindowName;
	std::wstring m_Fps;
	timer m_Timer;
	ID3D11Device            *m_D3DDevice;
	ID3D11DeviceContext     *m_D3DDC;
	IDXGISwapChain1         *m_SwapChain;
	ID3D11Texture2D         *m_DepthStencilBuffer;
	ID3D11RenderTargetView  *m_RenderTargetView;
	ID3D11DepthStencilView  *m_DepthStencilView;
	D3D11_VIEWPORT          m_ScreenViewport;
	DXGI_PRESENT_PARAMETERS m_DXGIPresentPara;
	// D2D
	ID2D1Device             *m_D2DDevice;
	ID2D1DeviceContext      *m_D2DDC;
	ID2D1Bitmap1            *m_D2DTargetBitmap;
	D2D1_BITMAP_PROPERTIES1 m_D2DBitMapProp;
private:
	base_win(const base_win &rhs);
	base_win &operator=(const base_win &rhs);
};
////////////////
// see 3D Game Programming with DirectX 11 by Frank Luna, d3dApp.h
////////////////
////////////////
template <class DERIVED_TYPE>
base_win<DERIVED_TYPE>::base_win():
	m_hwnd(NULL),
	m_Paused(false),
	m_Minimized(false),
	m_Maximized(false),
	m_FullScreen(false),
	m_Resizing(false),
	m_Enable4xMsaa(false),
	m_IsInteropD2D(true),
	m_IsLockFrameRate(false),
	m_4xMsaaQuality(0),
	m_ClientWidth(1067),
	m_ClientHeight(600),
	m_AspectRatio(0.0f),
	m_FrameDeltaLock(1.0/60.0),
	m_DpiX(96.0f),
	m_DpiY(96.0f),
	m_WindowName(L"D3D11 Demo"),
	m_D3DDevice(nullptr),
	m_D3DDC(nullptr),
	m_SwapChain(nullptr),
	m_DepthStencilBuffer(nullptr),
	m_RenderTargetView(nullptr),
	m_DepthStencilView(nullptr),
	m_DXGIPresentPara(),
	m_D2DDevice(nullptr),
	m_D2DDC(nullptr),
	m_D2DTargetBitmap(nullptr),
	m_D2DBitMapProp(D2D1::BitmapProperties1())
{
	IMM_PATH_init();
	ZeroMemory(&m_ScreenViewport, sizeof(D3D11_VIEWPORT));
	ImmDisableIME(0xffffffff);
}
//
template <class DERIVED_TYPE>
base_win<DERIVED_TYPE>::~base_win()
{
	OutputDebugString(L"IMM ~base_win");
	RELEASE_COM(m_RenderTargetView);
	RELEASE_COM(m_DepthStencilView);
	RELEASE_COM(m_SwapChain);
	RELEASE_COM(m_DepthStencilBuffer);
	// Restore all default settings.
	if (m_D3DDC) m_D3DDC->ClearState();
	RELEASE_COM(m_D3DDC);
	//RELEASE_COM(m_D3DDevice);
	// D2D Release
	RELEASE_COM(m_D2DDevice);
	RELEASE_COM(m_D2DDC);
	RELEASE_COM(m_D2DTargetBitmap);
#if defined(DEBUG) || defined(_DEBUG)
	ID3D11Debug *d3d_debug;
	HRESULT hr = m_D3DDevice->QueryInterface(__uuidof(ID3D11Debug), reinterpret_cast<void**>(&d3d_debug));
	if (SUCCEEDED(hr))
	{
		HR(d3d_debug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL));
		OutputDebugString(L"IMM ReportLiveDeviceObjects");
	}
	RELEASE_COM(d3d_debug);
#endif
	RELEASE_COM(m_D3DDevice);
	OutputDebugString(L"IMM quit");
}
//
template <class DERIVED_TYPE>
LRESULT CALLBACK base_win<DERIVED_TYPE>::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	DERIVED_TYPE *pThis = NULL;
	if (uMsg == WM_NCCREATE) {
		CREATESTRUCT *pCreate = (CREATESTRUCT*)lParam;
		pThis = (DERIVED_TYPE*)pCreate->lpCreateParams;
		SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)pThis);
		pThis->m_hwnd = hwnd;
	}
	else pThis = (DERIVED_TYPE*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
	if (pThis) return pThis->handle_message(uMsg, wParam, lParam);
	else return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
//
template <class DERIVED_TYPE>
BOOL base_win<DERIVED_TYPE>::init_win()
{
	OutputDebugString(L"IMM init_win");
	// DPI in game always 96.0f, UI corresponding screen's height and width, not only DPI
	assert(SetProcessDPIAware());
	// WNDCLASS
	WNDCLASS wc = {0};
	wc.style         = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc   = DERIVED_TYPE::WindowProc;
	wc.hInstance     = GetModuleHandle(NULL);
	wc.lpszClassName = class_name();
	wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
	RegisterClassW(&wc);
	// Rect
	RECT rect = {0, 0, m_ClientWidth, m_ClientHeight};
	LONG l_style = WS_OVERLAPPEDWINDOW;
	// Full screen (windowed)
	if (m_FullScreen) {
		GetWindowRect(GetDesktopWindow(), &rect);
		l_style = WS_POPUP;
	}
	else {
		l_style = WS_OVERLAPPEDWINDOW;
	}
	AdjustWindowRect(&rect, l_style, false);
	int width  = rect.right - rect.left;
	int height = rect.bottom - rect.top;
	m_hwnd = CreateWindowEx(
		0,
		class_name(),
		m_WindowName.c_str(),
		l_style,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		width,
		height,
		0,
		0,
		GetModuleHandle(NULL),
		this
	);
	return (m_hwnd ? true : false);
}
//
template <class DERIVED_TYPE>
bool base_win<DERIVED_TYPE>::init_d3d()
{
	// Create the device and device context.
	UINT create_device_flags = 0;
#if defined(DEBUG) || defined(_DEBUG)
	create_device_flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
	// This flag adds support for surfaces with a different color channel ordering than the API default.
	// You need it for compatibility with Direct2D.
	if (m_IsInteropD2D) {
		create_device_flags |= D3D11_CREATE_DEVICE_BGRA_SUPPORT;
	}
	D3D_FEATURE_LEVEL set_feature_level[] = {
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_9_3,
		D3D_FEATURE_LEVEL_9_2,
		D3D_FEATURE_LEVEL_9_1,
	};
	D3D_FEATURE_LEVEL check_feature_level;
	HRESULT hr = D3D11CreateDevice(
		nullptr,                      // default adapter
		D3D_DRIVER_TYPE_HARDWARE,
		0,                            // no software device
		create_device_flags,
		set_feature_level,
		ARRAYSIZE(set_feature_level),
		D3D11_SDK_VERSION,
		&m_D3DDevice,
		&check_feature_level,
		&m_D3DDC
	);
	if (FAILED(hr)) {
		std::wstring hr_mes = std::to_wstring(hr);
		hr_mes = L"D3D11CreateDevice Failed: "+hr_mes;
		MessageBox(0, hr_mes.c_str(), 0, 0);
		return false;
	}
	if (check_feature_level < D3D_FEATURE_LEVEL_11_0) {
		MessageBox(0, L"Direct3D Feature Level 11 unsupported.", 0, 0);
		return false;
	}
	if (!XMVerifyCPUSupport()) {
		MessageBox(0, L"DirectXMath unsupported.", 0, 0);
		return false;
	}
	// Check ID3D11Device multithreading
	D3D11_FEATURE_DATA_THREADING check_feature_support;
	m_D3DDevice->CheckFeatureSupport(
		D3D11_FEATURE_THREADING, &check_feature_support, sizeof(check_feature_support));
	if (!check_feature_support.DriverConcurrentCreates) {
		MessageBox(0, L"The graphics hardware multithreading unsupported.", 0, 0);
		return false;
	}
	// Check 4X MSAA quality support for our back buffer format.
	// All Direct3D 11 capable devices support 4X MSAA for all render
	// target formats, so we only need to check quality support.
	HR(m_D3DDevice->CheckMultisampleQualityLevels(DXGI_FORMAT_B8G8R8A8_UNORM, 4, &m_4xMsaaQuality));
	assert(m_4xMsaaQuality > 0);
	// Fill out a DXGI_SWAP_CHAIN_DESC to describe our swap chain.
	DXGI_SWAP_CHAIN_DESC1 sd;
	sd.Width                  = 0;
	sd.Height                 = 0;
	sd.Format                 = DXGI_FORMAT_B8G8R8A8_UNORM;
	sd.Stereo                 = false;
	// Use 4X MSAA?
	if (m_Enable4xMsaa) {
		sd.SampleDesc.Count   = 4;
		sd.SampleDesc.Quality = m_4xMsaaQuality-1;
	}
	// No MSAA
	else {
		sd.SampleDesc.Count   = 1;
		sd.SampleDesc.Quality = 0;
	}
	sd.BufferUsage            = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferCount            = 1;
	sd.Scaling                = DXGI_SCALING_STRETCH;
	sd.SwapEffect             = DXGI_SWAP_EFFECT_DISCARD;
	sd.AlphaMode              = DXGI_ALPHA_MODE_UNSPECIFIED;
	sd.Flags                  = 0;
	// To correctly create the swap chain, we must use the IDXGIFactory that was
	// used to create the device.  If we tried to use a different IDXGIFactory instance
	// (by calling CreateDXGIFactory), we get an error: "IDXGIFactory::CreateSwapChain:
	// This function is being called with a device from a different IDXGIFactory."
	IDXGIDevice1 *dxgi_device = 0;
	HR(m_D3DDevice->QueryInterface(IID_PPV_ARGS(&dxgi_device)));
	IDXGIAdapter1 *dxgi_adapter = 0;
	HR(dxgi_device->GetParent(IID_PPV_ARGS(&dxgi_adapter)));
	IDXGIFactory2 *dxgi_factory = 0;
	HR(dxgi_adapter->GetParent(IID_PPV_ARGS(&dxgi_factory)));
	HR(dxgi_factory->CreateSwapChainForHwnd(m_D3DDevice, m_hwnd, &sd, nullptr, nullptr, &m_SwapChain));
	m_DXGIPresentPara.DirtyRectsCount = 0;
	m_DXGIPresentPara.pDirtyRects     = nullptr;
	m_DXGIPresentPara.pScrollRect     = nullptr;
	m_DXGIPresentPara.pScrollOffset   = nullptr;
	// D2D init
	if (m_IsInteropD2D) {
		ID2D1Factory1 *d2d_factory;
		HR(D2D1CreateFactory(D2D1_FACTORY_TYPE_MULTI_THREADED, &d2d_factory));
		HR(d2d_factory->CreateDevice(dxgi_device, &m_D2DDevice));
		// warning C4996: 'ID2D1Factory::GetDesktopDpi': Deprecated. 
		//d2d_factory->GetDesktopDpi(&m_DpiX, &m_DpiY);
		m_DpiX = static_cast<float>(GetDpiForWindow(get_hwnd()));
		m_DpiY = m_DpiX;
		m_D2DBitMapProp =
			D2D1::BitmapProperties1(
				D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
				D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_IGNORE), m_DpiX, m_DpiY);
		RELEASE_COM(d2d_factory);
	}
	RELEASE_COM(dxgi_device);
	RELEASE_COM(dxgi_adapter);
	RELEASE_COM(dxgi_factory);
	// The remaining steps that need to be carried out for d3d creation
	// also need to be executed every time the window is resized.  So
	// just call the on_resize method here to avoid code duplication.
	on_resize();
	return true;
}
//
template <class DERIVED_TYPE>
int base_win<DERIVED_TYPE>::run()
{
	MSG msg = {};
	m_Timer.reset();
	while (msg.message != WM_QUIT) {
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else {
			m_Timer.tick();
			if (!m_Paused) {
				calc_frmae_stats();
				update_scene(m_Timer.delta_time());
				draw_scene();
				if (m_IsLockFrameRate) {
					DWORD time_wait = static_cast<DWORD>((m_FrameDeltaLock-m_Timer.delta_time_test())*1000.0);
					if (time_wait > 0 && time_wait < 100) Sleep(time_wait);
				}
			}
			else Sleep(100);
		}
	}
	return (int)msg.wParam;
}
//
template <class DERIVED_TYPE>
void base_win<DERIVED_TYPE>::on_resize()
{
	assert(m_D3DDC);
	assert(m_D3DDevice);
	assert(m_SwapChain);
	m_AspectRatio = aspect_ratio();
	// Release the old views, as they hold references to the buffers we
	// will be destroying.  Also release the old depth/stencil buffer.
	RELEASE_COM(m_RenderTargetView);
	RELEASE_COM(m_DepthStencilView);
	RELEASE_COM(m_DepthStencilBuffer);
	// D2D
	if (m_IsInteropD2D) {
		assert(m_D2DDevice);
		RELEASE_COM(m_D2DTargetBitmap);
		RELEASE_COM(m_D2DDC);
	}
	// Resize the swap chain and recreate the render target view.
	HR(m_SwapChain->ResizeBuffers(1, m_ClientWidth, m_ClientHeight, DXGI_FORMAT_B8G8R8A8_UNORM, 0));
	IDXGISurface *dxgi_back_buffer = 0;
	if (m_IsInteropD2D) {
		HR(m_D2DDevice->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &m_D2DDC));
		HR(m_SwapChain->GetBuffer(0, IID_PPV_ARGS(&dxgi_back_buffer)));
		HR(m_D2DDC->CreateBitmapFromDxgiSurface(
			dxgi_back_buffer,
			m_D2DBitMapProp,
			&m_D2DTargetBitmap));
		m_D2DDC->SetTarget(m_D2DTargetBitmap);
	}
	ID3D11Texture2D *back_buffer;
	HR(m_SwapChain->GetBuffer(0, IID_PPV_ARGS(&back_buffer)));
	HR(m_D3DDevice->CreateRenderTargetView(back_buffer, 0, &m_RenderTargetView));
	RELEASE_COM(back_buffer);
	if (m_IsInteropD2D) {
		RELEASE_COM(dxgi_back_buffer);
	}
	// Create the depth/stencil buffer and view.
	D3D11_TEXTURE2D_DESC depth_stencil_desc;
	depth_stencil_desc.Width                  = m_ClientWidth;
	depth_stencil_desc.Height                 = m_ClientHeight;
	depth_stencil_desc.MipLevels              = 1;
	depth_stencil_desc.ArraySize              = 1;
	depth_stencil_desc.Format                 = DXGI_FORMAT_D24_UNORM_S8_UINT;
	// Use 4X MSAA? --must match swap chain MSAA values.
	if (m_Enable4xMsaa) {
		depth_stencil_desc.SampleDesc.Count   = 4;
		depth_stencil_desc.SampleDesc.Quality = m_4xMsaaQuality-1;
	}
	// No MSAA
	else {
		depth_stencil_desc.SampleDesc.Count   = 1;
		depth_stencil_desc.SampleDesc.Quality = 0;
	}
	depth_stencil_desc.Usage                  = D3D11_USAGE_DEFAULT;
	depth_stencil_desc.BindFlags              = D3D11_BIND_DEPTH_STENCIL;
	depth_stencil_desc.CPUAccessFlags         = 0;
	depth_stencil_desc.MiscFlags              = 0;
	HR(m_D3DDevice->CreateTexture2D(&depth_stencil_desc, 0, &m_DepthStencilBuffer));
	HR(m_D3DDevice->CreateDepthStencilView(m_DepthStencilBuffer, 0, &m_DepthStencilView));
	// Bind the render target view and depth/stencil view to the pipeline.
	m_D3DDC->OMSetRenderTargets(1, &m_RenderTargetView, m_DepthStencilView);
	// Set the viewport transform.
	m_ScreenViewport.TopLeftX = 0;
	m_ScreenViewport.TopLeftY = 0;
	m_ScreenViewport.Width    = static_cast<float>(m_ClientWidth);
	m_ScreenViewport.Height   = static_cast<float>(m_ClientHeight);
	m_ScreenViewport.MinDepth = 0.0f;
	m_ScreenViewport.MaxDepth = 1.0f;
	m_D3DDC->RSSetViewports(1, &m_ScreenViewport);
}
//
template <class DERIVED_TYPE>
LRESULT base_win<DERIVED_TYPE>::handle_message(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg) {
	// WM_ACTIVATE is sent when the window is activated or deactivated.
	// We pause the game when the window is deactivated and unpause it
	// when it becomes active.
	case WM_ACTIVATE:
		if (LOWORD(wParam) == WA_INACTIVE) {
			game_suspend(true);
		}
		else {
			game_suspend(false);
		}
		return 0;
	// WM_SIZE is sent when the user resizes the window.
	case WM_SIZE:
		handle_message_WM_SIZE(wParam, lParam);
		return 0;
	// WM_EXITSIZEMOVE is sent when the user grabs the resize bars.
	case WM_ENTERSIZEMOVE:
		m_Resizing = true;
		game_suspend(true);
		return 0;
	// WM_EXITSIZEMOVE is sent when the user releases the resize bars.
	// Here we reset everything based on the new window dimensions.
	case WM_EXITSIZEMOVE:
		m_Resizing = false;
		game_suspend(false);
		on_resize();
		return 0;
	// WM_DESTROY is sent when the window is being destroyed.
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	// The WM_MENUCHAR message is sent when a menu is active and the user presses
	// a key that does not correspond to any mnemonic or accelerator key.
	case WM_MENUCHAR:
		// Don't beep when we alt-enter.
		return MAKELRESULT(0, MNC_CLOSE);
	// Catch this message so to prevent the window from becoming too small.
	case WM_GETMINMAXINFO:
		((MINMAXINFO*)lParam)->ptMinTrackSize.x = 800;
		((MINMAXINFO*)lParam)->ptMinTrackSize.y = 600;
		return 0;
	case WM_LBUTTONDOWN:
	case WM_MBUTTONDOWN:
	case WM_RBUTTONDOWN:
		on_mouse_down(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;
	case WM_LBUTTONUP:
	case WM_MBUTTONUP:
	case WM_RBUTTONUP:
		on_mouse_up(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;
	case WM_MOUSEMOVE:
		on_mouse_move(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;
	case WM_MOUSEWHEEL:
		on_mouse_wheel(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;
	case WM_CHAR:
		on_input_char(wParam, lParam);
		return 0;
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
		on_input_keydown(wParam, lParam);
	case WM_KEYUP:
		on_input_keyup(wParam, lParam);
		return 0;
	}
	return DefWindowProc(m_hwnd, uMsg, wParam, lParam);
}
//
template <class DERIVED_TYPE>
void base_win<DERIVED_TYPE>::handle_message_WM_SIZE(WPARAM wParam, LPARAM lParam)
{
	// Save the new client area dimensions.
	m_ClientWidth  = LOWORD(lParam);
	m_ClientHeight = HIWORD(lParam);
	if (!m_D3DDevice) return;
	if (wParam == SIZE_MINIMIZED) {
		game_suspend(true);
		m_Minimized = true;
		m_Maximized = false;
		return;
	}
	if (wParam == SIZE_MAXIMIZED) {
		game_suspend(false);
		m_Minimized = false;
		m_Maximized = true;
		on_resize();
		return;
	}
	if (wParam == SIZE_RESTORED) {
		// Restoring from minimized state?
		if (m_Minimized) {
			game_suspend(false);
			m_Minimized = false;
			on_resize();
			return;
		}
		// Restoring from maximized state?
		if (m_Maximized) {
			game_suspend(false);
			m_Maximized = false;
			on_resize();
			return;
		}
		if (m_Resizing) {
			// If user is dragging the resize bars, we do not resize
			// the buffers here because as the user continuously
			// drags the resize bars, a stream of WM_SIZE messages are
			// sent to the window, and it would be pointless (and slow)
			// to resize for each WM_SIZE message received from dragging
			// the resize bars.  So instead, we reset after the user is
			// done resizing the window and releases the resize bars, which
			// sends a WM_EXITSIZEMOVE message.
			return;
		}
		else {
			// API call such as SetWindowPos or m_SwapChain->SetFullscreenState.
			on_resize();
			return;
		}
	}
}
//
template <class DERIVED_TYPE>
void base_win<DERIVED_TYPE>::game_suspend(const bool &is_stop)
{
	// derived game_suspend() must call this base version
	m_Paused = is_stop;
	if (is_stop) m_Timer.stop();
	else m_Timer.start();
}
//
template <class DERIVED_TYPE>
void base_win<DERIVED_TYPE>::calc_frmae_stats()
{
	// Code computes the average frames per second, and also the
	// average time it takes to render one frame.  These stats
	// are appended to the window caption bar.
	static int frame_cnt = 0;
	static float time_elapsed = 0.0f;
	frame_cnt++;
	// Compute averages over one second period.
	if ((m_Timer.total_time()-time_elapsed) >= 1.0f) {
		m_Fps = std::to_wstring(frame_cnt);
		// Reset for next average.
		frame_cnt = 0;
		time_elapsed += 1.0f;
	}
}
}
#endif