#pragma once

#include "EarthCore/Core/PotatoEarthBase.h"
#include "Viewer.h"
#include "Entity.h"
#include <string>
#include <functional>
#include <map>

namespace PTEarth {

    // Script context - provides access to engine API
    class ScriptContext {
    public:
        ScriptContext(Viewer* viewer);
        
        // Access to viewer systems
        Viewer* GetViewer() const { return m_Viewer; }
        EntityCollection& GetEntities() { return m_Viewer->GetEntities(); }
        PT_Ref<Clock> GetClock() { return m_Viewer->GetClock(); }
        PT_Ref<Camera> GetCamera() { return m_Viewer->GetCamera(); }
        
        // Helper functions exposed to scripts
        Entity* CreatePointEntity(const std::string& id, double lon, double lat, double height);
        Entity* CreateLineEntity(const std::string& id, const std::vector<glm::dvec3>& positions);
        void FlyTo(double lon, double lat, double height, float duration);
        void SetTime(double julianDate);
        
    private:
        Viewer* m_Viewer;
    };

    // Script interface
    class ScriptEngine {
    public:
        virtual ~ScriptEngine() = default;
        
        virtual bool Initialize() = 0;
        virtual void Shutdown() = 0;
        
        // Execute script
        virtual bool Execute(const std::string& code) = 0;
        virtual bool ExecuteFile(const std::string& filepath) = 0;
        
        // Bind C++ function to script
        template<typename Func>
        void BindFunction(const std::string& name, Func func);
        
        // Set global variable
        template<typename T>
        void SetGlobal(const std::string& name, const T& value);
        
        // Get global variable
        template<typename T>
        T GetGlobal(const std::string& name);
        
        // Call script function
        template<typename... Args>
        void CallFunction(const std::string& name, Args... args);
    };

    // Lua script engine
    class LuaScriptEngine : public ScriptEngine {
    public:
        LuaScriptEngine();
        ~LuaScriptEngine();
        
        bool Initialize() override;
        void Shutdown() override;
        bool Execute(const std::string& code) override;
        bool ExecuteFile(const std::string& filepath) override;
        
        // Lua-specific
        void* GetLuaState() const { return m_LuaState; }
        
    private:
        void BindEngineAPI();
        
    private:
        void* m_LuaState = nullptr;
    };

    // JavaScript/TypeScript engine (using QuickJS or similar)
    class JSScriptEngine : public ScriptEngine {
    public:
        JSScriptEngine();
        ~JSScriptEngine();
        
        bool Initialize() override;
        void Shutdown() override;
        bool Execute(const std::string& code) override;
        bool ExecuteFile(const std::string& filepath) override;
        
    private:
        void* m_JSContext = nullptr;
        void* m_JSRuntime = nullptr;
    };

    // Script manager
    class ScriptManager {
    public:
        void Initialize(Viewer* viewer);
        void Shutdown();
        
        // Create script engines
        void CreateLuaEngine();
        void CreateJSEngine();
        
        // Execute scripts
        bool RunScript(const std::string& code, const std::string& language = "lua");
        bool RunScriptFile(const std::string& filepath, const std::string& language = "lua");
        
        // Auto-run scripts from directory
        void AutoRunScripts(const std::string& directory);
        
        // Update (for scripts that need per-frame callbacks)
        void Update(float deltaTime);
        
    private:
        Viewer* m_Viewer;
        PT_Ref<ScriptContext> m_Context;
        
        PT_Ref<LuaScriptEngine> m_LuaEngine;
        PT_Ref<JSScriptEngine> m_JSEngine;
        
        std::vector<std::string> m_UpdateCallbacks;
    };

    // CZML loader (Cesium-style JSON data)
    class CZMLLoader {
    public:
        // Load CZML from string or file
        static bool Load(const std::string& czmlData, Viewer* viewer);
        static bool LoadFile(const std::string& filepath, Viewer* viewer);
        
    private:
        static Entity* ParsePacket(const std::string& json, Viewer* viewer);
    };

}
