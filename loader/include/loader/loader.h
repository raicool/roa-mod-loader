#pragma once

#ifdef _WIN32
#define WIN32_MEAN_AND_LEAN
#include <windows.h>
#endif

#include <cstdint>

#ifndef LOADER_DLL
#define LOADER_DLL __declspec(dllexport)
#endif

#define _LOADER_MODS_DIRECTORY "mods/"
#define _LOADER_ALLOCATE_CONSOLE 1

#include "hook.h"
#include "load.h"
#include "log.h"
#include "memory.h"
#include "yyc.h"