#include "init.h"

#include "loader/d3d11_hook.h"

#include "internal/vm.h"
#include "internal/instance.h"
#include "internal/memory/memorymanager.h"

#include <MinHook.h>

bool initialized = false;
int32_t internal_init()
{
	if (initialized == true)
	{
		return InitializationStatusEnum::ALREADY_INITIALIZED;
	}

	if (MH_Initialize() != MH_OK)
	{
		return InitializationStatusEnum::INIT_MINHOOK_FAIL;
	}

	hook_d3d11();
	hook_wndproc();

	if (internal_find_llvm_data() == false)
	{
		return InitializationStatusEnum::FIND_LLVMVARS_FAIL;
	}

	if (internal_find_yysetstring() == false)
	{
		return InitializationStatusEnum::FIND_YYSETSTRING_FAIL;
	}

	if (internal_function_init() == false)
	{
		return InitializationStatusEnum::INIT_FUNCTION_FAIL;
	}

	if (internal_instance_init() == false)
	{
		return InitializationStatusEnum::INIT_INSTANCE_FAIL;
	}

	if (internal_memorymanager_init() == false)
	{
		return InitializationStatusEnum::INIT_MEMORYMANAGER_FAIL;
	}

	initialized = true;
	return InitializationStatusEnum::OKAY;
}