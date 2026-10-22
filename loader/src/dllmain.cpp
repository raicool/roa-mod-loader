#include "loader/yyc.h"
#include "loader/log.h"
#include "loader/d3d11_hook.h"
#include "loader/info.h"

#include "internal/init.h"
#include "internal/event.h"

#include "MinHook.h"

#include <fstream>
#include <filesystem>
#include <string>
#include <winternl.h>

constexpr int library_load_flags = DONT_RESOLVE_DLL_REFERENCES;

extern loader_info g_loader_init_info;

typedef NTSTATUS(NTAPI* pLdrLoadDll) (
	PWCHAR PathToFile,
	ULONG Flags,
	PUNICODE_STRING ModuleFileName,
	PHANDLE ModuleHandle
	);

typedef VOID(NTAPI* pRtlInitUnicodeString)(PUNICODE_STRING DestinationString, PCWSTR SourceString);

HMODULE loader_load_library(LPCWSTR lpFileName) 
{
	UNICODE_STRING ustrModule;
	HANDLE hModule = NULL;

	HMODULE hNtdll = GetModuleHandle("ntdll.dll");
	pRtlInitUnicodeString RtlInitUnicodeString = (pRtlInitUnicodeString)GetProcAddress(hNtdll, "RtlInitUnicodeString");

	RtlInitUnicodeString(&ustrModule, lpFileName);

	pLdrLoadDll LdrLoadDll = (pLdrLoadDll)GetProcAddress(GetModuleHandle("ntdll.dll"), "LdrLoadDll");
	if (!LdrLoadDll) 
	{
		return NULL;
	}

	NTSTATUS status = LdrLoadDll(NULL, 0, &ustrModule, &hModule);
	return (HMODULE)hModule;
}

DWORD WINAPI loader_initialize(LPVOID hModule)
{
#if _LOADER_ALLOCATE_CONSOLE
	AllocConsole();

	SetConsoleTitleA("loader");

	freopen_s((FILE**)stdin, "conin$", "r", stdin);
	freopen_s((FILE**)stdout, "conout$", "w", stdout);
#endif

	loader_mod& internal_mod = loader_mod::get();

	internal_mod.name = "roa-mod-loader";
	internal_mod.description = "internal mod loader";
	internal_mod.is_loaded = true;

	logger_init();

	loader_log_trace("\n"
		"------------------------------------------------------------------------------------\n"
		" Rivals of Aether Mod Loader\n"
		" Version: {}\n"
		" Commit Hash: {:08x}\n"
		" Build Type: {}\n"
		" Any crashes while this mod loader is enabled should NOT be reported to developers,\n"
		" as it most likely is caused by the mod loader and not the game itself.\n"
		"------------------------------------------------------------------------------------\n",
		loader_initialization_get_version(&g_loader_init_info),
		loader_initialization_get_commit_hash(&g_loader_init_info),
		loader_initialization_buildtype_string(&g_loader_init_info)
	);

	uint8_t result = internal_init();
	
	if (result != InitializationStatusEnum::OKAY)
	{
		loader_log_fatal("----------------!!!!!----------------");
		loader_log_fatal("failed to initialize roa-mod-loader!!\n\terror code: 0x{:08x}", result);
		loader_log_fatal("----------------!!!!!----------------");
		loader_log_warn("loader console will stay open but roa-mod-loader will be unloaded.");
#if DEBUG
		__debugbreak();
#endif
		return FALSE;
	}
	else
	{
		internal_mod.is_loaded = true;
		loader_log_info("welcome to roa-mod-loader version {}", loader_initialization_get_version(&g_loader_init_info));
	}

	hook_d3d11();
	hook_wndproc();

	char module_path[128];
	GetModuleFileName(0, module_path, 128);

	const std::string module_folder = std::filesystem::path(module_path).parent_path().string();
	const std::string mods_folder = std::format("{}/{}", module_folder, _LOADER_MODS_DIRECTORY);

	std::vector<loader_init_func> registry;

	for (auto& entry : std::filesystem::directory_iterator(mods_folder))
	{
		if (entry.is_directory()) continue;

		std::filesystem::path path = entry.path();

		if (path.extension() != ".dll")
		{
			continue;
		}

		HMODULE mod = loader_load_library(path.generic_wstring().data());

		if (mod == nullptr)
		{
			loader_log_error("error trying to load mod {}", path.generic_string().data());
			continue;
		}
		else
		{
			loader_init_func mod_init_func = (loader_init_func)GetProcAddress(mod, "?initialize@@YAAAVloader_mod@@PAUloader_info@@@Z");

			if (mod_init_func == nullptr)
			{
				// try C style mangled name
				mod_init_func = (loader_init_func)GetProcAddress(mod, "_initialize");
			}

			if (mod_init_func)
			{
				registry.emplace_back(mod_init_func);
			}
			else
			{
				FreeLibraryAndExitThread(mod, 0);
			}
		}
	}

	for (loader_init_func init_func : registry)
	{
		init_func(&g_loader_init_info);
	}

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
		//DisableThreadLibraryCalls(hModule);

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