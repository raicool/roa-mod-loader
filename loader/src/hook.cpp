#include "loader/hook.h"

#include <MinHook.h>

#include <format>

int32_t LOADER_DLL loader_hook_create(LPVOID target_ptr, LPVOID detour_ptr, LPVOID* original_dptr)
{
	const MH_STATUS status = MH_CreateHook(target_ptr, detour_ptr, original_dptr);
	if (status != MH_STATUS::MH_OK)
	{
		log_minhook_status(status, (uint32_t)target_ptr, (uint32_t)detour_ptr);
	}
	return status;
}

int32_t LOADER_DLL loader_hook_enable(LPVOID target_ptr)
{
	if (target_ptr == MH_ALL_HOOKS)
	{
		loader_log_error("loader_hook_enable(): param MH_ALL_HOOKS/nullptr is not allowed");
		return MH_STATUS::MH_UNKNOWN;
	}

	const MH_STATUS status = MH_EnableHook(target_ptr);
	if (status != MH_STATUS::MH_OK)
	{
		log_minhook_status(status, (uint32_t)target_ptr);
	}
	return status;
}

int32_t LOADER_DLL loader_hook_remove(LPVOID target_ptr)
{
	const MH_STATUS status = MH_RemoveHook(target_ptr);
	if (status != MH_STATUS::MH_OK)
	{
		log_minhook_status(status, (uint32_t)target_ptr);
	}
	return status;
}

int32_t LOADER_DLL loader_hook_disable(LPVOID target_ptr)
{
	if (target_ptr == MH_ALL_HOOKS)
	{
		loader_log_error("loader_hook_disable(): param MH_ALL_HOOKS/nullptr is not allowed");
		return MH_STATUS::MH_UNKNOWN;
	}

	const MH_STATUS status = MH_DisableHook(target_ptr);
	if (status != MH_STATUS::MH_OK)
	{
		log_minhook_status(status, (uint32_t)target_ptr);
	}
	return status;
}

void log_minhook_status(int32_t status, uint32_t target, uint32_t detour)
{
	std::string err_string = "";

	switch (status)
	{
		case (MH_STATUS::MH_ERROR_ALREADY_INITIALIZED):
			err_string.append(std::format("hook at {:08x} is already initialized", target)); break;
		case (MH_STATUS::MH_ERROR_NOT_INITIALIZED):
			err_string.append("minhook is not initialized"); break;
		case (MH_STATUS::MH_ERROR_ALREADY_CREATED):
			err_string.append(std::format("hook at {:08x} is already created", target)); break;
		case (MH_STATUS::MH_ERROR_NOT_CREATED):
			err_string.append(std::format("hook at {:08x} is not created yet", target)); break;
		case (MH_STATUS::MH_ERROR_ENABLED):
			err_string.append(std::format("hook at {:08x} is already enabled", target)); break;
		case (MH_STATUS::MH_ERROR_DISABLED):
			err_string.append(std::format("hook at {:08x} is already disabled", target)); break;
		case (MH_STATUS::MH_ERROR_NOT_EXECUTABLE):
			err_string.append(std::format("detour ptr at {:08x} points to invalid memory", detour)); break;
		case (MH_STATUS::MH_ERROR_UNSUPPORTED_FUNCTION):
			err_string.append(std::format("hook at {:08x} cannot be hooked (MH_ERROR_UNSUPPORTED_FUNCTION)", target)); break;
		case (MH_STATUS::MH_ERROR_MEMORY_ALLOC):
			err_string.append(std::format("failed to allocate memory for hook at {:08x}", target)); break;
		case (MH_STATUS::MH_ERROR_MEMORY_PROTECT):
			err_string.append(std::format("failed to change memory protection for hook at {:08x}", target)); break;
		case (MH_STATUS::MH_ERROR_MODULE_NOT_FOUND):
			err_string.append(std::format("hook at {:08x} (MH_ERROR_MODULE_NOT_FOUND)", target)); break;
		case (MH_STATUS::MH_ERROR_FUNCTION_NOT_FOUND):
			err_string.append(std::format("hook at {:08x} function could not be found", target)); break;
	}

	loader_log_warn(err_string);
}

