#pragma once

#include "loader.h"

#include "internal/search.h"

#include "loader/yoyo.h"

//
// seaches for a byte pattern inside of rivals' executable/rw memory
// 
// @return pointer to the location of the matched byte pattern
// @return returns nullptr if no matching byte pattern is found
//
LOADER_DLL void* loader_search_memory(
	IN std::vector<uint8_t>& bytes
);

//
// seaches for a byte pattern within a specified region in rivals' executable/rw memory
//
// @return pointer to the location of the matched byte pattern
// @return returns nullptr if no matching byte pattern is found
//
LOADER_DLL void* loader_search_memory_local(
	IN std::vector<uint8_t>& bytes,
	IN void* local,
	IN size_t size
);

LOADER_DLL void* loader_read_ptr(
	IN void* start
);

// @return YYC static MemoryManager struct
LOADER_DLL MemoryManager* loader_get_memory_manager();

// calls GameMaker's MemoryManager::Free on a value
LOADER_DLL void loader_memory_free(void* data);