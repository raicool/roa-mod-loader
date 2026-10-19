#pragma once

#include <stdint.h>

enum InitializationStatusEnum
{
	OKAY,
	ALREADY_INITIALIZED,
	INIT_MINHOOK_FAIL,
	INIT_FUNCTION_FAIL,
	INIT_INSTANCE_FAIL,
	INIT_MEMORYMANAGER_FAIL,
	ERROR_GENERIC,
};

int32_t internal_init();