#pragma once

#include "loader.h"

#include <d3d11.h>
#include <imgui.h>

LOADER_DLL ImGuiContext* loader_get_imgui_context();

typedef void (*dxpresent_callback_func)(ID3D11RenderTargetView*, IDXGISwapChain*, ID3D11Device*, ID3D11DeviceContext*);
typedef void (*wndproc_callback_func)(const HWND, UINT, WPARAM, LPARAM);

//
// adds a function to be called when d3d frame is being presented
//
LOADER_DLL void loader_add_present_callback(
	IN dxpresent_callback_func callback
);

LOADER_DLL void loader_remove_present_callback(
	IN dxpresent_callback_func callback
);

//
//
//
LOADER_DLL void loader_add_wndproc_callback(
	IN wndproc_callback_func callback
);

LOADER_DLL void loader_remove_wndproc_callback(
	IN wndproc_callback_func callback
);


//
// @return pointer to rivals' process window
//
LOADER_DLL HWND loader_get_window();

//
// @return pointer to the current d3d11 device of rivals's main window
//
LOADER_DLL ID3D11Device* loader_get_d3d_device();

//
// @return pointer to the current d3d11 device context of rivals's main window
//
LOADER_DLL ID3D11DeviceContext* loader_get_d3d_device_context();

//
// @return pointer to the current d3d11 render target of rivals's main window
//
LOADER_DLL ID3D11RenderTargetView* loader_get_d3d_render_target();

//
// @return pointer to the current swapchain of rivals's main window
//
LOADER_DLL IDXGISwapChain* loader_get_dx_swapchain();

void hook_d3d11();
void hook_wndproc();
void unhook_d3d11();