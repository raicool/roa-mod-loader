#include "loader/load.h"

#include "loader/log.h"

#include "yaml-cpp/yaml.h"

bool LOADER_DLL loader_fetch_mod_repository(std::string& mod_name)
{
	const YAML::Node mod_config = YAML::LoadFile(std::format(
		"{}/{}/info.yaml",
		_LOADER_MODS_DIRECTORY,
		mod_name
	));

	std::string err_string = std::format("unable to check repository status for mod: {}", mod_name);
	if (!mod_config)
	{
		err_string.append("\n\tmod config yaml could not be opened!");
		loader_log_error(err_string);
		return false;
	}

	if (mod_config["repository"].IsDefined())
	{
		const std::string repository = mod_config["repository"].as<std::string>();

		// placeholder, will implement at some point
		loader_log_info(repository);
		return false;
	}
}