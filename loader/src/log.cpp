#include "loader/loader.h"
#include "loader/log.h"

#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"

#include <string>
#include <filesystem>
#include <memory>

// basic c wrapper for spdlog

#ifdef _cplusplus
extern "C"
{
#endif

	inline std::shared_ptr<spdlog::logger> debuglogger;

	bool LOADER_DLL logger_init()
	{
		std::filesystem::remove("console.log");
		spdlog::sink_ptr consolesink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
		spdlog::sink_ptr filesink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("console.log");

		consolesink->set_pattern("%T %^%-9l%$ %@: %v");
		filesink->set_pattern("%T %-9l %@: %v");

		debuglogger = std::make_shared<spdlog::logger>("console", spdlog::sinks_init_list{ consolesink, filesink });
		spdlog::register_logger(debuglogger);
		spdlog::set_level(spdlog::level::trace);
		spdlog::flush_on(spdlog::level::trace);

		return true;
	}

	void LOADER_DLL loader_log_trace(std::string str)
	{
		debuglogger->trace(str);
	}

	void LOADER_DLL loader_log_debug(std::string str)
	{
		debuglogger->debug(str);
	}

	void LOADER_DLL loader_log_info(std::string str)
	{
		debuglogger->info(str);
	}

	void LOADER_DLL loader_log_warn(std::string str)
	{
		debuglogger->warn(str);
	}

	void LOADER_DLL loader_log_error(std::string str)
	{
		debuglogger->error(str);
	}

	void LOADER_DLL loader_log_fatal(std::string str)
	{
		debuglogger->critical(str);
	}


	bool LOADER_DLL logger_shutdown()
	{
		debuglogger->flush();
		spdlog::shutdown();
		return true;
	}

	std::shared_ptr<spdlog::logger> LOADER_DLL spdlog_instance()
	{
		return debuglogger;
	}

#ifdef _cplusplus
}
#endif