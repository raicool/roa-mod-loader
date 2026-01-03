#pragma once

#include "loader.h"

#include "GMLScriptEnv/search.h"

// dllexport

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