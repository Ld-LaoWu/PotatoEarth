#pragma once
#include "PotatoEarthBase.h"
#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"


namespace PTEarth {
	
	class PTEARTH_API Log final
	{
	public:

		static std::shared_ptr<spdlog::logger> GetIntance();

		Log(const Log&) = delete;
		Log& operator=(const Log&) = delete;

	private:

		Log() = default;
	};
}

#define PT_EARTH_TRACE(...)		 ::PTEarth::Log::GetIntance()->trace(__VA_ARGS__)
#define PT_EARTH_INFO(...)		 ::PTEarth::Log::GetIntance()->info(__VA_ARGS__)
#define PT_EARTH_WARN(...)		 ::PTEarth::Log::GetIntance()->warn(__VA_ARGS__)
#define PT_EARTH_ERROR(...)		 ::PTEarth::Log::GetIntance()->error(__VA_ARGS__)
#define PT_EARTH_CRITICAL(...)   ::PTEarth::Log::GetIntance()->critical(__VA_ARGS__)