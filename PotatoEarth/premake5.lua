-- 核心库
project "PotatoEarth"
	-- location "PotatoEarth" -- 位置
	cppdialect "C++17"
	kind "SharedLib" -- dll
	--kind "StaticLib"
	language "C++"
	staticruntime "on"
 
	targetdir("%{wks.location}/BuildResult/exec/" .. outputdir .. "/%{prj.name}" ) -- 输出的路径
	objdir("%{wks.location}/BuildResult/mid/" .. outputdir .. "/%{prj.name}" ) -- 中间文件输出的路径

	--预编译文件
	pchheader "potatopch.h"
	pchsource "%{wks.location}/PotatoEarth/source/potatopch.cpp"

	defines
	{
		"PT_BUILD_DLL"
	}

	files
	{
		"source/**.h",
		"source/**.cpp"
	}

	includedirs
	{
		"source",
		"%{wks.location}/PotatoEarth/extern/glm",
		"%{wks.location}/PotatoEarth/extern/spdlog/include",
		"%{wks.location}/PotatoEarth/extern/Glad/include",
		"%{wks.location}/PotatoEarth/extern/ImGui",
		"%{wks.location}/PotatoEarth/extern/ImGui/backends"
	}

	links 
	{
		"Glad",
		"ImGui",
		"opengl32.lib"
	}

	filter "system:windows"
		cppdialect "C++17"
		systemversion "latest"
        -- buildoptions { "/wd4251" }
		defines
		{
			"PTEARTH_PLATFORM_WINDOWS", -- 不同渲染库
			"PTEARTH_DYNAMIC_LINK"
		}
		
		postbuildcommands
		{
			("{COPY} %{cfg.buildtarget.relpath} \"../BuildResult/exec/" .. outputdir .. "/PotatoEarth-Qt/\"")
		}

	filter "configurations:Debug"
		defines { "PO_DEBUG" }
		runtime "Debug"
		symbols "On"

	filter "configurations:Release"
		defines { "PO_RELEASE" }
		runtime "Release"
		optimize "On"

	filter "configurations:Dist"
		defines { "PO_DIST" }
		runtime "Release"
		optimize "On"