#include "potatopch.h"
#include "Application.h"

#include "EarthCore/Event/WindowEvent.h"
#include "EarthCore/Event/KeyEvent.h"
#include "EarthCore/Event/MouseEvent.h"

#include "EarthCore/Core/PTInput.h"
#include "PTKeyCodes.h"
#include "PTButtonCodes.h"

namespace PTEarth {
	Application* Application::S_Instance = nullptr;

	Application::Application(const HDC& inHDC)
	{

		PTEARTH_ENGINGE_ASSERT(!S_Instance, "App已经创建!");
		S_Instance = this;

		// 窗口设置
		m_Window = GraphicsWindow::Create(inHDC);
		// m_Window->SetEventCallback(PT_BIND_EVENT_FN(Application::onEvent)); 
		m_Window->SetVSync(true);
		

		// 渲染设置
	}
	Application::~Application()
	{
	}

	void Application::onEvent(Event& e)
	{
		EventDispatcher dispatcher(e);

		dispatcher.Dispatch<WindowCloseEvent>(PT_BIND_EVENT_FN(Application::onWindowClose));
		dispatcher.Dispatch<WindowResizedEvent>(PT_BIND_EVENT_FN(Application::onWindowResized));

		m_Window->onWindowsEvent(e);

	}
	void Application::Close()
	{
		m_Runing = false;
	}
	void Application::Run()
	{
		m_Window->OnUpdate();

		if (PTInput::IsKeyPressed((int)PTEarth_Key::Key_A) ){
			PT_EARTH_INFO("A键按下");
		}
		if (PTInput::IsMouseButtonPressed(PTEarth_Button::PTEARTH_MOUSE_BUTTON_LEFT)) {
			PT_EARTH_INFO("鼠标左键键按下");
		}
	}
	bool Application::onWindowClose(WindowCloseEvent& e)
	{
		m_Runing = false;
		return true;
	}
	bool Application::onWindowResized(WindowResizedEvent& e)
	{
		if (e.GetWidth() == 0 || e.GetHeight() == 0)
		{
			return false;
		}

		return false;
	}
}