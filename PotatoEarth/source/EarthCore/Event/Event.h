#pragma once

#include "potatopch.h"
#include "EarthCore/Core/PotatoEarthBase.h"

namespace PTEarth {

    enum class EventType
    {
        None = 0,
        WindowClosed, WindowResized,
        MousePressed, MouseReleased,
        MouseMoved, MouseWheeled,
        KeyPressed, KeyReleased
    };

    enum EventCategory
    {
        None = 0,
        EventCategoryInput       = BIT(0),
        EventCategoryKeyboard    = BIT(1),
        EventCategoryMouse       = BIT(2),
        EventCategoryMouseButton = BIT(3),
        EventCategoryWindow      = BIT(4)
    };

#define EVENT_CLASS_TYPE(type) static EventType GetStaticEventType() { return EventType::type; } \
                                virtual EventType GetEventType() const override { return GetStaticEventType(); } \
                                virtual const char* GetEventName() const override { return #type; }

#define EVENT_CLASS_CATEGORY(category) virtual int GetCategoryFlags() const override { return category; }

    class PTEARTH_API Event {
    public:
        virtual ~Event() = default;
        
        virtual EventType GetEventType() const = 0;
        virtual const char* GetEventName() const = 0;
        virtual int GetCategoryFlags() const = 0;
        virtual std::string ToString() const { return GetEventName(); }

        inline bool IsInCategory(EventCategory category) {
            return GetCategoryFlags() & category;
        }

        inline bool IsHandled() const { return m_Handled; }

    protected:
        bool m_Handled = false;
        friend class EventDispatcher;
    };

    class PTEARTH_API EventDispatcher {
    public:
        template<typename T>
        using EventFn = std::function<bool(T&)>;
        
        EventDispatcher(Event& event) : m_Event(event) {}
        
        template<typename T>
        bool Dispatch(EventFn<T> func) {
            if (m_Event.GetEventType() == T::GetStaticEventType()) {
                m_Event.m_Handled = func(*(T*)&m_Event);
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
