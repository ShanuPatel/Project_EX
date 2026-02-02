#pragma once
#include <memory>

#ifndef EX_PLATFORM_WINDOWS
#ifdef EX_BUILD
#define EX_API printf("Using")
#else
#define EX_API printf("NotUsing")
#endif
#else
#error ONLY SUPPORTS WINDOWS
#endif // !EX_PLATFORM_WINDOWS

#ifdef OE_DEBUG
#define EX_DEBUGBREAK() __debugbreak()
#define EX_ENABLE_ASSERTS
#else
#define EX_DEBUGBREAK()
#endif // DEBUG

//ifdef _DEBUG
//#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
//#define new DBG_NEW
//#endif

#define EX_EXPAND_MACRO(x) x
#define EX_STRINGIFY_MACRO(x) #x
#define BIT(x) (1 << x)

#define EX_BIND_FN(fn) std::bind(&fn, this, std::placeholders::_1)

//CMake Source Dir
#define STRING(x) #x
#define XSTRING(x) STRING(x)


template<typename T>
using Scope = std::unique_ptr<T>;
template<typename T, typename ... Args>
constexpr Scope<T> CreateScope(Args&& ... args)
{
	return std::make_unique<T>(std::forward<Args>(args)...);
}

template<typename T>
using Ref = std::shared_ptr<T>;
template<typename T, typename ... Args>
constexpr Ref<T> CreateRef(Args&& ... args)
{
	return std::make_shared<T>(std::forward<Args>(args)...);
}


#include "Log.h"
#include "AssertLog.h"