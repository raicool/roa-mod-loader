#pragma once

#include "loader.h"

// dllexport

// 
//
//
int32_t LOADER_DLL loader_hook_create(
	IN void* target_ptr, 
	IN void* detour_ptr, 
	OUT void** original_dptr
);

//
//
//
int32_t LOADER_DLL loader_hook_enable(
	IN void* target_ptr
);

//
//
//
int32_t LOADER_DLL loader_hook_remove(
	IN void* target_ptr
);

//
//
//
int32_t LOADER_DLL loader_hook_disable(
	IN void* target_ptr
);

void log_minhook_status(int32_t status, uint32_t target = 0, uint32_t detour = 0);