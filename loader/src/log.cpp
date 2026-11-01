#include "loader/loader.h"
#include "loader/log.h"

#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"

#include <string>
#include <filesystem>
#include <memory>
#include <queue>

// basic c wrapper for spdlog

#ifdef _cplusplus
extern "C"
{
#endif
	LOADER_DLL std::shared_ptr<spdlog::logger> logger;

	struct LoggerEvent 
	{ 
		const uint8_t scope_depth;
		const std::string scope_name;
		const loader_mod& mod;
		const spdlog::level::level_enum severity; 
		const std::string msg;
	};

	std::queue<LoggerEvent> log_queue;
	std::condition_variable worker_cv;
	std::mutex logger_mutex;
	std::mutex logger_event_mutex;
	std::thread logger_thread(logger_worker);
	bool terminating = false;

	std::string current_scope;
	uint8_t scope_depth = 0;
	uint8_t scope_tab_size = 2;

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
		{
			std::lock_guard lock(logger_event_mutex);

			log_queue.push({ scope_depth, current_scope, mod, severity, std::move(message) });
		}
		worker_cv.notify_one();
	}

	void loader_set_logger_scope(std::string scope_name)
	{
		std::lock_guard lock(logger_event_mutex);
		current_scope = scope_name;
	}

	bool LOADER_DLL logger_shutdown()
	{
		terminating = true;
		worker_cv.notify_all();
		if (logger_thread.joinable())
		{
			logger_thread.join();
		}

		logger->flush();
		spdlog::shutdown();
		return true;
	}

	// THREAD: logger_thread
	void logger_worker()
	{
		std::unique_lock<std::mutex> lock(logger_mutex);
		std::string this_scope;
		std::string formatted_msg;

		while (1)
		{
			worker_cv.wait(lock, [] { return terminating == true || log_queue.size() > 0; });

			if (terminating == true)
			{
				break;
			}

			logger_event_mutex.lock();
			LoggerEvent job = log_queue.front();
			log_queue.pop();
			logger_event_mutex.unlock();
			
			if (&job.mod != &loader_mod::get())
			{
				this_scope = "external";
			}
			else
			{
				this_scope = job.scope_name;
			}

			formatted_msg = std::format("[{}] [{}]: {}", job.mod.name, this_scope, job.msg);
			formatted_msg.insert(0, (size_t)job.scope_depth * scope_tab_size, ' ');
			
			logger->log(job.severity, formatted_msg);
		}
	}

	std::shared_ptr<spdlog::logger> LOADER_DLL spdlog_instance()
	{
		return logger;
	}

	logger_nestscope::logger_nestscope()
	{
		std::lock_guard lock(logger_event_mutex);
		if (scope_depth < (uint8_t)-1) 
			scope_depth++;
	}

	logger_nestscope::~logger_nestscope()
	{
		std::lock_guard lock(logger_event_mutex);
		if (scope_depth > 0) 
			scope_depth--;
	}

#ifdef _cplusplus
}
#endif