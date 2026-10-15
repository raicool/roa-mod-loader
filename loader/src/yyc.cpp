#include "loader/yyc.h"

#include "Internal/function.h"

LOADER_DLL int loader_yyc_get_funcid(const char* name)
{
	return internal_get_func_info(name).id;
}

LOADER_DLL void* loader_get_yyc_func_ptr(const char* name)
{
	return internal_get_func_info(name).ptr;
}

LOADER_DLL RValue* loader_yyc_call_func(const char* name, size_t args_count, RValue* args)
{
	uint32_t id = internal_get_func_info(name).id;
	return loader_yyc_call_func(id, args_count, args);
}

LOADER_DLL RValue* loader_yyc_call_func(uint32_t id, size_t args_count, RValue* args)
{
	return internal_call_gml_function(id, args_count, args);
}