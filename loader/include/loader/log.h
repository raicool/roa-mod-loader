#pragma once

#include "loader/loader.h"
#include "loader/mod.h"

#define FMT_UNICODE 0
#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"

#include <string>
#include <format>

#ifdef _cplusplus
extern "C"
{
#endif
	std::shared_ptr<spdlog::logger> LOADER_DLL spdlog_instance();

	void LOADER_DLL loader_log(loader_mod& mod, spdlog::level::level_enum severity, std::string message);

	// prints to the loader console at a trace level
	template<typename... T>
	inline void LOADER_DLL loader_log_trace(const std::string str, T&&... args)
	{
		loader_log(loader_mod::get(), spdlog::level::trace, std::vformat(str, std::make_format_args(args...)));
	}

	// prints to the loader console at a debug level
	template<typename... T>
	inline void LOADER_DLL loader_log_debug(const std::string str, T&&... args)
	{
		loader_log(loader_mod::get(), spdlog::level::debug, std::vformat(str, std::make_format_args(args...)));
	}

	// prints to the loader console at a info level
	template<typename... T>
	inline void LOADER_DLL loader_log_info(const std::string str, T&&... args)
	{
		loader_log(loader_mod::get(), spdlog::level::info, std::vformat(str, std::make_format_args(args...)));
	}

	// prints to the loader console at a warning level
	template<typename... T>
	inline void LOADER_DLL loader_log_warn(const std::string str, T&&... args)
	{
		loader_log(loader_mod::get(), spdlog::level::warn, std::vformat(str, std::make_format_args(args...)));
	}

	// prints to the loader console at a error level
	template<typename... T>
	inline void LOADER_DLL loader_log_error(const std::string str, T&&... args)
	{
		loader_log(loader_mod::get(), spdlog::level::err, std::vformat(str, std::make_format_args(args...)));
	}

	// prints to the loader console at a fatal level
	template<typename... T>
	inline void LOADER_DLL loader_log_fatal(const std::string str, T&&... args)
	{
		loader_log(loader_mod::get(), spdlog::level::critical, std::vformat(str, std::make_format_args(args...)));
	}


	bool LOADER_DLL logger_init();
	bool LOADER_DLL logger_shutdown();

#ifdef _cplusplus
}
#endif