#include "loader/yyc.h"
#include "loader/log.h"
#include "loader/d3d11_hook.h"

#include "internal/init.h"
#include "internal/event.h"

#include "MinHook.h"

#include <fstream>
#include <filesystem>

DWORD WINAPI loader_initialize(LPVOID hModule)
{
#if _LOADER_ALLOCATE_CONSOLE
	AllocConsole();

	SetConsoleTitleA("loader");

	freopen_s((FILE**)stdin, "conin$", "r", stdin);
	freopen_s((FILE**)stdout, "conout$", "w", stdout);
#endif

	logger_init();

	loader_log_trace(std::format("\n"
		"------------------------------------------------------------------------------------\n"
		" Rivals of Aether Mod Loader\n"
		" Version: {}\n"
		" Commit Hash: {}\n"
		" Build Type: {}\n"
		" Any crashes while this mod loader is enabled should NOT be reported to developers,\n"
		" as it most likely is caused by the mod loader and not the game itself.\n"
		"------------------------------------------------------------------------------------\n",
		LOADER_VERSION,
		GIT_COMMIT_HASH,
		DEBUG ? "DEBUG" : "RELEASE"
	));

	uint8_t result = internal_init();
	
	if (result != InitializationStatusEnum::OKAY)
	{
		loader_log_fatal("failed to initialize roa-mod-loader!\n\terror code: 0x{:08x}", result);
#if DEBUG
		__debugbreak();
#endif
		return FALSE;
	}
	else
	{
		loader_log_info("initialized roa-mod-loader");
	}

	hook_d3d11();
	hook_wndproc();

	char module_path[128];
	GetModuleFileName(0, module_path, 128);
	const std::string module_folder = std::filesystem::path(module_path).parent_path().string();
	const std::string mods_folder = std::format("{}/{}", module_folder, _LOADER_MODS_DIRECTORY);

	HMODULE mod;
	for (auto& entry : std::filesystem::directory_iterator(mods_folder))
	{
		if (entry.is_directory()) continue;

		std::filesystem::path path = entry.path();
		if (path.extension() == ".dll")
		{
			mod = LoadLibraryA(path.generic_string().c_str());
			if (mod)
			{
				loader_log_info(std::format("{} loaded", path.generic_string()));
			}
		}
	}

	loader_log_info("Loader has initialized");
	return TRUE;
}

void loader_shutdown()
{
	logger_shutdown();

#if _LOADER_ALLOCATE_CONSOLE
	fclose(stdin);
	fclose(stdout);

	FreeConsole();
#endif
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD fdwReason, LPVOID lpReserved)
{
	if (fdwReason == DLL_PROCESS_ATTACH)
	{
		DisableThreadLibraryCalls(hModule);

		HANDLE hHandle = CreateThread(NULL, 0, loader_initialize, hModule, 0, NULL);
		if (hHandle != NULL)
		{
			CloseHandle(hHandle);
		}
	}

	else if (fdwReason == DLL_PROCESS_DETACH && !lpReserved)
	{
		loader_shutdown();
		FreeLibraryAndExitThread(hModule, 0);
	}

	return TRUE;
}