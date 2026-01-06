#include "loader/yyc.h"
#include "loader/log.h"
#include "loader/d3d11_hook.h"

#include "GMLScriptEnv/event.h"
#include "GMLScriptEnv/gml.h"
#include "GMLScriptEnv/resources.h"
#include "GMLScriptEnv/room.h"

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
		" Build Type: {}\n"
		" Any crashes while this mod loader is enabled should NOT be reported to developers,\n"
		" as it most likely is caused by the mod loader and not the game itself.\n"
		"------------------------------------------------------------------------------------\n",
		LOADER_VERSION,
		DEBUG ? "DEBUG" : "RELEASE"
	));

	std::string result = __gml_setup();
	
	if (result != "")
	{
		loader_log_fatal("failed to initialize YYCHook");
	}
	else
	{
		loader_log_info("initialized YYCHook");
	}

	room::__setup();
	if (MH_Initialize() == MH_OK)
	{
		loader_log_info("minhook successfully initialized");
	}

	hook_d3d11();
	hook_wndproc();

	HMODULE mod;
	for (auto entry : std::filesystem::directory_iterator(_LOADER_MODS_DIRECTORY))
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