#include "loader/yyc.h"

#include "GMLScriptEnv/room.h"

LOADER_DLL CRoom* loader_get_room_by_index(uint32_t room_idx)
{
	return room::__impl_get_room_by_index(room_idx);
}