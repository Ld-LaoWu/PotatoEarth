#include "potatopch.h"
#include "Application.h"

namespace PTEarth {
	Application* Application::S_Instance = nullptr;

	Application::Application()
	{

		S_Instance = this;
	}
	Application::~Application()
	{
	}
	void Application::Run()
	{
		while (true)
		{

		}
	}
}