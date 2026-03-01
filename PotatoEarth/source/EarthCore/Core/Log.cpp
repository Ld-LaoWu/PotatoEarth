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

			// ����̨�� �ļ������
			auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
			auto file_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>("logs/PTEarth_log.txt", 1024 * 1024 * 5, 3); // ÿ����־�ļ����5M ������3��

			// ������־��ʽ
			console_sink->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] %v");
			file_sink->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%l] %v");

			// ������ sink �����һ��
			std::vector<spdlog::sink_ptr> sinks{ console_sink, file_sink };
			instance = std::make_shared<spdlog::logger>("multi_sink", begin(sinks), end(sinks));

			// ������־����� flush ��Ϊ
			instance->set_level(spdlog::level::trace);
			spdlog::flush_on(spdlog::level::trace);

			// ע����� logger ʹ���ΪĬ�ϵ� logger
			spdlog::register_logger(instance);
		});

		return instance;

	}
}