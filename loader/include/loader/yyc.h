#pragma once

#include "GMLScriptEnv/gml.h"

#include "loader.h"

// returns gml room struct by index
LOADER_DLL CRoom* get_room_by_index(uint32_t room_idx);

LOADER_DLL GMLVar* get_yyc_var(const char* name);