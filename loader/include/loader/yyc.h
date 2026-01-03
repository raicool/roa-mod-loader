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
LOADER_DLL GMLVar* loader_get_yyc_var(const char* name);