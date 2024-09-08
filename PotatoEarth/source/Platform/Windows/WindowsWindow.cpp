#include "potatopch.h"
#include "WindowsWindow.h"

#include <glad/glad.h>
#include "Platform/Opengl/OpenGLContext.h"
#include "EarthCore/Core/Log.h"
#include "EarthCore/Core/PTWindow.h"
#include "EarthCore/Core/PTKeyCodes.h"


namespace PTEarth {

	PT_Local<GraphicsWindow> GraphicsWindow::Create(const WindowProps& props) {

	#ifdef PTEARTH_PLATFORM_WINDOWS

		return CreateLocal<WindowsWindow>(props);
	#else
		PTEARTH_ENGINGE_ASSERT(false, "未知平台！");
		return nullptr;
	#endif // PTEARTH_PLATFORM_WINDOWS

	}

	WindowsWindow::WindowsWindow(const WindowProps& props)
	{
		Init(props);
		// glEnable(GL_DEPTH_TEST);
	}
	WindowsWindow::~WindowsWindow()
	{
		Shutdown();
	}
	void WindowsWindow::OnUpdate()
	{
		/*glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(1.0, 1.0, 0.0, 1.0);*/

		m_Context->SwapContextBuffers();
	}
	void WindowsWindow::SetVSync(bool enabled)
	{
	}
	bool WindowsWindow::IsVSync() const
	{
		return false;
	}
	void WindowsWindow::OnResize()
	{
	}
	void WindowsWindow::OnUpdateFrame()
	{
	}
	void WindowsWindow::OnPreRenderFrame()
	{
	}
	void WindowsWindow::OnRenderFrame()
	{
	}
	void WindowsWindow::OnPostRenderFrame()
	{
	}

	void WindowsWindow::onWindowsEvent(Event& event)
	{

		EventDispatcher dispatcher(event);
		PT_EARTH_INFO(event);

		// dispatcher.Dispatch<WindowCloseEvent>(PT_BIND_EVENT_FN(Application::onWindowClose));

		dispatcher.Dispatch<KeyPressedEvent>(PT_BIND_EVENT_FN(WindowsWindow::onKeyPressed));
		dispatcher.Dispatch<KeyReleasedEvent>(PT_BIND_EVENT_FN(WindowsWindow::onKeyReleased));

		dispatcher.Dispatch<MousePressEvent>(PT_BIND_EVENT_FN(WindowsWindow::onMousePressed));
		dispatcher.Dispatch<MouseReleasedEvent>(PT_BIND_EVENT_FN(WindowsWindow::onMouseRleaseed));
	}

	bool WindowsWindow::PTKeyPressed(int keyCode)
	{
		return pressedKey == keyCode;
	}

	bool WindowsWindow::PTMouseButtonPressed(int button)
	{
		return pressedButton == button;
	}

	bool WindowsWindow::onKeyPressed(KeyPressedEvent& event)
	{
		pressedKey = event.GetKeyCode();
		return true;
	}

	bool WindowsWindow::onKeyReleased(KeyReleasedEvent& event)
	{
		pressedKey = PTEarth_Key::Key_unknown;
		return true;
	}

	bool WindowsWindow::onMousePressed(MousePressEvent& event)
	{
		pressedButton = event.GetButton();
		return true;
	}

	bool WindowsWindow::onMouseRleaseed(MouseReleasedEvent& event)
	{
		pressedButton = -1;
		return true;
	}

	void WindowsWindow::Init(const WindowProps& props)
	{
		m_Data.Title = props.Titile;
		m_Data.Width = props.Width;
		m_Data.Height = props.Height;

		// 转换成UTF8

		PT_EARTH_INFO("创建窗口:{0}({1},{2})", m_Data.Title.c_str(), props.Width, props.Height);

		// 构建
		m_Window = new PTWindow(props.WHdc);
		m_Window->Init();

		m_Context = new OpenGLContext(m_Window);
		m_Context->Init();

	}
	void WindowsWindow::Shutdown()
	{

	}
	//void WindowsWindow::InitWindowsEvents(Event& event)
	//{
	//	 /*if(ev.GetEventType() == EventType::WindowResized){
	//		 PT_EARTH_INFO(ev.ToString());
	//	 }*/
	//}
}