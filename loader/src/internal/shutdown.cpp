#include "internal/shutdown.h"

#include "internal/vm.h"

#include "loader/hook.h"
#include "../dllmain.h"

#include <MinHook.h>

PFUNC_Run_EndGame Run_EndGame_original = nullptr;

extern PFUNC_Run_EndGame Run_EndGame;

void Run_EndGame_detour(void)
{
	loader_shutdown();
	MH_DisableHook(MH_ALL_HOOKS);
}

void internal_create_shutdown_hook()
{
	loader_hook_create(
		Run_EndGame,
		&Run_EndGame_detour,
		reinterpret_cast<void**>(&Run_EndGame_original)
	);
	loader_hook_enable(Run_EndGame);
}