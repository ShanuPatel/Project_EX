#ifndef LOG_HEADER
#define LOG_HEADER

// This ignores all warnings raised inside External headers
//#pragma warning(push, 0)
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>
//#pragma warning(pop)

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>
#include <filesystem>
#include "Core.h"

namespace EX
{
	class Log
	{
	public:
		enum Level : uint32_t
		{
			Trace = 1,
			Debug = 2,
			Info = 4,
			Warn = 8,
			Error = 16,
			Critical = 32,
		};
		Log();
		~Log();
		static void Init();
		static void Shutdown();
		static Ref<spdlog::logger>& GetLogger() { return Logger; }
		// --- Status logging ---
		template<typename... Args>
		static void Status(fmt::format_string<Args...> fmtString, Args&&... args)
		{
			std::string msg = fmt::format(fmtString, std::forward<Args>(args)...);
			fmt::print("\r{}", msg);
			std::fflush(stdout);
		}
	private:
		static Ref<spdlog::logger> Logger;
	};

}
template<typename OStream, glm::length_t L, typename T, glm::qualifier Q>
inline OStream& operator<<(OStream& os, const glm::vec<L, T, Q>& vector)
{
	return os << glm::to_string(vector);
}

template<typename OStream, glm::length_t C, glm::length_t R, typename T, glm::qualifier Q>
inline OStream& operator<<(OStream& os, const glm::mat<C, R, T, Q>& matrix)
{
	return os << glm::to_string(matrix);
}

template<typename OStream, typename T, glm::qualifier Q>
inline OStream& operator<<(OStream& os, glm::qua<T, Q> quaternion)
{
	return os << glm::to_string(quaternion);
}

#ifdef SPDLOG_USE_STD_FORMAT
#define FMT std
#else
#define FMT fmt
#endif

template<>
struct
	FMT::formatter<std::filesystem::path>
{
	constexpr auto parse(format_parse_context& ctx) const -> decltype(ctx.begin())
	{
		return ctx.end();
	}

	template <typename FormatContext>
	auto format(const std::filesystem::path& input, FormatContext& ctx) -> decltype(ctx.out())
	{
		return FMT::format_to(ctx.out(), "{}", input.string());
	}
};

#undef FMT

#define EX_CORE_ERROR(...) Log::GetLogger()->error(__VA_ARGS__)
#define EX_CORE_WARN(...) Log::GetLogger()->warn(__VA_ARGS__)
#define EX_CORE_INFO(...) Log::GetLogger()->info(__VA_ARGS__)
#define EX_CORE_TRACE(...) Log::GetLogger()->trace(__VA_ARGS__)
#define EX_CORE_STATUS(...) Log::Status(__VA_ARGS__)
#define EX_CORE_CRITICAL(...) Log::GetLogger()->critical(__VA_ARGS__)
#endif // !LOG_HEADER
