#pragma once
#include "PotatoEarthBase.h"
#include "EarthCore/Event/Event.h"
#include "EarthCore/Render/GraphicsWindow.h"


// ×éÖ¯²ã
namespace PTEarth {
	class WindowResizedEvent;
	class WindowCloseEvent;
	class PTEARTH_API Application 
	{
	public:
		Application(const HDC& inHDC);
		virtual ~Application();

		void onEvent(Event& e);
		
		void Close();
		void Run();

		inline GraphicsWindow& GetGraphicWidow() { return *m_Window; }
		inline static Application& Get() { return *S_Instance; }

	private:
		bool onWindowClose(WindowCloseEvent& e);
		bool onWindowResized(WindowResizedEvent& e);

	private:
		std::unique_ptr<GraphicsWindow> m_Window;
		bool m_Runing = true;

		static Application* S_Instance;
	};

	Application* CreateApplication(const HDC& inHDC);
}