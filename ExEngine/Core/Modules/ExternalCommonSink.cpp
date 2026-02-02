#include "ExternalConsoleSink.h"


namespace EX
{
	std::function<void(std::string_view, const char*, const char*, int32_t, Log::Level)> ExternalConsoleSink::OnFlush;
}
