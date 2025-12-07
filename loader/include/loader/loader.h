#pragma once

#ifdef _WIN32
#define WIN32_MEAN_AND_LEAN
#include <windows.h>
#endif

#include <cstdint>

#ifndef LOADER_DLL
#define LOADER_DLL __declspec(dllexport)
#endif

#include "hook.h"
#include "log.h"