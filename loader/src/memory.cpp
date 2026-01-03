#include "loader/memory.h"

#include "loader/loader.h"

LOADER_DLL void* loader_search_memory(std::vector<uint8_t>& bytes)
{
	return __impl_scan(bytes);
}

LOADER_DLL void* loader_search_memory_local(std::vector<uint8_t>& bytes, void* local, size_t size)
{
	return __impl_scan_local(bytes, local, size);
}

LOADER_DLL void* loader_read_ptr(void* start)
{
	return __impl_read_ptr(start);
}