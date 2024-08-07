#pragma once
#include "EarthCore/Core/PotatoEarthBase.h"
#include "EarthCore/Event/Event.h"
#include <string>

namespace PTEarth {

	struct WindowProps
	{
		std::string Titile;
		unsigned int Width, Height;

		WindowProps(const std::string& title="PotatoEarth",
			        unsigned int width = 1280,
			        unsigned int height = 720)
			: Titile(title), Width(width), Height(height)
		{}
	};
	
	class PTEARTH_API GraphicsWindow {

	public:
		using EventCallBackFn = std::function<void(Event&)>;

		virtual ~GraphicsWindow() {}

		virtual void OnResize() = 0;
		virtual void OnUpdateFrame() = 0;
		virtual void OnPreRenderFrame() = 0;
		virtual void OnRenderFrame() = 0;
		virtual void OnPostRenderFrame() = 0;
		
		virtual void Run(double updateRate) = 0;

	private:

		struct WindowData {
			std::string Title;
			unsigned int m_Width, m_Height;
			bool VSync;
		};

		WindowData m_Data;
	};

}