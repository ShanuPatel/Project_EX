#pragma once

#include "Engine.h"

namespace EX
{
	class Shader;
	class VertexArray;
	class BaseCamera;
	class Sandbox : public Layer
	{
	public:
		Sandbox();
		virtual ~Sandbox() = default;
		//static Sandbox* GetInstance() { return sSandboxInstance; }
		virtual void OnAttach() override;

		virtual void OnDetach() override;
		virtual void OnUpdate(DeltaTime timestep) override;
		virtual void OnEvent(Event& e)override;

		bool OnKeyPressed(KeyPressedEvent& e);
		bool OnMouseScroll(MouseScrolledEvent& e);
		bool OnMouseButtonPressed(MouseButtonPressedEvent& e);
		bool OnMouseButtonReleased(MouseButtonReleasedEvent& e);
	private:
		BaseCamera mCamera;

		bool mCursorLocked = false;
		static Sandbox* sSandboxInstance;
	};
}