#pragma once

#ifdef _cplusplus
extern "C"
{
#endif
	inline std::shared_ptr<spdlog::logger> debuglogger;

	__declspec(dllexport) std::shared_ptr<spdlog::logger> spdlog_instance();

#define LOG_TRACE(...) spdlog_instance()->trace(__VA_ARGS__)
#define LOG_DEBUG(...) spdlog_instance()->debug(__VA_ARGS__)
#define LOG_INFO(...)  spdlog_instance()->info(__VA_ARGS__)
#define LOG_WARN(...)  spdlog_instance()->warn(__VA_ARGS__)
#define LOG_ERROR(...) spdlog_instance()->error(__VA_ARGS__)
#define LOG_FATAL(...) spdlog_instance()->critical(__VA_ARGS__)

	void __declspec(dllexport) loader_log_trace(std::string str);
	void __declspec(dllexport) loader_log_debug(std::string str);
	void __declspec(dllexport) loader_log_info(std::string str);
	void __declspec(dllexport) loader_log_warn(std::string str);
	void __declspec(dllexport) loader_log_error(std::string str);
	void __declspec(dllexport) loader_log_fatal(std::string str);

	bool __declspec(dllexport) logger_init();
	bool __declspec(dllexport) logger_shutdown();

#ifdef _cplusplus
}
#endif