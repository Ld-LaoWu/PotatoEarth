#pragma once
#include "PotatoEarthBase.h"
#include "spdlog/spdlog.h"

namespace PTEarth {
	
	class PTEARTH_API Log 
	{
	public:

		static void Init();
		inline static std::shared_ptr<spdlog::logger>& GetEarthLogger() { return s_EarthLogger; }

	private:
		// 这么做或许并不合理 暂时先打印核心库的相关信息
		static std::shared_ptr<spdlog::logger> s_EarthLogger;
	};
}

#define PO_EARTH_TRACE(...)		 ::PTEarth::Log::GetEarthLogger()->trace(__VA_ARGS__)
#define PO_EARTH_INFO(...)		 ::PTEarth::Log::GetEarthLogger()->info(__VA_ARGS__)
#define PO_EARTH_WARN(...)		 ::PTEarth::Log::GetEarthLogger()->warn(__VA_ARGS__)
#define PO_EARTH_ERROR(...)		 ::PTEarth::Log::GetEarthLogger()->error(__VA_ARGS__)
#define PO_EARTH_CRITICAL(...)   ::PTEarth::Log::GetEarthLogger()->critical(__VA_ARGS__)