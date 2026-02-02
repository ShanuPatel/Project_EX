#include "PCH.h"
#include "WindowsWindow.h"

#include "Core/Core.h"
#include <Core/DeltaTime.h>
#include "Core/Event/ApplicationEvent.h"
#include "Core/Event/MouseEvent.h"
#include "Core/Event/KeyEvent.h"

#include "Renderer/OpenGL/OpenGLContext.h"

#include <glad/glad.h>
#include "GLFW/glfw3.h"


namespace EX
{
	bool WindowsWindow::s_GLFWInitialized = false;
	static void GLFWErrorCallback(int error, const char* description)
	{
		EX_CORE_ERROR("GLFW ERROR ({0}): {1}", error, description);
	}

	Scope<Window> Window::Create(const WindowProps& props)
	{
		return CreateScope<WindowsWindow>(props);
	}

	float DeltaTime::GetTime()
	{
		return static_cast<float>(glfwGetTime());
	}

	WindowsWindow::WindowsWindow(const WindowProps& props)
	{
		Init(props);
	}

	WindowsWindow::~WindowsWindow()
	{
		Shutdown();
	}

	void WindowsWindow::SetVSync(bool bEnabled)
	{
		if (bEnabled)
		{
			glfwSwapInterval(1);
		}
		else
		{
			glfwSwapInterval(0);
		}
		mData.VSync = bEnabled;
	}

	bool WindowsWindow::IsVSync() const
	{
		return mData.VSync;
	}

	void WindowsWindow::ShowCursor()
	{
		glfwSetInputMode(mWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}

	void WindowsWindow::HideCursor()
	{
		glfwSetInputMode(mWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	}

	void WindowsWindow::Init(const WindowProps& props)
	{
		mData.Title = props.Title;
		mData.Width = props.Width;
		mData.Height = props.Height;

		EX_CORE_INFO("Creating window {0}, ({1}, {2})", props.Title, props.Width, props.Height);

		if (!s_GLFWInitialized)
		{
			int success = glfwInit();
			EX_CORE_ASSERT(success, "Could not Initialize GLFW");
			glfwSetErrorCallback(GLFWErrorCallback);
			s_GLFWInitialized = true;
		}
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		mWindow = glfwCreateWindow((int)mData.Width, (int)mData.Height, mData.Title.c_str(), nullptr, nullptr);

		mContext = CreateScope<OpenGLContext>(mWindow);
		mContext->Init();

		glfwSetWindowUserPointer(mWindow, &mData);
		SetVSync(false);
		//glfwMaximizeWindow(mWindow);
		//set GLFW callbacks
		glfwSetWindowSizeCallback(mWindow, [](GLFWwindow* window, int width, int height)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
				data.Width = width;
				data.Height = height;

				WindowResizeEvent event(width, height);
				data.EventCallback(event);
			});

		glfwSetWindowCloseCallback(mWindow, [](GLFWwindow* window)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
				WindowCloseEvent event;
				data.EventCallback(event);
			});

		glfwSetKeyCallback(mWindow, [](GLFWwindow* window, int key, int scancode, int acttion, int mods)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
				switch (acttion)
				{
				case GLFW_PRESS:
				{
					KeyPressedEvent event(key, 0);
					data.EventCallback(event);
					break;
				}
				case GLFW_RELEASE:
				{
					KeyReleasedEvent event(key);
					data.EventCallback(event);
					break;
				}
				case GLFW_REPEAT:
				{
					KeyPressedEvent event(key, 1);
					data.EventCallback(event);
					break;
				}
				default:
					break;
				}
			});

		glfwSetCharCallback(mWindow, [](GLFWwindow* window, unsigned int keycode)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
				KeyTypedEvent event(keycode);
				data.EventCallback(event);
			});

		glfwSetMouseButtonCallback(mWindow, [](GLFWwindow* window, int button, int action, int mods)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
				switch (action)
				{
				case GLFW_PRESS:
				{
					MouseButtonPressedEvent event(button);
					data.EventCallback(event);
					break;
				}
				case GLFW_RELEASE:
				{
					MouseButtonReleasedEvent event(button);
					data.EventCallback(event);
					break;
				}
				default:
					break;
				}
			});
		glfwSetScrollCallback(mWindow, [](GLFWwindow* window, double xOffset, double yOffset)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
				MouseScrolledEvent event((float)xOffset, (float)yOffset);
				data.EventCallback(event);
			});

		glfwSetCursorPosCallback(mWindow, [](GLFWwindow* window, double xPos, double yPos)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
				MouseMovedEvent event((float)xPos, (float)yPos);
				data.EventCallback(event);
			});
	}

	void WindowsWindow::Shutdown()
	{
		glfwDestroyWindow(mWindow);
		glfwTerminate();
	}

	void WindowsWindow::OnUpdate()
	{
		EX_ZONE_SCOPEDN("WindowsWindow::OnUpdate");

		{
			EX_ZONE_SCOPEDN("PollEvents");
			glfwPollEvents();
		}
		mContext->SwapBuffers();
	}
}