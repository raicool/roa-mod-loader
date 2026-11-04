#include "dllmain.h"

#include "loader/yyc.h"
#include "loader/log.h"
#include "loader/d3d11_hook.h"
#include "loader/info.h"

#include "internal/init.h"
#include "internal/shutdown.h"
#include "internal/event.h"
#include "internal/ldrload.h"

#include "MinHook.h"

#include <fstream>
#include <filesystem>
#include <string>
#include <internal/vm.h>

extern loader_info g_loader_init_info;

std::vector<loader_mod*> g_loaded_mods;

DWORD WINAPI loader_initialize(LPVOID hModule)
{
	auto begin = std::chrono::high_resolution_clock::now();

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
	internal_mod.handle = static_cast<HMODULE>(hModule);

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
		loader_get_version(&g_loader_init_info),
		loader_get_commit_hash(&g_loader_init_info),
		loader_get_buildtype_string(&g_loader_init_info)
	);

	loader_set_logger_scope("Loader Init");

	uint8_t result = internal_init();
	
	if (result != InitializationStatusEnum::OKAY)
	{
		loader_log_fatal("----------------!!!!!----------------");
		loader_log_fatal("failed to initialize roa-mod-loader!!");
		loader_log_fatal("error code : 0x{:08x}", result);
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
		loader_log_info("welcome to roa-mod-loader version {}", loader_get_version(&g_loader_init_info));
	}

	char module_path[128];
	GetModuleFileName(0, module_path, 128);

	const std::string module_folder = std::filesystem::path(module_path).parent_path().string();
	const std::string mods_folder = std::format("{}/{}", module_folder, _LOADER_MODS_DIRECTORY);

	std::vector<std::pair<HMODULE, loader_init_func>> registry;

	loader_set_logger_scope("Mod Initialization");

	for (auto& entry : std::filesystem::directory_iterator(mods_folder))
	{
		logger_nestscope nest;

		if (entry.is_directory()) continue;

		std::filesystem::path path = entry.path();

		if (path.extension() != ".dll")
		{
			continue;
		}

		HMODULE mod = internal_load_library(path.generic_wstring().data());

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
				registry.emplace_back(std::make_pair(mod, mod_init_func));
			}
			else
			{
				FreeLibraryAndExitThread(mod, 0);
			}
		}
	}

	loader_log_info("Initializing Mods...");
	{
		logger_nestscope nest;

		loader_mod* mod = nullptr;

		for (auto& this_registry : registry)
		{
			{
				logger_nestscope nest;

				mod = this_registry.second(&g_loader_init_info);
				mod->handle = this_registry.first;
			}

			g_loaded_mods.emplace_back(mod);

			loader_log_info("{} loaded.", mod->name);
		}
	}

	loader_set_logger_scope("Loader Init");

	loader_log_info("loaded {} mods", g_loaded_mods.size());

	auto end = std::chrono::high_resolution_clock::now();
	auto time = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
	loader_log_info("loading took {}s!", static_cast<float>(time) / 1000.f);

	internal_create_shutdown_hook();

	return TRUE;
}

void loader_shutdown()
{
	loader_set_logger_scope("Loader Shutdown");

	loader_log_info("Shutting down...");
	{
		logger_nestscope nest;

		for (auto& mod : g_loaded_mods)
		{
			const auto mod_name = mod->name;
			auto shutdown_function = (void(*)(void))GetProcAddress(mod->handle, "?shutdown@@YAXXZ");
			
			if (shutdown_function == nullptr)
			{
				// try C style mangled name
				shutdown_function = (void(*)(void))GetProcAddress(mod->handle, "shutdown");
			}

			if (shutdown_function)
			{
				shutdown_function();
			}
			else
			{
				loader_log_warn("no shutdown function was called for mod {}", mod_name);
			}
		}
	}

	logger_shutdown();
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
	else if (fdwReason == DLL_PROCESS_DETACH)
	{
		for (auto& mod : g_loaded_mods)
		{
			FreeLibrary(mod->handle);
		}

#if _LOADER_ALLOCATE_CONSOLE
		fclose(stdin);
		fclose(stdout);

		FreeConsole();
#endif

		FreeLibraryAndExitThread(hModule, 0);
	}

	return TRUE;
}