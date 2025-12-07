#pragma once

#include "loader.h"

// wrapper funcs for minhook

int32_t LOADER_DLL loader_hook_create(LPVOID target_ptr, LPVOID detour_ptr, LPVOID* original_dptr);
int32_t LOADER_DLL loader_hook_enable(LPVOID target_ptr);
int32_t LOADER_DLL loader_hook_remove(LPVOID target_ptr);
int32_t LOADER_DLL loader_hook_disable(LPVOID target_ptr);

void log_minhook_status(int32_t status, uint32_t target = 0, uint32_t detour = 0);