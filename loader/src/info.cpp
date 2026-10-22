#include "loader/info.h"

#include <string>

struct loader_info
{
	const char* version;
	uint32_t commit_hash;
	LoaderBuildType build_type;
};

loader_info g_loader_init_info
{
	.version = LOADER_VERSION,
	.build_type = DEBUG ? LoaderBuildType_DEBUG : LoaderBuildType_RELEASE,
};

std::string loader_initialization_get_version(loader_info* info)
{
	return info->version;
}

uint32_t loader_initialization_get_commit_hash(loader_info* info)
{
	return info->commit_hash;
}

std::string loader_initialization_buildtype_string(loader_info* info)
{
	return info->build_type == LoaderBuildType_DEBUG ? "DEBUG" : "RELEASE";
}
