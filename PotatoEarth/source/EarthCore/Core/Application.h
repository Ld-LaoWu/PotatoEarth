#pragma once
#include "PotatoEarthBase.h"

namespace PTEarth {
	
	class PTEARTH_API Application 
	{
	public:
		Application();
		virtual ~Application();

		void Run();

	private:
		static Application* S_Instance;
	};
}