#include "Log.h"

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>
#include "Core/Modules/ExternalConsoleSink.h"


namespace EX
{
	Ref<spdlog::logger> Log::Logger;
	Log::Log()
	{

	}

	Log::~Log()
	{

	}

	void Log::Init()
	{
		std::vector<spdlog::sink_ptr> logSinks;
		logSinks.emplace_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
		logSinks.emplace_back(std::make_shared<ExternalConsoleSink>(true));
		logSinks.emplace_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>("GLSL.log", true));

		// Patterns for sinks
		logSinks[0]->set_pattern("%^[%T] %n: %v%$");
		logSinks[1]->set_pattern("[%T] [%l] %n: %v");

		// Create logger from all sinks
		Logger = std::make_shared<spdlog::logger>("GLSL", begin(logSinks), end(logSinks));
		spdlog::register_logger(Logger);

#ifdef OE_DEBUG
		Logger->set_level(spdlog::level::trace); // Verbose logging in debug
		Logger->flush_on(spdlog::level::trace);
#else
		Logger->set_level(spdlog::level::warn);  // Quiet in release (warn and above)
		Logger->flush_on(spdlog::level::err);    // Flush only on serious issues
#endif

	}

	void Log::Shutdown()
	{
		// Flush any pending messages
		if (Logger)
		{
			Logger->flush();
			spdlog::drop_all(); // Drop all loggers from registry
			Logger.reset();      // Release shared_ptr
		}

		// Drop all loggers and release sink memory
		spdlog::shutdown();
	}
}