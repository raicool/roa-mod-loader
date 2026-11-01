#pragma once

class loader_mod
{
public:
	std::string name;
	std::string description;
	
	std::string repository;
	uint32_t commit_hash;

	static loader_mod& get()
	{
		static loader_mod instance;
		return instance;
	}

	bool is_loaded = false;

	HMODULE handle;

private:
	loader_mod() {};
};

