#pragma once

#include "Window.h"
#include "Event/Event.h"
#include "Event/ApplicationEvent.h"
#include "LayerStack.h"

namespace EX
{
	class Application 
	{
	public:
		Application(const std::string& name = "App");
		virtual ~Application();
		
		void OnEvent(Event& e);

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* overlay);

		void PopLayer(Layer* layer) const { mLayerStack->PopLayer(layer); }
		void PopOverlay(Layer* overlay) const { mLayerStack->PopOverlay(overlay); }

		inline static Application& Get() { return *sInstance; }
		inline Window& GetWindow() const { return *mWindow; }

		void Run();

	private:		
		bool OnWindowClosed(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);
		static Application* sInstance;
		Scope<Window>mWindow;
		bool bIsRunning = true;
		bool mMinimized = false;
		float mLastFrameTime = 0.0f;
		Scope<LayerStack> mLayerStack;
	};
		Scope < Application> CreateApplication();
}