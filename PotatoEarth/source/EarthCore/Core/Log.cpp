#include "potatopch.h"
#include "Log.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/rotating_file_sink.h"
#include <vector>
#include <mutex>

namespace PTEarth {

	std::shared_ptr<spdlog::logger> Log::GetIntance() {

		static std::once_flag onceFlag;
		static std::shared_ptr<spdlog::logger> instance;

		std::call_once(onceFlag, []() {

			// 控制台和 文件的输出
			auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
			auto file_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>("logs/PTEarth_log.txt", 1024 * 1024 * 5, 3); // 每个日志文件最大5M 最多存留3天

			// 设置日志格式
			console_sink->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] %v");
			file_sink->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%l] %v");

			// 将两个 sink 结合在一起
			std::vector<spdlog::sink_ptr> sinks{ console_sink, file_sink };
			instance = std::make_shared<spdlog::logger>("multi_sink", begin(sinks), end(sinks));

			// 设置日志级别和 flush 行为
			instance->set_level(spdlog::level::trace);
			spdlog::flush_on(spdlog::level::trace);

			// 注册这个 logger 使其成为默认的 logger
			spdlog::register_logger(instance);
		});

		return instance;

	}
}