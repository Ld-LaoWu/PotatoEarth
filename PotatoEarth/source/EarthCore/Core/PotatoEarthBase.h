#pragma once
#include <memory>

#ifdef PT_BUILD_DLL
	#define PTEARTH_API __declspec(dllexport)
#else
	#define PTEARTH_API __declspec(dllimport)
#endif // PO_BUILD_DLL

#define BIT(x) (1 << x )

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

