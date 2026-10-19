#include "internal/memory/memorymanager.h"

#include "internal/search.h"
#include "internal/utils.h"

#include "loader/log.h"
#include "loader/yoyo.h"

#include <vector>

MemoryManager* g_MemoryManager = nullptr;
void* g_MemoryMutex = nullptr;

void(*MemoryManager__Free)(void* data) = nullptr;

bool internal_memorymanager_init()
{
	std::vector<uint8_t> Alloc_pattern =
	{
		0x8b, 0x0d, '?', '?', '?', '?',
		0x56,
		0x51,
		0x8b, 0x32,
		0x51,
		0x68, '?', '?', '?', '?',
		0x52,
		0xff, 0x56, 0x08,
		0x57,
		0x53,
		0x68, '?', '?', '?', '?',
		0x8d, 0x85, 0xfc, 0xfb, 0xff, 0xff
	};

	void* Alloc_address = internal_scan(Alloc_pattern);
	if (Alloc_address)
	{
		g_MemoryManager = *(MemoryManager**)INTERNAL_INCR_PTR(Alloc_address, 2);
		loader_log_info("internal_memorymanager_init() : MemoryManager: 0x{:08x}", (int)g_MemoryManager);
	}
	else
	{
		loader_log_error("internal_memorymanager_init(): could not find MemoryManager::Alloc() subroutine");
		return false;
	}

	std::vector<uint8_t> ReAlloc_pattern =
	{
		0x8b, 0x0d, '?', '?', '?', '?',
		0x85, 0xc9,
		0x75, '?',
		0x6a, 0x04,
		0xe8, '?', '?', '?', '?',
		0x83, 0xc4, 0x04,
		0xa3, '?', '?', '?', '?',
		0x8b, 0xc8,
		0x68,
	};

	void* ReAlloc_address = internal_scan_local(ReAlloc_pattern, Alloc_address, 0x1000);
	if (ReAlloc_address)
	{
		g_MemoryMutex = *(std::mutex***)INTERNAL_INCR_PTR(ReAlloc_address, 2);
		loader_log_info("internal_memorymanager_init() : g_MemoryMutex: 0x{:08x}", (int)g_MemoryMutex);
	}
	else
	{
		loader_log_error("internal_memorymanager_init(): could not find g_MemoryMutex");
		return false;
	}

	std::vector<uint8_t> Free_pattern =
	{
		0x55,
		0x8b, 0xec,
		0x56,
		0x8b, 0x75, 0x08,
		0x85, 0xf6,
		0x0f, 0x84, '?', '?', '?', '?',
		0x8b, 0x0d, '?', '?', '?', '?',
		0x85, 0xc9,
		0x75, '?',
		0x6a, 0x04,
		0xe8, '?', '?', '?', '?',
		0x83, 0xc4, 0x04,
		0xa3, '?', '?', '?', '?',
		0x8b, 0xc8,
		0x68, '?', '?', '?', '?',
		0xe8, '?', '?', '?', '?',
		0x8b, 0x0d, '?', '?', '?', '?'
	};

	void* Free_address = internal_scan_local(Free_pattern, Alloc_address, 0x1000);
	if (Free_address)
	{
		MemoryManager__Free = (void(*)(void*)) Free_address;
		loader_log_info("internal_memorymanager_init() : MemoryManager::Free: 0x{:08x}", (int)MemoryManager__Free);
	}
	else
	{
		loader_log_error("internal_memorymanager_init(): could not find MemoryManager::Free");
		return false;
	}

	return true;
}

void internal_memory_manager_free(void* data)
{
	return MemoryManager__Free(data);
}

MemoryManager* internal_get_memory_manager()
{
	return g_MemoryManager;
}