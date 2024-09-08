#pragma once
#include "EarthCore/Core/PotatoEarthBase.h"
#include "EarthCore/Event/WindowEvent.h"
#include "EarthCore/Event/KeyEvent.h"
#include "EarthCore/Event/MouseEvent.h"
#include "EarthCore/Core/PTButtonCodes.h"
#include "EarthCore/Core/PTKeyCodes.h"
#include <string>


/*
* 抽象 windowsDx , opengl vulkan 等相关API 的窗口句柄 
* 可以按需抽象
*/

namespace PTEarth {

	struct WindowProps{

		HDC WHdc;
		std::string Titile;
		unsigned int Width, Height;

		WindowProps(const HDC& inHdc,
					const std::string& title="PotatoEarth",
			        unsigned int width = 1280,
			        unsigned int height = 720)
			: WHdc(inHdc), Titile(title), Width(width), Height(height)
		{}
	};
	
	class PTEARTH_API GraphicsWindow {

	public:
		using EventCallbackFn = std::function<void(Event&)>;
		virtual ~GraphicsWindow () {}

		virtual void OnUpdate () = 0;
		virtual unsigned int GetWidth () const = 0;
		virtual unsigned int GetHeight () const = 0;

		// virtual void SetEventCallback ( const EventCallbackFn& callback ) = 0;
		virtual void SetVSync ( bool enabled ) = 0;
		virtual bool IsVSync () const = 0;

		virtual void OnResize() = 0;
		virtual void OnUpdateFrame() = 0;
		virtual void OnPreRenderFrame() = 0;
		virtual void OnRenderFrame() = 0;
		virtual void OnPostRenderFrame() = 0;

		// 抽象不同的
		virtual void* GetNativeWindow () const = 0;
		
		virtual void onWindowsEvent(Event& event) = 0;
		virtual bool PTKeyPressed(int keyCode) = 0;
		virtual bool PTMouseButtonPressed(int button) = 0;

		static PT_Local<GraphicsWindow> Create(const WindowProps& props = WindowProps(NULL));

	protected:

		virtual bool onKeyPressed(KeyPressedEvent& event) = 0;
		virtual bool onKeyReleased(KeyReleasedEvent& event) = 0;
		virtual bool onMousePressed(MousePressEvent& event) = 0;
		virtual bool onMouseRleaseed(MouseReleasedEvent& event) = 0;

	protected:
		int pressedKey = PTEarth_Key::Key_unknown;
		int pressedButton = PTEarth_Button::PTEARTH_MOUSE_BUTTON_NONE;
	};

}