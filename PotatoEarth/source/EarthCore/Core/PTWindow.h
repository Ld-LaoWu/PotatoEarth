#pragma once

#ifdef PTEARTH_PLATFORM_WINDOWS
#include <Windows.h>
#endif // PTEARTH_PLATFORM_WINDOWS

namespace PTEarth {
	class PTWindow {
	public:
		PTWindow(const HDC& hdc) :m_hDC(hdc) {  }
		~PTWindow() {
			if (m_hGLRC) {
				wglMakeCurrent(nullptr, nullptr);
				wglDeleteContext(m_hGLRC);
				m_hGLRC = nullptr;
			}
		}

		void Init() {
			if (m_hGLRC || !m_hDC) return;
			PIXELFORMATDESCRIPTOR pfd;
			memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR));
			pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
			pfd.nVersion = 1;
			pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
			pfd.iPixelType = PFD_TYPE_RGBA;
			pfd.cColorBits = 32;
			pfd.cDepthBits = 24;
			pfd.cStencilBits = 8;
			pfd.iLayerType = PFD_MAIN_PLANE;

			int pixelFormat = ChoosePixelFormat(m_hDC, &pfd);
			SetPixelFormat(m_hDC, pixelFormat, &pfd);
			m_hGLRC = wglCreateContext(m_hDC);
		}

		HDC& GetHDC() { return m_hDC; }
		HGLRC& GetHGLRC() { return m_hGLRC; }

	private:
		// 获取到外部传递进来的句柄
		HDC m_hDC = NULL;
		HGLRC m_hGLRC = NULL;

		// 鼠标和键盘的控制事件
	};
}


