#pragma once

#include "loader.h"

// wrapper funcs for minhook

MH_STATUS LOADER_DLL loader_hook_create(LPVOID target_ptr, LPVOID detour_ptr, LPVOID* original_dptr);
MH_STATUS LOADER_DLL loader_hook_enable(LPVOID target_ptr);
MH_STATUS LOADER_DLL loader_hook_remove(LPVOID target_ptr);
MH_STATUS LOADER_DLL loader_hook_disable(LPVOID target_ptr);

void log_minhook_status(MH_STATUS status, uint32_t target = 0, uint32_t detour = 0);