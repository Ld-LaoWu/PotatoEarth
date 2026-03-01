#pragma once
#include "EarthCore/Core/PotatoEarthBase.h"
#include "EarthCore/Core/PTKeyCodes.h"
#include "EarthCore/Core/PTButtonCodes.h"

// ͨ??Tick ??????¼???????
// input ???Ǽ???¼??Ƿ񱻱?????
// ?¼???ѯ

namespace PTEarth {
	class PTInput {
	public:
		static bool IsKeyPressed(int keyCode);
		static bool IsMouseButtonPressed(int button);
		/*static std::pair<float, float> GetMousePosition();
		static float GetMouseX();
		static float GetMouseY();*/
	};
}