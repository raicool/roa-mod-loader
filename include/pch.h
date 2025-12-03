#pragma once

#ifdef _WIN32
// Windows
#define WIN32_LEAN_AND_MEAN // Exclude rarely-used stuff from Windows headers
#include <windows.h>
#include <shlobj_core.h>
#endif

// Minhook
#include "minhook.h"

// spdlog
#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"