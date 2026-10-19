#include "loader/memory.h"

#include "internal/memory/memorymanager.h"

#include "loader/loader.h"

LOADER_DLL void* loader_search_memory(std::vector<uint8_t>& bytes)
{
	return internal_scan(bytes);
}

LOADER_DLL void* loader_search_memory_local(std::vector<uint8_t>& bytes, void* local, size_t size)
{
	return internal_scan_local(bytes, local, size);
}

LOADER_DLL void* loader_read_ptr(void* start)
{
	return internal_read_ptr(start);
}

LOADER_DLL void loader_memory_free(void* data)
{
	internal_memory_manager_free(data);
}

LOADER_DLL MemoryManager* loader_get_memory_manager()
{
	return internal_get_memory_manager();
}