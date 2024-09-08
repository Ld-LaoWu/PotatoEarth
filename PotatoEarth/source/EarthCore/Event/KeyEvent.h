#pragma once
#include "Event.h"

namespace PTEarth {

	class PTEARTH_API KeyEvent : public Event {
	public:
		inline int GetKeyCode()const { return m_KeyCode; }
		EVENT_CLASS_CATEGORY( EventCategoryKeyboard | EventCategoryInput )
	protected:
		KeyEvent(const int keyCode) :m_KeyCode(keyCode){}
		int m_KeyCode;
	};

	class PTEARTH_API KeyPressedEvent : public KeyEvent {
	public:
		KeyPressedEvent(const int keyCode, bool isRepeat = false) : KeyEvent(keyCode), m_isRepeat(isRepeat) { }

		bool GetIsRepeat() const { return m_isRepeat; }
		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "键盘按下:" << m_KeyCode << "(repeat = " << m_isRepeat << ")";
			return ss.str();
		}

		EVENT_CLASS_TYPE(KeyPressed)
	private:
		bool m_isRepeat;
	};

	class PTEARTH_API KeyReleasedEvent : public KeyEvent {
	public:
		KeyReleasedEvent(const int keyCode) : KeyEvent(keyCode) { }
		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "键盘按下偷抬起:" << m_KeyCode ;
			return ss.str();
		}

		EVENT_CLASS_TYPE(KeyPressed)
	};
}