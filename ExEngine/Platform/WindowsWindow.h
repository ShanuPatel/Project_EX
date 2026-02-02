#ifndef WINDOWSWINDOW_HEADER
#define WINDOWSWINDOW_HEADER

#include "Core/Window.h"
#include "Renderer/GraphicsContext.h"

struct GLFWwindow;

namespace EX
{
	class WindowsWindow :public Window
	{
	public:
		WindowsWindow(const WindowProps& props);
		virtual ~WindowsWindow();

		void OnUpdate()override;
		inline unsigned int GetWidth()const override { return mData.Width; }
		inline unsigned int GetHeight()const override { return mData.Height; }

		//window attribute 
		inline void SetEventCallback(const EventCallbackFn& callback) override { mData.EventCallback = callback; }
		virtual void SetVSync(bool bEnabled) override;
		virtual bool IsVSync() const override;

		void ShowCursor() override;
		void HideCursor() override;

		inline virtual void* GetNativeWindow() const {
			return mWindow;
		}
	private:
		GLFWwindow* mWindow;
		Scope<GraphicsContext> mContext;
		virtual void Init(const WindowProps& props);
		virtual void Shutdown();
		struct WindowData
		{
			std::string Title;
			unsigned int Width, Height;
			bool VSync;
			EventCallbackFn EventCallback;
		};
		WindowData mData;
		static bool s_GLFWInitialized;
	};
};


#endif // !WINDOWSWINDOW_HEADER
