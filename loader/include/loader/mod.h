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

	loader_mod(loader_mod& other) = delete;
	void operator=(const loader_mod&) = delete;

private:
	loader_mod() {};

	HMODULE handle;
};

