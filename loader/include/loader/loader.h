#pragma once

#ifdef _WIN32
#define WIN32_MEAN_AND_LEAN
#include <windows.h>
#endif

#include <cstdint>

#define LOADER_MOD_ENTRY __declspec(dllexport) struct loader_mod&
#define LOADER_MOD_EXIT __declspec(dllexport) void

#define _LOADER_MODS_DIRECTORY "mods/"
#define _LOADER_ALLOCATE_CONSOLE 1

#include "hook.h"
#include "info.h"
#include "load.h"
#include "log.h"
#include "memory.h"
#include "mod.h"
#include "yoyo.h"
#include "yyc.h"