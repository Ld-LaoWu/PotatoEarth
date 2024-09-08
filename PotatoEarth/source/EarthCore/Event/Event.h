#pragma once
#include "potatopch.h"
#include "EarthCore/Core/PotatoEarthBase.h"


// �����¼���
namespace PTEarth {

	// �¼�����
	enum class EventType
	{
		None = 0,
		WindowClosed, WindowResized,
		// ���
		MousePressed, MouseReleased,   //������ֵ
		MouseMoved,MouseWheeled, // 
		// ���������¼�
		KeyPressed, KeyReleased
		// ����

		// ����
	};

	// �¼�����
	enum EventCategory
	{
		None = 0,
		EventCategoryInput       = BIT(0),
		EventCategoryKeyboard    = BIT(1),
		EventCategoryMouse       = BIT(2),
		EventCategoryMouseButton = BIT(3),
		EventCategoryWindow      = BIT(4)
	};


	// ʹ�ú��
#define EVENT_CLASS_TYPE(type) static EventType GetStaticEventType() { return EventType::type; }\
                                virtual EventType GetEventType() const override { return GetStaticEventType();}\
								virtual const char* GetEventName() const override { return #type;}


#define EVENT_CLASS_CATEGORY(category) virtual int GetCategoryFlags() const override { return category; }


	class PTEARTH_API Event {

	public:
		virtual EventType GetEventType() const = 0;
		virtual const char* GetEventName() const = 0;
		virtual int GetCategoryFlags() const = 0;
		virtual std::string ToString() const { return GetEventName(); }

		inline bool IsInCategory( EventCategory categoroy ) {
			return GetCategoryFlags() & categoroy;
		}

	private:

		bool m_Handled = false;
		friend class EventDispatcher;
		
	};

	// �¼��ַ�
	class PTEARTH_API EventDispatcher {
		template<typename T>
		using EventFn = std::function<bool(T&)>;
	public:
		EventDispatcher(Event& event)
		 :m_Event(event)
		{}
		template< typename T>
		bool Dispatch(EventFn<T> func) {
			if (m_Event.GetEventType() == T::GetStaticEventType()) {
				m_Event.m_Handled = func(static_cast<T&>(m_Event));
				return true;
			}
			return false;
		}
	private:
		Event& m_Event;
	};

	inline std::ostream& operator<<(std::ostream& os, const Event& e) {
		return os << e.ToString();
	}
}