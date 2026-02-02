#ifndef OPENGLCONTEXT_HEADER
#define OPENGLCONTEXT_HEADER

#include "Renderer/GraphicsContext.h"

struct GLFWwindow;

namespace EX
{
	class OpenGLContext : public GraphicsContext
	{
	public:
		OpenGLContext(GLFWwindow* windowHandle);
		virtual void Init() override;
		virtual void SwapBuffers() override;
	private:
		GLFWwindow* mWindowHandle;
	};
}
#endif // !OPENGLCONTEXT_HEADER
