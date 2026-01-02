#pragma once

#include "loader.h"

#include <d3d11.h>
#include <imgui.h>

void hook_d3d11();
void unhook_d3d11();

static ImGuiContext* imgui_context = nullptr;
LOADER_DLL ImGuiContext* loader_get_imgui_context();
LOADER_DLL ID3D11Device* get_d3d_device();
LOADER_DLL ID3D11DeviceContext* get_d3d_device_context();
LOADER_DLL ID3D11RenderTargetView* get_d3d_render_target();
LOADER_DLL IDXGISwapChain* get_dx_swapchain();