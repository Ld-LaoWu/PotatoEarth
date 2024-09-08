#pragma once

namespace PTEarth {
	class GraphicsContext {
	public:
		virtual void Init() = 0;
		virtual void SwapContextBuffers() = 0;
	};
}
