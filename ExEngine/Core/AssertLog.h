#ifndef ASSERTLOG_HEADER
#define ASSERTLOG_HEADER

#include "Core.h"
#include "Log.h"

#include <filesystem>

#ifdef EX_ENABLE_ASSERTS

// Full assert logic (already correct)
#define EX_INTERNAL_ASSERT_IMPL(type, check, msg, ...) \
    { if(!(check)) { EX##type##ERROR(msg, __VA_ARGS__); EX_DEBUGBREAK(); } }

#define EX_INTERNAL_ASSERT_WITH_MSG(type, check, ...) \
    EX_INTERNAL_ASSERT_IMPL(type, check, "Assertion failed: {0}", __VA_ARGS__)

#define EX_INTERNAL_ASSERT_NO_MSG(type, check) \
    EX_INTERNAL_ASSERT_IMPL(type, check, "Assertion '{0}' failed at {1}:{2}", \
    EX_STRINGIFY_MACRO(check), std::filesystem::path(__FILE__).filename().string(), __LINE__)

#define EX_INTERNAL_ASSERT_GET_MACRO_NAME(arg1, arg2, macro, ...) macro
#define EX_INTERNAL_ASSERT_GET_MACRO(...) \
    EX_EXPAND_MACRO(EX_INTERNAL_ASSERT_GET_MACRO_NAME(__VA_ARGS__, \
    EX_INTERNAL_ASSERT_WITH_MSG, EX_INTERNAL_ASSERT_NO_MSG))

#define EX_ASSERT(...) EX_EXPAND_MACRO(EX_INTERNAL_ASSERT_GET_MACRO(__VA_ARGS__)(_, __VA_ARGS__))
#define EX_CORE_ASSERT(...) EX_EXPAND_MACRO(EX_INTERNAL_ASSERT_GET_MACRO(__VA_ARGS__)(_CORE_, __VA_ARGS__))

#else
// Release: no-op
#define EX_ASSERT(...)
#define EX_CORE_ASSERT(...)
#endif
#endif // !ASSERT_HEADER
