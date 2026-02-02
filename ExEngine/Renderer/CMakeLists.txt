Set(LIB_NAME Renderer)
Set(LIBRARY_HEADERS
	GraphicsContext.h
	Buffer.h
	VertexArray.h
	UniformBuffer.h
	Shader.h
	ShaderLibrary.h
	Texture.h
	Mesh.h
	Renderer.h
	RendererAPI.h
	RenderCommand.h
	MikkTSpaceHelper.h
	Camera.h
	BaseCamera.h
	Framebuffer.h
	Material.h
	OpenGL/OpenGLContext.h
	OpenGL/OpenGLBuffer.h
	OpenGL/OpenGLVertexArray.h
	OpenGL/OpenGLUniformBuffer.h
	OpenGL/OpenGLTexture2D.h
	OpenGL/OpenGLTextureCubemap.h
	OpenGL/OpenGLShader.h
	OpenGL/OpenGLRendererAPI.h
	OpenGL/OpenGLFramebuffer.h
)

Set(LIBRARY_SOURCES
	Mesh.cpp
	Renderer.cpp
	RendererAPI.cpp
	ShaderLibrary.cpp
	BaseCamera.cpp
	Material.cpp
	OpenGL/OpenGLContext.cpp
	OpenGL/OpenGLBuffer.cpp
	OpenGL/OpenGLVertexArray.cpp
	OpenGL/OpenGLUniformBuffer.cpp
	OpenGL/OpenGLTexture2D.cpp
	OpenGL/OpenGLTextureCubemap.cpp
	OpenGL/OpenGLShader.cpp
	OpenGL/OpenGLRendererAPI.cpp
	OpenGL/OpenGLFramebuffer.cpp
)

add_library(${LIB_NAME} STATIC
	${LIBRARY_HEADERS}
	${LIBRARY_SOURCES}
)

target_link_libraries(${LIB_NAME} PRIVATE CoreLib)