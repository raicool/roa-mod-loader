#pragma once

struct MemoryManager;

bool internal_memorymanager_init();

MemoryManager* internal_get_memory_manager();

void internal_memory_manager_free(void* data);