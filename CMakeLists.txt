cmake_minimum_required(VERSION 3.27)
project(Project_Ex VERSION 0.0.1 LANGUAGES C CXX)

#c# Engine-wide compile options & definitions ompiler version for cpp
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)#required for compilers like clang with has additional extention lib other then standard also

#add_definitions(-D_ITERATOR_DEBUG_LEVEL=0) 
#Prevents LNK2005 by avoiding multiple inconsistent definitions of spdlog.
add_compile_definitions(
    # Global library settings
    SPDLOG_COMPILED_LIB
    YAML_CPP_STATIC_DEFINE
    _HAS_EXCEPTIONS=0

    # Internal debug-related flags
    # Debug/release config macros
    $<$<CONFIG:Debug>:_DEBUG>              # Required by MSVC for STL/debug CRT
    $<$<CONFIG:Debug>:OE_DEBUG>            # Your engine's debug flag
    $<$<CONFIG:RelWithDebInfo>:OE_DEBUG>   # Also count RelWithDebInfo as debug

    # Internal release flags
    $<$<CONFIG:Release>:OE_RELEASE>
    $<$<CONFIG:MinSizeRel>:OE_RELEASE>

    # Optional profiling tools (only in debug-like builds)
    # Enable Tracy only in Debug & RelWithDebInfo
    $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:TRACY_ENABLE>
    $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:TRACY_ALLOC>

    # Define project root path for logging, dev tools
    SOURCE_ROOT=${CMAKE_CURRENT_SOURCE_DIR}
)
if (MSVC)
    #Suppress only that specific warning
    add_compile_definitions(_SILENCE_STDEXT_ARR_ITERS_DEPRECATION_WARNING)
    # remove default warning level from CMAKE_CXX_FLAGS_INIT
    string(REGEX REPLACE "/W[3|4]" "/w" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
endif()
add_subdirectory(ExternalLibrary)
add_subdirectory(ExEngine)
add_subdirectory(Sandbox)

add_custom_target(ASSETS ALL

    # Copy Common assets (models, shaders, etc.) to Sandbox
	COMMAND ${CMAKE_COMMAND} -E copy_directory
	${PROJECT_SOURCE_DIR}/Sandbox/assets
	${PROJECT_BINARY_DIR}/Sandbox/bin/assets

	COMMENT "MOVING DATA...."
)
add_dependencies(SandboxApp ASSETS)