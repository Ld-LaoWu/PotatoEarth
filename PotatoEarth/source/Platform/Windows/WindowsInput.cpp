#include "potatopch.h"
#include "EarthCore/Core/PTInput.h"
#include "EarthCore/Core/Application.h"
#include "Platform/Windows/WindowsWindow.h"

namespace PTEarth {

	bool PTInput::IsKeyPressed(int keyCode)
	{
		auto window = static_cast<WindowsWindow*>(&Application::Get().GetGraphicWidow());
		return window->PTKeyPressed(keyCode);
	}

	bool PTInput::IsMouseButtonPressed(int button)
	{
		auto window = static_cast<WindowsWindow*>(&Application::Get().GetGraphicWidow());
		return window->PTMouseButtonPressed(button);
	}
}