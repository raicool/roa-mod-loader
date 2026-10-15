#include "internal/search.h"
#include "internal/detours.h"
#include "internal/event.h"

#pragma comment(lib, "detours.lib")

// Helper function for installing hooks,
// returns null if the hook failed, otherwise returns the original hooked method
void* InstallHook(std::vector<uint8_t> dat, void* hookFunction) 
{
    void* addr = internal_scan(dat);

    if (addr == nullptr) 
    {
        // Failed to find function to hook
        return NULL;
    }

    // Store it
    void* methodOrig = addr;

    // Attach with Detours
    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());
    DetourAttach(&methodOrig, hookFunction);
    DetourTransactionCommit();

    return methodOrig;
}

// The original function
bool(*GMLStepEventDispatcherOrig)(INT32);
// User hook
void(*UserStepEventDispatcherHook)(int);
// What we're hooking it with
bool GMLStepEventDispatcherHook(INT32 event_subtype) 
{
    UserStepEventDispatcherHook(event_subtype);
    return GMLStepEventDispatcherOrig(event_subtype);
}

// Hooks the GameMaker step event using the passed function.
// The function is passed the step event subtype:
// 0 - Normal step
// 1 - Begin step
// 2 - End step
// Returns whether the hook was successful.
bool GMLHookGlobalStep(void(* hook)(int event_subtype)) 
{
    std::vector<uint8_t> pattern = 
    {
        0x55,
		0x8B, 0xEC,
		0x6A, 0xFF,
		0x68, '?', '?', '?', '?',
		0x64, 0xA1, 0x00, 0x00, 0x00, 0x00,
		0x50,
		0x83, 0xEC, 0x10,
		0x53,
		0x56,
		0x57,
		0xA1, '?', '?', '?', '?',
		0x33, 0xC5,
		0x50,
		0x8D, 0x45, 0xF4,
		0x64, 0xA3, 0x00, 0x00, 0x00, 0x00,
		0x80, 0x3D, '?', '?', '?', '?', 0x00,
		0x74, 0x0E,
		0x6A, 0x04,
		0x6A, 0x06,
		0xB9, '?', '?', '?', '?',
		0xE8, '?', '?', '?', '?'
    };

    GMLStepEventDispatcherOrig = (bool(*)(int32_t))InstallHook(pattern, (void*)GMLStepEventDispatcherHook);
    UserStepEventDispatcherHook = hook;
    return GMLStepEventDispatcherOrig != NULL;
}