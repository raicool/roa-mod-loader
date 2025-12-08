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
	LOADER_DLL std::shared_ptr<spdlog::logger> debuglogger;

	bool LOADER_DLL logger_init()
	{
		std::filesystem::remove("console.log");
		spdlog::sink_ptr consolesink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
		spdlog::sink_ptr filesink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("console.log");

		consolesink->set_pattern("%T %^%9l%$ %@: %v");
		filesink->set_pattern("%T %9l %@: %v");

		debuglogger = std::make_shared<spdlog::logger>("console", spdlog::sinks_init_list{ consolesink, filesink });
		spdlog::register_logger(debuglogger);
		spdlog::set_level(spdlog::level::trace);
		spdlog::flush_on(spdlog::level::trace);

		return true;
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