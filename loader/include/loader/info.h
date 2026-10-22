#pragma once

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
std::string loader_initialization_get_version(loader_info* info);

// @return current roa-mod-loader build commit hash
uint32_t loader_initialization_get_commit_hash(loader_info* info);

// @return current roa-mod-loader build commit hash string
std::string loader_initialization_buildtype_string(loader_info* info);