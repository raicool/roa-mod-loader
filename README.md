# roa-mod-loader
basic loader for mods made for Rivals of Aether.

> Works through versions 1.4.0 - 2.1.9.1, for older version you will have to manually start the loader using Cheat Engine

> [!NOTE]
> interested in creating your own roa mod? a mod template is available [here](https://github.com/raicool/roa-mod-template)

## Initializing a mod example
```cpp
#include <loader/loader.h>

LOADER_MOD_ENTRY initialize(loader_info* info)
{
	loader_mod& mod_instance = loader_mod::get();

	mod_instance.name = "roa-mod-template";
	mod_instance.description = "a template mod for roa-mod-loader";
	mod_instance.repository = "https://github.com/raicool/roa-mod-template";
	mod_instance.commit_hash = 0x00000000;

	loader_log_info("welcome to roa-mod-template version {}", VERSION_STR);

	// logs can also be nested using logger_nestscope
	// logs will automatically be unnested when the nest class is out of scope
	{
		logger_nestscope nest;

		loader_log_info("this is my nested log!");
	}

	return mod_instance;
}

LOADER_MOD_EXIT shutdown()
{
	loader_log_info("this is my shutdown!");
}
```
