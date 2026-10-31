#pragma once

#include "internal/internal.h"

#include <string>

enum LoaderBuildType
{
	LoaderBuildType_DEBUG,
	LoaderBuildType_RELEASE
};

// opaque struct, will change depending on loader version
// use loader_initialization...() functions to get init info
struct loader_info;

typedef void (*loader_init_func)(loader_info* loader_info);

// @return roa-mod-loader version
LOADER_DLL std::string loader_get_version(loader_info* info);

// @return current roa-mod-loader build commit hash
LOADER_DLL uint32_t loader_get_commit_hash(loader_info* info);

// @return current roa-mod-loader build commit hash string
LOADER_DLL std::string loader_get_buildtype_string(loader_info* info);