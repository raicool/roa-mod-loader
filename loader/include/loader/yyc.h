#pragma once

#include "loader.h"


//
// @return gml room struct by index
// 
LOADER_DLL CRoom* loader_get_room_by_index(
	IN uint32_t room_idx
);

//
// @return a builtin yyc variable struct
//
LOADER_DLL RValue* loader_yyc_get_var(
	IN const char* name
);
LOADER_DLL int loader_yyc_get_funcid(
	IN const char* name
);

LOADER_DLL void* loader_get_yyc_func_ptr(
	IN const char* name
);
LOADER_DLL RValue* loader_yyc_call_func(
	IN const char* name,
	IN size_t args_count,
	IN RValue* args
);
LOADER_DLL RValue* loader_yyc_call_func(
	IN uint32_t id,
	IN size_t args_count,
	IN RValue* args
);