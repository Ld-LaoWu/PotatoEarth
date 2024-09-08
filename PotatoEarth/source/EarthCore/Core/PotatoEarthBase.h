#pragma once
#include <memory>


#ifdef PTEARTH_PLATFORM_WINDOWS
#if PTEARTH_DYNAMIC_LINK	
	#ifdef PT_BUILD_DLL
		#define PTEARTH_API __declspec(dllexport)
	#else
		#define PTEARTH_API __declspec(dllimport)
	#endif // PO_BUILD_DLL
#else
	#define PTEARTH_API
#endif
#else
	#error 土豆地球仅支持windows平台
#endif // PTEARTH_PLATFORM_WINDOWS

// deugger版本
#ifdef PO_DEBUG
	#define PTEARTH_ENBLE_ASSERTS
#endif 

#ifdef PTEARTH_ENBLE_ASSERTS
	#define PTEARTH_ENGINGE_ASSERT(x,...){if (!(x)) { PT_EARTH_ERROR("意外错误:{0}",__VA_ARGS__);__debugbreak(); }}
#else
	#define PTEARTH_ENGINGE_ASSERT(x,...)
#endif


#define BIT(x) (1 << x )
#define PT_BIND_EVENT_FN(fn) std::bind(&fn, this, std::placeholders::_1)

// earth里面 内部管理统计指针使用

namespace PTEarth 
{
	template<typename T>
	using PT_Local = std::unique_ptr<T>;

	template<typename T, typename ... Args>
	constexpr PT_Local<T> CreateLocal(Args&& ... args) {
		return std::make_unique<T>(std::forward<Args>(args)...);
	}

	template<typename T>
	using PT_Ref = std::shared_ptr<T>;

	template<typename T, typename ... Args>
	constexpr PT_Ref<T> CreateRef(Args&& ... args) {
		return std::make_shared<T>(std::forward<Args>(args)...);
	}
}

