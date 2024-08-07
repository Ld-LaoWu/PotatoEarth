#include "potatopch.h"
#include "Log.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/basic_file_sink.h"

namespace PTEarth {
	std::shared_ptr<spdlog::logger> Log::S_EarthLogger;

	void Log::Init()
	{
		spdlog::set_pattern("%^[%D]-[%T] %n: %v%$");
		S_EarthLogger = spdlog::stdout_color_mt("土豆地球");

		// 发布模式下可以写入文件
		S_EarthLogger->set_level(spdlog::level::trace);

	}
}