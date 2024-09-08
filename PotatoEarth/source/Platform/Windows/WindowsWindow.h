#pragma once

#include "EarthCore/Render/GraphicsWindow.h"
#include "EarthCore/Render/GraphicsContext.h"
#include "EarthCore/Event/WindowEvent.h"


// windows 平台下的API 抽象
// 根据 底层API 的不同

namespace PTEarth {
	class PTWindow;
	class WindowsWindow : public GraphicsWindow {
	public:
		WindowsWindow(const WindowProps& props);
		virtual ~WindowsWindow();

		void OnUpdate() override;

		inline unsigned int GetWidth() const override { return m_Data.Width; }
		inline unsigned int GetHeight() const override { return m_Data.Height; }

		// 窗口的属性
		void SetVSync(bool enabled) override;
		bool IsVSync() const override;


		virtual void OnResize() override;
		virtual void OnUpdateFrame() override;
		virtual void OnPreRenderFrame() override;
		virtual void OnRenderFrame() override;
		virtual void OnPostRenderFrame() override;

		inline virtual void* GetNativeWindow() const { return m_Window; }
		inline virtual void* GetNativeContext() const { return m_Context; }

		virtual void onWindowsEvent(Event& event) override;

		bool PTKeyPressed(int keyCode) override;
		bool PTMouseButtonPressed(int button) override;

	protected:

		virtual bool onKeyPressed(KeyPressedEvent& event) override;
		virtual bool onKeyReleased(KeyReleasedEvent& event) override;
		virtual bool onMousePressed(MousePressEvent& event) override;
		virtual bool onMouseRleaseed(MouseReleasedEvent& event) override;


	private:
		virtual void Init(const WindowProps& props);
		virtual void Shutdown();

	private:

		GraphicsContext* m_Context;
		PTWindow* m_Window;
		struct WindowData {
			std::string Title;
			unsigned int Width, Height;
			bool VSync;

			EventCallbackFn EventCallback;
		};

		WindowData m_Data;

	};
}