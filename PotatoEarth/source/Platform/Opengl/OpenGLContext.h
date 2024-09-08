#pragma once

#include "EarthCore/Render/GraphicsContext.h"

namespace PTEarth {
	
	class PTWindow;
	class OpenGLContext : public GraphicsContext {
	public:
		OpenGLContext(PTWindow* windowHandle);

		virtual void Init() override;
		virtual void SwapContextBuffers() override;

	private:
		PTWindow* m_WindowHandle;
	};
}