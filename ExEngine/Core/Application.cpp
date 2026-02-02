#include "PCH.h"
#include "Application.h"
#include "Renderer/Renderer.h"

namespace EX
{
#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)
	Application* Application::sInstance = nullptr;

	Application::Application(const std::string& name)
	{
		EX_CORE_INFO("Application Contrtucted");
		EX_ZONE_SCOPED;
		sInstance = this;
		mWindow = Window::Create(WindowProps(name));
		mWindow->SetEventCallback(BIND_EVENT_FN(OnEvent));
		Renderer::Init();
		mLayerStack = CreateScope<LayerStack>();
	}

	Application::~Application()
	{
		EX_CORE_INFO("Application Destroyed\n");
		//if (sInstance == this) sInstance = nullptr; // clear non-owning pointer
		Renderer::Shutdown();
	}

	void Application::Run()
	{
		while (bIsRunning)
		{
			EX_ZONE_SCOPEDN("Main Thread");
			const float time = DeltaTime::GetTime();
			const DeltaTime timestep = time - mLastFrameTime;
			mLastFrameTime = time;//temp should be platform specific


			if (!mMinimized)
			{
				for (Layer* layer : *mLayerStack)
				{
					EX_ZONE_SCOPEDN("LayerStack OnUpdate");
					layer->OnUpdate(timestep);
				}
			}				
			{
				{
					EX_ZONE_SCOPEDN("window::Update");
					//std::this_thread::sleep_for(std::chrono::milliseconds(500)); // half second
					mWindow->OnUpdate();
				}
			}
			EX_FRAME_MARK_N("MainLoop");
		}

	}

	bool Application::OnWindowClosed(WindowCloseEvent& e)
	{
		bIsRunning = false;
		
		return true;
	}
	void Application::OnEvent(Event& e)
	{
		EventDispatcher dispactcher(e);
		dispactcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(OnWindowClosed));
		dispactcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FN(OnWindowResize));

		//Debug events
		//EX_CORE_TRACE("{0}", e);

		for (auto it = mLayerStack->end(); it != mLayerStack->begin();)
		{
			if (e.Handled)
				break;
			(*--it)->OnEvent(e);
		}

	}

	void Application::PushLayer(Layer* layer)
	{
		mLayerStack->PushLayer(layer);
		layer->OnAttach();
	}

	void Application::PushOverlay(Layer* overlay)
	{
		mLayerStack->PushOverlay(overlay);
		overlay->OnAttach();
	}

	bool Application::OnWindowResize(WindowResizeEvent& e)
	{
		if (e.GetWidth() == 0 || e.GetHeight() == 0)
		{
			mMinimized = true;
			return false;
		}
		mMinimized = false;

		Renderer::OnWindowResize(e.GetWidth(), e.GetHeight());
		return false;
	}
}