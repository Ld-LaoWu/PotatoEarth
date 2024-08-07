#pragma once

#include "Event.h"

namespace PTEarth {

	//  Û±Í“∆∂Øº‡≤‚
	class PTEARTH_API MouseMovedEvent : public Event {
	public:
		MouseMovedEvent(const float x, const float y)
			:m_MouseX(x), m_MouseY(y)
		{}

		float GetMouseX() const { return m_MouseX; }
		float GetMouseY() const { return m_MouseY; }

		virtual std::string ToString() const override { 
			std::stringstream outss;
			outss << "MouseMoved:( " << m_MouseX << "," << m_MouseY << " )";
			return outss.str();
		}

		EVENT_CLASS_TYPE(EventType::MouseMoved)
		EVENT_CLASS_CATEGORY(EventCategory::EventCategoryMouse | EventCategory::EventCategoryInput)
	private:
		float m_MouseX, m_MouseY;
	};

	class PTEARTH_API MouseWheelEvent : public Event {
	public:
		MouseWheelEvent(const float delta) : m_Delta(delta) {}
		float GetDelta() const { return m_Delta; }

		virtual std::string ToString() const override {
			std::stringstream outss;
			outss << "MouseWheel:( " << m_Delta << " )";
			return outss.str();
		}

		EVENT_CLASS_TYPE(EventType::MouseWheeled)
		EVENT_CLASS_CATEGORY(EventCategory::EventCategoryMouse | EventCategory::EventCategoryInput)

	private:
		float m_Delta;
	};

	class PTEARTH_API MouseButtonEvent : public Event {
	public:
		inline  int GetButton() const { return m_Button; }
		EVENT_CLASS_CATEGORY(EventCategory::EventCategoryMouse | EventCategory::EventCategoryMouseButton | EventCategory::EventCategoryInput)
	protected:
		MouseButtonEvent(const int button) :
			m_Button(button)
		{}
		int m_Button;
	};

	class PTEARTH_API MousePressEvent : public MouseButtonEvent {
	public:
		MousePressEvent(const int button) 
			:MouseButtonEvent(button)
		{}

		virtual std::string ToString() const override {
			std::stringstream outss;
			outss << "MousePressed:( " << m_Button << " )";
			return outss.str();
		}
		EVENT_CLASS_TYPE(EventType::MousePressed)
	};

	class PTEARTH_API MouseReleasedEvent : public MouseButtonEvent {
	public:
		MouseReleasedEvent(const int button)
			:MouseButtonEvent(button)
		{}

		virtual std::string ToString() const override {
			std::stringstream outss;
			outss << "MouseReleased:( " << m_Button << " )";
			return outss.str();
		}
		EVENT_CLASS_TYPE(EventType::MouseReleased)
	};
}