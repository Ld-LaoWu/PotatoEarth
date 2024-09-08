#pragma once
#include "EarthCore/Core/PotatoEarthBase.h"
#include "EarthCore/Core/PTKeyCodes.h"
#include "EarthCore/Core/PTButtonCodes.h"

// 通过Tick 来完成事件的驱动
// input 则是检测事件是否被被触发
// 事件轮询

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