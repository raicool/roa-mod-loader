#pragma once

#include "loader/loader.h"

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

	/*
	prints to the plugin logger at a trace level
	*/
	template<typename... T>
	void LOADER_DLL loader_log_trace(const std::string str, T&&... args)
	{
		spdlog_instance()->trace(std::vformat(str, std::make_format_args(args...)));
	}

	/*
	prints to the plugin logger at a debug level
	*/
	template<typename... T>
	void LOADER_DLL loader_log_debug(const std::string str, T&&... args)
	{
		spdlog_instance()->debug(std::vformat(str, std::make_format_args(args...)));
	}

	/*
	prints to the plugin logger at a info level
	*/
	template<typename... T>
	void LOADER_DLL loader_log_info(const std::string str, T&&... args)
	{
		spdlog_instance()->info(std::vformat(str, std::make_format_args(args...)));
	}

	/*
	prints to the plugin logger at a warn level
	*/
	template<typename... T>
	void LOADER_DLL loader_log_warn(const std::string str, T&&... args)
	{
		spdlog_instance()->warn(std::vformat(str, std::make_format_args(args...)));
	}

	/*
	prints to the plugin logger at a error level
	*/
	template<typename... T>
	void LOADER_DLL loader_log_error(const std::string str, T&&... args)
	{
		spdlog_instance()->error(std::vformat(str, std::make_format_args(args...)));
	}

	/*
	prints to the plugin logger at a critical level
	*/
	template<typename... T>
	void LOADER_DLL loader_log_fatal(const std::string str, T&&... args)
	{
		spdlog_instance()->critical(std::vformat(str, std::make_format_args(args...)));
	}

	bool LOADER_DLL logger_init();
	bool LOADER_DLL logger_shutdown();

#ifdef _cplusplus
}
#endif