#include "loader/d3d11_hook.h"
#include "loader/log.h"

#include <Windows.h>

#include <d3d11.h>
#include <dxgi1_2.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")

#include <memory>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>

HWND g_MainWindow = nullptr;
ID3D11Device* g_pd3dDevice = NULL;
ID3D11DeviceContext* g_pd3dDeviceContext = NULL;
ID3D11RenderTargetView* g_pd3dRenderTarget = NULL;
IDXGISwapChain* g_pSwapChain = NULL;

static void CleanupDeviceD3D11();
static void CleanupRenderTarget();

ImGuiContext* imgui_context = nullptr;

std::vector<dxpresent_callback_func> g_present_callbacks;
std::vector<wndproc_callback_func> g_wndproc_callbacks;

int GetCorrectDXGIFormat(int eCurrentFormat)
{
	switch (eCurrentFormat)
	{
	case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB: return DXGI_FORMAT_R8G8B8A8_UNORM;
	}

	return eCurrentFormat;
}

static bool CreateDeviceD3D11(HWND hWnd)
{
	// Create the D3DDevice
	DXGI_SWAP_CHAIN_DESC swapChainDesc = { };
	swapChainDesc.Windowed = TRUE;
	swapChainDesc.BufferCount = 2;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.OutputWindow = hWnd;
	swapChainDesc.SampleDesc.Count = 1;

	const D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_0,
	};

	HRESULT hr = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_NULL, NULL, 0, featureLevels, 2, D3D11_SDK_VERSION, &swapChainDesc, &g_pSwapChain, &g_pd3dDevice, nullptr, nullptr);
	if (hr != S_OK)
	{
		loader_log_error("D3D11CreateDeviceAndSwapChain() failed. [rv: {}]", (int)hr);
		return false;
	}

	return true;
}

static void CreateRenderTarget(IDXGISwapChain* pSwapChain)
{
	ID3D11Texture2D* pBackBuffer = NULL;
	pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
	if (pBackBuffer)
	{
		DXGI_SWAP_CHAIN_DESC sd;
		pSwapChain->GetDesc(&sd);

		D3D11_RENDER_TARGET_VIEW_DESC desc = { };
		desc.Format = static_cast<DXGI_FORMAT>(GetCorrectDXGIFormat(sd.BufferDesc.Format));
		desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;

		g_pd3dDevice->CreateRenderTargetView(pBackBuffer, &desc, &g_pd3dRenderTarget);
		pBackBuffer->Release();
	}
}

static void Render_DX11(IDXGISwapChain* pSwapChain)
{
	if (!g_pd3dDevice)
	{
		pSwapChain->GetDevice(IID_PPV_ARGS(&g_pd3dDevice));
		g_pd3dDevice->GetImmediateContext(&g_pd3dDeviceContext);
	}

	if (!g_pd3dRenderTarget)
	{
		CreateRenderTarget(pSwapChain);
	}

	if (g_pd3dRenderTarget)
	{
		for (dxpresent_callback_func _callback : g_present_callbacks)
		{
			// these callbacks are mainly for imgui but can really be for anything related to dx11
			_callback(g_pd3dRenderTarget, pSwapChain, g_pd3dDevice, g_pd3dDeviceContext);
		}
	}
}

static std::add_pointer_t<HRESULT WINAPI(IDXGISwapChain*, UINT, UINT)> oPresent;
static HRESULT WINAPI hkPresent(IDXGISwapChain* pSwapChain,
	UINT SyncInterval,
	UINT Flags)
{
	Render_DX11(pSwapChain);

	return oPresent(pSwapChain, SyncInterval, Flags);
}

static std::add_pointer_t<HRESULT WINAPI(IDXGISwapChain*, UINT, UINT, const DXGI_PRESENT_PARAMETERS*)> oPresent1;
static HRESULT WINAPI hkPresent1(IDXGISwapChain* pSwapChain,
	UINT SyncInterval,
	UINT PresentFlags,
	const DXGI_PRESENT_PARAMETERS* pPresentParameters)
{
	Render_DX11(pSwapChain);

	return oPresent1(pSwapChain, SyncInterval, PresentFlags, pPresentParameters);
}

static std::add_pointer_t<HRESULT WINAPI(IDXGISwapChain*, UINT, UINT, UINT, DXGI_FORMAT, UINT)> oResizeBuffers;
static HRESULT WINAPI hkResizeBuffers(IDXGISwapChain* pSwapChain,
	UINT BufferCount,
	UINT Width,
	UINT Height,
	DXGI_FORMAT NewFormat,
	UINT SwapChainFlags)
{
	CleanupRenderTarget();

	return oResizeBuffers(pSwapChain, BufferCount, Width, Height, NewFormat, SwapChainFlags);
}

static std::add_pointer_t<HRESULT WINAPI(IDXGISwapChain*, UINT, UINT, UINT, DXGI_FORMAT, UINT, const UINT*, IUnknown* const*)> oResizeBuffers1;
static HRESULT WINAPI hkResizeBuffers1(IDXGISwapChain* pSwapChain,
	UINT BufferCount,
	UINT Width,
	UINT Height,
	DXGI_FORMAT NewFormat,
	UINT SwapChainFlags,
	const UINT* pCreationNodeMask,
	IUnknown* const* ppPresentQueue)
{
	CleanupRenderTarget();

	return oResizeBuffers1(pSwapChain, BufferCount, Width, Height, NewFormat, SwapChainFlags, pCreationNodeMask, ppPresentQueue);
}

static std::add_pointer_t<HRESULT WINAPI(IDXGIFactory*, IUnknown*, DXGI_SWAP_CHAIN_DESC*, IDXGISwapChain**)> oCreateSwapChain;
static HRESULT WINAPI hkCreateSwapChain(IDXGIFactory* pFactory,
	IUnknown* pDevice,
	DXGI_SWAP_CHAIN_DESC* pDesc,
	IDXGISwapChain** ppSwapChain)
{
	CleanupRenderTarget();

	return oCreateSwapChain(pFactory, pDevice, pDesc, ppSwapChain);
}

static std::add_pointer_t<HRESULT WINAPI(IDXGIFactory*, IUnknown*, HWND, const DXGI_SWAP_CHAIN_DESC1*, const DXGI_SWAP_CHAIN_FULLSCREEN_DESC*, IDXGIOutput*, IDXGISwapChain1**)> oCreateSwapChainForHwnd;
static HRESULT WINAPI hkCreateSwapChainForHwnd(IDXGIFactory* pFactory,
	IUnknown* pDevice,
	HWND hWnd,
	const DXGI_SWAP_CHAIN_DESC1* pDesc,
	const DXGI_SWAP_CHAIN_FULLSCREEN_DESC* pFullscreenDesc,
	IDXGIOutput* pRestrictToOutput,
	IDXGISwapChain1** ppSwapChain)
{
	CleanupRenderTarget();

	return oCreateSwapChainForHwnd(pFactory, pDevice, hWnd, pDesc, pFullscreenDesc, pRestrictToOutput, ppSwapChain);
}

static std::add_pointer_t<HRESULT WINAPI(IDXGIFactory*, IUnknown*, IUnknown*, const DXGI_SWAP_CHAIN_DESC1*, IDXGIOutput*, IDXGISwapChain1**)> oCreateSwapChainForCoreWindow;
static HRESULT WINAPI hkCreateSwapChainForCoreWindow(IDXGIFactory* pFactory,
	IUnknown* pDevice,
	IUnknown* pWindow,
	const DXGI_SWAP_CHAIN_DESC1* pDesc,
	IDXGIOutput* pRestrictToOutput,
	IDXGISwapChain1** ppSwapChain)
{
	CleanupRenderTarget();

	return oCreateSwapChainForCoreWindow(pFactory, pDevice, pWindow, pDesc, pRestrictToOutput, ppSwapChain);
}

static std::add_pointer_t<HRESULT WINAPI(IDXGIFactory*, IUnknown*, const DXGI_SWAP_CHAIN_DESC1*, IDXGIOutput*, IDXGISwapChain1**)> oCreateSwapChainForComposition;
static HRESULT WINAPI hkCreateSwapChainForComposition(IDXGIFactory* pFactory,
	IUnknown* pDevice,
	const DXGI_SWAP_CHAIN_DESC1* pDesc,
	IDXGIOutput* pRestrictToOutput,
	IDXGISwapChain1** ppSwapChain)
{
	CleanupRenderTarget();

	return oCreateSwapChainForComposition(pFactory, pDevice, pDesc, pRestrictToOutput, ppSwapChain);
}

static BOOL CALLBACK EnumWindowsCallback(HWND handle, LPARAM lParam)
{
	const auto isMainWindow = [handle]()
		{
			return GetWindow(handle, GW_OWNER) == nullptr && IsWindowVisible(handle);
		};

	DWORD pID = 0;
	GetWindowThreadProcessId(handle, &pID);

	if (GetCurrentProcessId() != pID || !isMainWindow() || handle == GetConsoleWindow())
	{
		return TRUE;
	}

	*reinterpret_cast<HWND*>(lParam) = handle;

	return FALSE;
}

void hook_d3d11()
{
	EnumWindows(::EnumWindowsCallback, reinterpret_cast<LPARAM>(&g_MainWindow));

	while (!g_MainWindow)
	{
		EnumWindows(::EnumWindowsCallback, reinterpret_cast<LPARAM>(&g_MainWindow));
		loader_log_trace("waiting for window to appear.");
		std::this_thread::sleep_for(std::chrono::milliseconds(200));
	}

	char name[128];
	GetWindowTextA(g_MainWindow, name, RTL_NUMBER_OF(name));
	loader_log_trace(std::format("got window with name : '{}'", name));

	if (!CreateDeviceD3D11(GetConsoleWindow()))
	{
		loader_log_error("CreateDeviceD3D11() failed.");
		return;
	}

	loader_log_trace("DirectX11: g_pd3dDevice: {:08x}", (int)g_pd3dDevice);
	loader_log_trace("DirectX11: g_pSwapChain: {:08x}", (int)g_pSwapChain);

	if (g_pd3dDevice)
	{
		if (ImGui::GetCurrentContext())
			return;

		imgui_context = ImGui::CreateContext();
		
		ImGui_ImplWin32_Init(g_MainWindow);

		// Hook
		IDXGIDevice* pDXGIDevice = NULL;
		g_pd3dDevice->QueryInterface(IID_PPV_ARGS(&pDXGIDevice));

		IDXGIAdapter* pDXGIAdapter = NULL;
		pDXGIDevice->GetAdapter(&pDXGIAdapter);

		IDXGIFactory* pIDXGIFactory = NULL;
		pDXGIAdapter->GetParent(IID_PPV_ARGS(&pIDXGIFactory));

		if (!pIDXGIFactory)
		{
			loader_log_error("pIDXGIFactory is NULL.\n");
			return;
		}

		pIDXGIFactory->Release();
		pDXGIAdapter->Release();
		pDXGIDevice->Release();

		void** pVTable = *reinterpret_cast<void***>(g_pSwapChain);
		void** pFactoryVTable = *reinterpret_cast<void***>(pIDXGIFactory);

		void* fnCreateSwapChain = pFactoryVTable[10];
		void* fnCreateSwapChainForHwndChain = pFactoryVTable[15];
		void* fnCreateSwapChainForCWindowChain = pFactoryVTable[16];
		void* fnCreateSwapChainForCompChain = pFactoryVTable[24];

		void* fnPresent = pVTable[8];
		void* fnPresent1 = pVTable[22];

		void* fnResizeBuffers = pVTable[13];
		void* fnResizeBuffers1 = pVTable[39];

		CleanupDeviceD3D11();

		static int32_t cscStatus = loader_hook_create(reinterpret_cast<void**>(fnCreateSwapChain), &hkCreateSwapChain, reinterpret_cast<void**>(&oCreateSwapChain));
		static int32_t cschStatus = loader_hook_create(reinterpret_cast<void**>(fnCreateSwapChainForHwndChain), &hkCreateSwapChainForHwnd, reinterpret_cast<void**>(&oCreateSwapChainForHwnd));
		static int32_t csccwStatus = loader_hook_create(reinterpret_cast<void**>(fnCreateSwapChainForCWindowChain), &hkCreateSwapChainForCoreWindow, reinterpret_cast<void**>(&oCreateSwapChainForCoreWindow));
		static int32_t csccStatus = loader_hook_create(reinterpret_cast<void**>(fnCreateSwapChainForCompChain), &hkCreateSwapChainForComposition, reinterpret_cast<void**>(&oCreateSwapChainForComposition));

		static int32_t presentStatus = loader_hook_create(reinterpret_cast<void**>(fnPresent), &hkPresent, reinterpret_cast<void**>(&oPresent));
		static int32_t present1Status = loader_hook_create(reinterpret_cast<void**>(fnPresent1), &hkPresent1, reinterpret_cast<void**>(&oPresent1));

		static int32_t resizeStatus = loader_hook_create(reinterpret_cast<void**>(fnResizeBuffers), &hkResizeBuffers, reinterpret_cast<void**>(&oResizeBuffers));
		static int32_t resize1Status = loader_hook_create(reinterpret_cast<void**>(fnResizeBuffers1), &hkResizeBuffers1, reinterpret_cast<void**>(&oResizeBuffers1));

		loader_hook_enable(fnCreateSwapChain);
		loader_hook_enable(fnCreateSwapChainForHwndChain);
		loader_hook_enable(fnCreateSwapChainForCWindowChain);
		loader_hook_enable(fnCreateSwapChainForCompChain);

		loader_hook_enable(fnPresent);
		loader_hook_enable(fnPresent1);

		loader_hook_enable(fnResizeBuffers);
		loader_hook_enable(fnResizeBuffers1);
	}
}

static WNDPROC oWndProc;
LRESULT WINAPI WndProc(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	for (wndproc_callback_func _callback : g_wndproc_callbacks)
	{
		_callback(hWnd, uMsg, wParam, lParam);
	}

	return CallWindowProc(oWndProc, hWnd, uMsg, wParam, lParam);
}

void hook_wndproc()
{
	oWndProc = reinterpret_cast<WNDPROC>(SetWindowLongPtr(g_MainWindow, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(WndProc)));
}

void unhook_d3d11()
{
	if (ImGui::GetCurrentContext())
	{
		if (ImGui::GetIO().BackendRendererUserData)
			ImGui_ImplDX11_Shutdown();

		if (ImGui::GetIO().BackendPlatformUserData)
			ImGui_ImplWin32_Shutdown();

		ImGui::DestroyContext();
	}

	CleanupDeviceD3D11();
}

static void CleanupRenderTarget()
{
	if (g_pd3dRenderTarget)
	{
		g_pd3dRenderTarget->Release();
		g_pd3dRenderTarget = NULL;
	}
}

static void CleanupDeviceD3D11()
{
	CleanupRenderTarget();

	if (g_pSwapChain)
	{
		g_pSwapChain->Release();
		g_pSwapChain = NULL;
	}
	if (g_pd3dDevice)
	{
		g_pd3dDevice->Release();
		g_pd3dDevice = NULL;
	}
	if (g_pd3dDeviceContext)
	{
		g_pd3dDeviceContext->Release();
		g_pd3dDeviceContext = NULL;
	}
}

LOADER_DLL void loader_add_present_callback(dxpresent_callback_func callback)
{
	g_present_callbacks.emplace_back(callback);
}

LOADER_DLL void loader_remove_present_callback(dxpresent_callback_func callback)
{
	auto idx = std::find(
		g_present_callbacks.begin(),
		g_present_callbacks.end(),
		callback);

	if (idx != g_present_callbacks.end())
	{
		// found callback
		g_present_callbacks.erase(idx);
	}
}

LOADER_DLL void loader_add_wndproc_callback(wndproc_callback_func callback)
{
	g_wndproc_callbacks.emplace_back(callback);
}

LOADER_DLL void loader_remove_wndproc_callback(wndproc_callback_func callback)
{
	auto idx = std::find(
		g_wndproc_callbacks.begin(),
		g_wndproc_callbacks.end(),
		callback);

	if (idx != g_wndproc_callbacks.end())
	{
		// found callback
		g_wndproc_callbacks.erase(idx);
	}
}

LOADER_DLL HWND loader_get_window()
{
	return g_MainWindow;
}

LOADER_DLL ImGuiContext* loader_get_imgui_context()
{
	return imgui_context;
}

LOADER_DLL ID3D11Device* loader_get_d3d_device()
{
	return g_pd3dDevice;
}

LOADER_DLL ID3D11DeviceContext* loader_get_d3d_device_context()
{
	return g_pd3dDeviceContext;
}

LOADER_DLL ID3D11RenderTargetView* loader_get_d3d_render_target()
{
	return g_pd3dRenderTarget;
}

LOADER_DLL IDXGISwapChain* loader_get_dx_swapchain()
{
	return g_pSwapChain;
}