#include "PCH.h"
#include "Core/Input.h"

#include "Core/Application.h"
#include "glad/glad.h"
#include "GLFW/glfw3.h"

namespace EX
{
	bool Input::IsKeyPressed(KeyCode keycode)
	{
		auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		auto state = glfwGetKey(window, (int)keycode);
		return state == GLFW_PRESS || state == GLFW_REPEAT;
	}
	bool Input::IsMouseButtonPressed(MouseCode button)
	{
		auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		auto state = glfwGetMouseButton(window, button);
		return state == GLFW_PRESS;
	}

	glm::vec2 Input::GetMousePosition()
	{
		auto* window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);

		return { (float)xpos, (float)ypos };

	}

	void Input::SetMousePosition(const glm::vec2& position)
	{
		auto* window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		glfwSetCursorPos(window, static_cast<double>(position.x), static_cast<double>(position.y));
	}

	float Input::GetMouseX()
	{
		return GetMousePosition().x;

	}

	float Input::GetMouseY()
	{
		return GetMousePosition().y;
	}

	void Input::SetCursorMode(CursorMode mode)
	{
		auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL + (int)mode);
	}
}