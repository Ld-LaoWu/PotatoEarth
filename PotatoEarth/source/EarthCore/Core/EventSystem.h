#pragma once

#include "EarthCore/Core/PotatoEarthBase.h"
#include <functional>
#include <vector>
#include <map>

namespace PTEarth {

    // Mouse buttons
    enum class MouseButton {
        Left = 0,
        Right = 1,
        Middle = 2,
        None = 3
    };

    // Event types
    enum class EventType {
        None = 0,
        MouseMove,
        MouseDown,
        MouseUp,
        MouseWheel,
        KeyDown,
        KeyUp,
        Click,
        DoubleClick,
        CameraChanged,
        EntitySelected,
        EntityHovered,
        WindowResize
    };

    // Base event
    struct Event {
        EventType Type = EventType::None;
        bool Handled = false;
    };

    // Mouse events
    struct MouseEvent : public Event {
        double X = 0.0;
        double Y = 0.0;
        double DeltaX = 0.0;
        double DeltaY = 0.0;
        MouseButton Button = MouseButton::None;
        bool Shift = false;
        bool Ctrl = false;
        bool Alt = false;
    };

    struct MouseWheelEvent : public Event {
        double Delta = 0.0;
        double X = 0.0;
        double Y = 0.0;
    };

    // Keyboard events
    struct KeyEvent : public Event {
        int KeyCode = 0;
        bool Repeat = false;
        bool Shift = false;
        bool Ctrl = false;
        bool Alt = false;
    };

    // Camera event
    struct CameraChangedEvent : public Event {
        glm::dvec3 Position;
        glm::dvec3 Direction;
        double Height;
    };

    // Entity events
    struct EntitySelectedEvent : public Event {
        std::string EntityID;
    };

    struct EntityHoveredEvent : public Event {
        std::string EntityID;
        bool Entered;  // true = enter, false = leave
    };

    // Window event
    struct WindowResizeEvent : public Event {
        int Width;
        int Height;
    };

    // Event handler type
    template<typename T>
    using EventHandler = std::function<void(const T&)>;

    // Event dispatcher
    class EventDispatcher {
    public:
        // Subscribe to events
        template<typename T>
        void Subscribe(EventHandler<T> handler) {
            auto& handlers = GetHandlers<T>();
            handlers.push_back([handler](const Event& e) {
                handler(static_cast<const T>&(e));
            });
        }

        // Emit event
        template<typename T>
        void Emit(const T& event) {
            auto& handlers = GetHandlers<T>();
            for (auto& handler : handlers) {
                handler(event);
                if (event.Handled) break;
            }
        }

        // Clear all handlers
        void Clear() {
            m_Handlers.clear();
        }

    private:
        template<typename T>
        std::vector<std::function<void(const Event&)>>& GetHandlers() {
            return m_Handlers[typeid(T).hash_code()];
        }

    private:
        std::map<size_t, std::vector<std::function<void(const Event&)>>> m_Handlers;
    };

    // Screen space event handler - handles picking and interaction
    class ScreenSpaceEventHandler {
    public:
        ScreenSpaceEventHandler(class Viewer* viewer);

        void Initialize();
        void Shutdown();

        // Input events from window
        void OnMouseMove(double x, double y);
        void OnMouseDown(MouseButton button, double x, double y);
        void OnMouseUp(MouseButton button, double x, double y);
        void OnMouseWheel(double delta, double x, double y);
        void OnKeyDown(int keyCode);
        void OnKeyUp(int keyCode);

        // Picking
        // Entity* PickEntity(double x, double y);
        // Cartographic PickPosition(double x, double y);

        // Event subscriptions
        void SetInputAction(EventHandler<MouseEvent> handler, EventType type, MouseButton button = MouseButton::None);
        void SetInputAction(EventHandler<KeyEvent> handler, EventType type);

    private:
        class Viewer* m_Viewer;
        EventDispatcher m_Dispatcher;
        
        double m_LastMouseX = 0.0;
        double m_LastMouseY = 0.0;
        bool m_IsDragging = false;
        MouseButton m_DragButton = MouseButton::None;
    };

}
