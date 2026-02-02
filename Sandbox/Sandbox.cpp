#include "PCH.h"
#include "Sandbox.h"
#include "Core/Application.h"
#include "Renderer/Shader.h"
#include "Renderer/Buffer.h"
#include "Renderer/VertexArray.h"
#include "Renderer/RenderCommand.h"
#include "Renderer/Renderer.h"
#include "Renderer/BaseCamera.h"

//there should only be one owner 
EX::Sandbox* EX::Sandbox::sSandboxInstance = nullptr;
namespace EX
{
	static glm::vec2 autoDelta;
	Sandbox::Sandbox()
		:Layer("EX")
	{
		sSandboxInstance = this;
		//sSandboxInstance = Scope<Sandbox>(this);//scpoe Doesnt work here no ownership ....cause Ref maybe
	}

	void Sandbox::OnAttach()
	{
		EX_CORE_INFO("Attached");
		mCamera.SetViewportSize(1280, 720);

	}

	void Sandbox::OnDetach()
	{
		EX_CORE_INFO("DeAttached");
	}

	void Sandbox::OnUpdate(EX::DeltaTime timestep)
	{
		// Camera setup for rendering
		mCamera.OnUpdate(timestep);
		//EX_CORE_INFO("{0}", mCamera.GetPosition());
		CameraData cameraData = {
			mCamera.GetView(),
			mCamera.GetProjection(),
			mCamera.GetViewProjection(),
			glm::vec4(mCamera.GetPosition(), 1.0f)
		};

		Renderer::BeginScene(cameraData);
		
		//// auto orbit around focus
		mCamera.AutoOrbit(1.0f, 0.0f, -timestep);
		Renderer::EndScene();

		////EX_CORE_INFO("OnUpdate");
	}
	void Sandbox::OnEvent(EX::Event& e)
	{
		//EX_CORE_INFO("OnEvent");		// Forward to camera first
		mCamera.OnEvent(e);

		// Then handle editor-specific events
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<KeyPressedEvent>(EX_BIND_FN(Sandbox::OnKeyPressed));
		dispatcher.Dispatch<MouseButtonPressedEvent>(EX_BIND_FN(Sandbox::OnMouseButtonPressed));
		dispatcher.Dispatch<MouseButtonReleasedEvent>(EX_BIND_FN(Sandbox::OnMouseButtonReleased));
	}
	bool Sandbox::OnKeyPressed(KeyPressedEvent& e)
	{
		return false;
	}

	bool Sandbox::OnMouseButtonPressed(MouseButtonPressedEvent& e)
	{
		if (e.GetMouseButton() == Mouse::Button1)
		{
			Application::Get().GetWindow().HideCursor();
			//EX_CORE_STATUS("Mouse1");
			mCursorLocked = true;
			mCamera.SetCursorLocked(mCursorLocked);
			return true;
		}
		if (e.GetMouseButton() == Mouse::Button0)
		{
			Application::Get().GetWindow().HideCursor();
			//EX_CORE_STATUS("Mouse0");
			mCursorLocked = true;
			mCamera.SetCursorLocked(mCursorLocked);
			mCamera.SetMouseLastPosition(Input::GetMousePosition());
			//EX_CORE_STATUS("{0}", Input::GetMousePosition());
			return true;
		}

		return false;
	}

	bool Sandbox::OnMouseButtonReleased(MouseButtonReleasedEvent& e)
	{
		if (mCursorLocked == true)
		{
			Application::Get().GetWindow().ShowCursor();
			mCursorLocked = false;
			mCamera.SetCursorLocked(mCursorLocked);
			return true;
		}
		return false;
	}

	bool Sandbox::OnMouseScroll(MouseScrolledEvent& e)
	{
		return false;
	}
}

