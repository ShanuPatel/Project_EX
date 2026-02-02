set(LIB_NAME Platform)

set(LIBRARY_HEADERS
	WindowsWindow.h
)

set(LIBRARY_SOURCES
	WindowsWindow.cpp
	WindowsInput.cpp
)

add_library(${LIB_NAME} 
	${LIBRARY_HEADERS}
	${LIBRARY_SOURCES}
)

target_include_directories(${LIB_NAME} 
	PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(${LIB_NAME} PRIVATE CoreLib Renderer)

target_precompile_headers(${LIB_NAME} REUSE_FROM Engine)
