#pragma once

#include "Core.h"
#include "KeyCodes.h"
#include "MouseCodes.h"
#include <glm/glm.hpp>
namespace EX
{
	class Input
	{
	protected:
	public:

		static bool IsKeyPressed(KeyCode keycode);
		static bool IsMouseButtonPressed(MouseCode button);
		static glm::vec2 GetMousePosition();
		static void SetMousePosition(const glm::vec2& position);
		static float GetMouseX();
		static float GetMouseY();
		static void SetCursorMode(CursorMode mode);
	};
}