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

	loader_log_info("Initializing MinHook");
	if (MH_Initialize() != MH_OK)
	{
		return InitializationStatusEnum::INIT_MINHOOK_FAIL;
	}
	

	loader_log_info("Initializing LLVM hook");
	{
		logger_nestscope nest;

		if (internal_find_llvm_data() == false)
		{
			return InitializationStatusEnum::FIND_LLVMVARS_FAIL;
		}

		if (internal_find_yysetstring() == false)
		{
			return InitializationStatusEnum::FIND_YYSETSTRING_FAIL;
		}

		if (internal_find_run_endgame() == false)
		{
			return InitializationStatusEnum::FIND_RUNENDGAME_FAIL;
		}

		if (internal_function_init() == false)
		{
			return InitializationStatusEnum::INIT_FUNCTION_FAIL;
		}
	}
	
	loader_log_info("Initializing instance hook");
	{
		logger_nestscope nest;

		if (internal_instance_init() == false)
		{
			return InitializationStatusEnum::INIT_INSTANCE_FAIL;
		}
	}

	loader_log_info("Initializing MemoryManager hook");
	{
		logger_nestscope nest;

		if (internal_memorymanager_init() == false)
		{
			return InitializationStatusEnum::INIT_MEMORYMANAGER_FAIL;
		}
	}

	loader_log_info("Initializing d3d11 hook");
	{
		logger_nestscope nest;

		hook_d3d11();
		hook_wndproc();
	}

	initialized = true;
	return InitializationStatusEnum::OKAY;
}