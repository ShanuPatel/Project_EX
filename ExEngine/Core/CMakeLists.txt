set(LIB_NAME CoreLib)
set(LIBRARY_HEADERS 
	Core.h
	AssertLog.h
	Application.h
	EntryPoint.h
	Log.h
	Modules/ExternalConsoleSink.h
	Window.h
	Event/ApplicationEvent.h
	Event/Event.h
	Event/KeyEvent.h
	Event/MouseEvent.h
	KeyCodes.h
	MouseCodes.h
	Input.h
	Layer.h
	LayerStack.h
	AssetManager.h
	Profiler.h
)

set(LIBRARY_SOURCES 
	Application.cpp
	Log.cpp
	Modules/ExternalCommonSink.cpp
	Layer.cpp
	LayerStack.cpp
	AssetManager.cpp
)

add_library(${LIB_NAME}
	${LIBRARY_HEADERS}
	${LIBRARY_SOURCES} 
)

target_include_directories(${LIB_NAME}
	PUBLIC 
	${CMAKE_CURRENT_SOURCE_DIR}/..
)

target_link_libraries(${LIB_NAME} PUBLIC Packages)
target_precompile_headers(${LIB_NAME} REUSE_FROM Engine)