#ifndef WINDOW_HEADER
#define WINDOW_HEADER

#include "Core.h"
#include "Event/Event.h"

namespace EX
{
	struct WindowProps
	{
		std::string Title;
		uint32_t Width;
		uint32_t Height;

		WindowProps(const std::string& title = "GLSL Engine", uint32_t width = 1280, uint32_t height = 720)
			:Title(title), Width(width), Height(height) 
		{}
	};

	//interface 
	class Window
	{
	public:
		using EventCallbackFn = std::function<void(Event&)>;
		virtual ~Window() = default;
		virtual void OnUpdate() = 0;
		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;

		//windows attribute
		virtual void SetEventCallback(const EventCallbackFn& callback) = 0;
		virtual void SetVSync(bool bEnabled) = 0;
		virtual bool IsVSync() const = 0;

		virtual void ShowCursor() = 0;
		virtual void HideCursor() = 0;

		virtual void* GetNativeWindow() const = 0;	

		static Scope<Window> Create(const WindowProps& props = WindowProps());
		//WindowProps mData;
	};
}

#endif // !WINDOW_HEADER
