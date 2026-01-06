#include "loader/yyc.h"

#include "GMLScriptEnv/room.h"

LOADER_DLL CRoom* loader_get_room_by_index(uint32_t room_idx)
{
	return room::__impl_get_room_by_index(room_idx);
}

LOADER_DLL int loader_yyc_get_funcid(const char* name)
{
	return get_func_info(name).id;
}

LOADER_DLL void* loader_get_yyc_func_ptr(const char* name)
{
	return get_func_info(name).ptr;
}

LOADER_DLL GMLVar* loader_yyc_call_func(const char* name, size_t args_count, GMLVar** args)
{
	uint32_t id = get_func_info(name).id;
	return loader_yyc_call_func(id, args_count, args);
}

LOADER_DLL GMLVar* loader_yyc_call_func(uint32_t id, size_t args_count, GMLVar** args)
{
	return gml_call_func(id, args_count, args);
}