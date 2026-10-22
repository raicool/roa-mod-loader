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
	LOADER_DLL std::shared_ptr<spdlog::logger> logger;

	bool LOADER_DLL logger_init()
	{
		std::filesystem::remove("console.log");
		spdlog::sinks_init_list sinks;
		spdlog::sink_ptr filesink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("console.log");
		filesink->set_pattern("[%T] %@: %v");

#if _LOADER_ALLOCATE_CONSOLE
		spdlog::sink_ptr consolesink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
		consolesink->set_pattern("[%^%T%$] %@: %v");

		logger = std::make_shared<spdlog::logger>("console", spdlog::sinks_init_list{ consolesink, filesink });
#else
		logger = std::make_shared<spdlog::logger>("console", spdlog::sinks_init_list{ filesink });
#endif

		spdlog::register_logger(logger);
		spdlog::set_level(spdlog::level::trace);
		spdlog::flush_on(spdlog::level::trace);

		return true;
	}

	void loader_log(loader_mod& mod, spdlog::level::level_enum severity, std::string message)
	{
		logger->log(severity, std::format("[{}]: {}", mod.name, message));
	}

	bool LOADER_DLL logger_shutdown()
	{
		logger->flush();
		spdlog::shutdown();
		return true;
	}

	std::shared_ptr<spdlog::logger> LOADER_DLL spdlog_instance()
	{
		return logger;
	}

#ifdef _cplusplus
}
#endif