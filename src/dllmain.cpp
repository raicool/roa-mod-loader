#include "pch.h"

#include "loader/log.h"

#include <filesystem>


#define _LOADER_MODS_DIRECTORY "mods/"
#define _LOADER_ALLOCATE_CONSOLE 1

DWORD WINAPI loader_initialize(LPVOID hModule)
{
#if _LOADER_ALLOCATE_CONSOLE
	AllocConsole();

	SetConsoleTitleA("loader");

	freopen_s((FILE**)stdin, "conin$", "r", stdin);
	freopen_s((FILE**)stdout, "conout$", "w", stdout);
#endif
	logger_init();
	if (std::filesystem::is_directory(_LOADER_MODS_DIRECTORY) == false)
	{
		loader_log_warn("no mods directory found, creating.");

		std::filesystem::create_directory(_LOADER_MODS_DIRECTORY);
	}

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