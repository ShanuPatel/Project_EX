#include "PCH.h"
#include "OpenGLContext.h"
#include "Core/Core.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace EX
{
	static uint32_t count = 0; 
	OpenGLContext::OpenGLContext(GLFWwindow* windowHandle)
		:mWindowHandle(windowHandle)
	{
		EX_CORE_ASSERT(windowHandle, "failed to windowHandle");
	}

	void OpenGLContext::Init()
	{
		glfwMakeContextCurrent(mWindowHandle);
		const int status = gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress));
		EX_CORE_ASSERT(status, "failed to Initialize glad");

		//EX_CORE_INFO("info: {0}");
		EX_CORE_INFO("Vendor: {0}", (const char*)glGetString(GL_VENDOR));
		EX_CORE_INFO("Renderer: {0}", (const char*)glGetString(GL_RENDERER));
		EX_CORE_INFO("Version: {0}", (const char*)glGetString(GL_VERSION));

//		TracyGpuContext;
		EX_CORE_ASSERT(GLVersion.major > 4 || (GLVersion.major == 4 && GLVersion.minor >= 5), "EX requires at least OpenGL version 4.5!");
	}

	void OpenGLContext::SwapBuffers()
	{
		EX_ZONE_SCOPEDN("SwapBuffers");
		{
			glfwSwapBuffers(mWindowHandle);
			++count;
			//EX_CORE_STATUS("SwapbufferCalled: {}", count);
		}
	}
}