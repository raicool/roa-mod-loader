#pragma once

#include "GMLScriptEnv/gml.h"

#include "loader.h"

//
// @return gml room struct by index
// 
LOADER_DLL CRoom* loader_get_room_by_index(uint32_t room_idx);

//
// @return a builtin yyc variable struct
//
LOADER_DLL GMLVar* loader_yyc_get_var(const char* name);

LOADER_DLL int loader_yyc_get_funcid(const char* name);

LOADER_DLL void* loader_get_yyc_func_ptr(const char* name);

LOADER_DLL GMLVar* loader_yyc_call_func(const char* name, size_t args_count, GMLVar** args);

LOADER_DLL GMLVar* loader_yyc_call_func(uint32_t id, size_t args_count, GMLVar** args);