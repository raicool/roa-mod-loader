#pragma once

#include "loader.h"

// wrapper funcs for minhook
int32_t LOADER_DLL loader_hook_create(void* target_ptr, void* detour_ptr, void** original_dptr);
int32_t LOADER_DLL loader_hook_enable(void* target_ptr);
int32_t LOADER_DLL loader_hook_remove(void* target_ptr);
int32_t LOADER_DLL loader_hook_disable(void* target_ptr);

void log_minhook_status(int32_t status, uint32_t target = 0, uint32_t detour = 0);