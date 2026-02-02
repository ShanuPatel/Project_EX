set(LIB_NAME Engine)

# Engine-wide source files (PCH.cpp etc.)
set(ENGINE_SOURCES
    PCH.cpp
)

set(ENGINE_HEADERS
    Engine.h
    PCH.h
)

# Create Engine static library
add_library(Engine STATIC
    ${ENGINE_SOURCES}
    ${ENGINE_HEADERS}
)

# Engine public include path (Engine root folder)
target_include_directories(Engine
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/..
)

# Precompiled header for Engine (all submodules can reuse this)
target_precompile_headers(Engine
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/PCH.h
)

# Add Core submodule
add_subdirectory(Core)
add_subdirectory(Renderer)
add_subdirectory(Platform)

# Link external packages (spdlog etc.) here once
target_link_libraries(Engine PUBLIC CoreLib Renderer Platform)
