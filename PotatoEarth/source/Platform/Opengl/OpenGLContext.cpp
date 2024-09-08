#include "potatopch.h"
#include "EarthCore/Core/PotatoEarthBase.h"
#include "OpenGLContext.h"
#include "EarthCore/Core/PTWindow.h"
#include <glad/glad.h>

namespace PTEarth {
	OpenGLContext::OpenGLContext(PTWindow* windowHandle)
		:m_WindowHandle(windowHandle)
	{
		PTEARTH_ENGINGE_ASSERT(windowHandle, "���ڵľ��Ϊ��")
	}
	void OpenGLContext::Init()
	{
		wglMakeCurrent(m_WindowHandle->GetHDC(), m_WindowHandle->GetHGLRC());
		// ��ʼ�� GLAD
		if (!gladLoadGL()) {
			PTEARTH_ENGINGE_ASSERT(false, "��ʼ��GLADʧ��!")
		}

		PT_EARTH_INFO ("OpenGL ��Ϣ��");
		PT_EARTH_INFO("OpenGL Vendor GPU:{0}", (char*)glGetString(GL_VENDOR));
		PT_EARTH_INFO("OpenGL Render GPU:{0}", (char*)glGetString(GL_RENDERER));
		PT_EARTH_INFO("OpenGL Version GPU:{0}", (char*)glGetString(GL_VERSION));
	}
	void OpenGLContext::SwapContextBuffers()
	{
		SwapBuffers(m_WindowHandle->GetHDC());
	}
}
