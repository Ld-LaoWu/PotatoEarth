#pragma once
#include "Event.h"

namespace PTEarth {

	class PTEARTH_API WindowResizedEvent : public Event
	{
	public:
		WindowResizedEvent(unsigned int width, unsigned int  height)
			:m_Width(width), m_Height(height) {}

		unsigned int GetWidth() const { return m_Width; }
		unsigned int GetHeight() const { return m_Height; }


		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "´°¿ÚËõ·Å: " << m_Width << ", " << m_Height;
			return ss.str();
		}


		EVENT_CLASS_TYPE(WindowResized)
		EVENT_CLASS_CATEGORY(EventCategory::EventCategoryWindow)

	private:
		unsigned int m_Width, m_Height;
	};

	class PTEARTH_API WindowCloseEvent : public Event
	{
	public:
		WindowCloseEvent() = default;

		EVENT_CLASS_TYPE(WindowClosed)
		EVENT_CLASS_CATEGORY(EventCategory::EventCategoryWindow)
	};


}