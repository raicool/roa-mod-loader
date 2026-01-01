#include "loader/yyc.h"

#include "GMLScriptEnv/RoomHelper.h"

LOADER_DLL CRoom* get_room_by_index(uint32_t room_idx)
{
	return RoomHelper::__impl_get_room_by_index(room_idx);
}