#pragma once

#include "loader.h"


LOADER_DLL RValue loader_get_instance_variable(
	IN RValue instance,
	IN const char* name
);

LOADER_DLL int32_t loader_get_instance_variable_count(
	IN RValue instance
);

LOADER_DLL RefDynamicArrayOfRValue* loader_get_instance_variable_names(
	RValue instance
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