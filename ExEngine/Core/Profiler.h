#pragma once

//#include "tracy/Tracy.hpp"
namespace EX::Profile
{
	// Toggle profiling on/off
//#define EX_ENABLE_CPU_PROFILING 1
//#define EX_ENABLE_GPU_PROFILING 1

// ----------------------------
// CPU PROFILING MACROS
// ----------------------------
#if EX_ENABLE_CPU_PROFILING
	// Frame boundary marker
#define EX_FRAME_MARK_N(NAME)               FrameMarkNamed(NAME)

	// Profile a static scope
#define EX_ZONE_SCOPED						ZoneScoped

// Profile a static scope with a custom name
#define EX_ZONE_SCOPEDN(NAME)               ZoneScopedN(NAME)

// Add a simple text message (useful for tagging events)
#define EX_PROFILE_TAG(NAME, VALUE)          TracyMessageL(NAME)

// Dynamic scope (Tracy zones are lightweight, same as static)
#define EX_ZONE_SCOPED_DYNAMIC(NAME)       ZoneScopedN(NAME)

// Name a thread (shows up in Tracy UI)
#define EX_PROFILE_THREAD(NAME)              tracy::SetThreadName(NAME)

// Category equivalent: just append category name in brackets
#define EX_ZONE_SCOPED_CATEGORY(NAME, CATEGORY)  ZoneScopedN(NAME " [" #CATEGORY "]")
#else
#define EX_FRAME_MARK_N(NAME)
#define EX_ZONE_SCOPED      
#define EX_ZONE_SCOPEDN(NAME)
#define EX_PROFILE_TAG(NAME, VALUE)
#define EX_ZONE_SCOPED_DYNAMIC(NAME)
#define EX_PROFILE_THREAD(NAME)
#define EX_ZONE_SCOPED_CATEGORY(NAME, CATEGORY)
#endif

// ----------------------------
// GPU PROFILING MACROS (OpenGL)
// ----------------------------
#if EX_ENABLE_GPU_PROFILING
	// GPU frame marker (used once per frame in render loop)
#define EX_PROFILE_GPU_FRAME(NAME)           FrameMarkNamed(NAME)

// GPU zone (around draw calls or passes)
#define EX_PROFILE_GPU_SCOPE(NAME)           GpuScopedZone(NAME)
#else
#define EX_PROFILE_GPU_FRAME(NAME)
#define EX_PROFILE_GPU_SCOPE(NAME)
#endif
}
