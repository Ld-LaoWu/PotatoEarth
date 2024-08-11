-- 核心库
project "PotatoEarth"
	cppdialect "C++17"
	-- kind "StaticLib"
	kind "SharedLib" -- dll
	language "C++"
	staticruntime "off" 
 
	targetdir("%{wks.location}/BuildResult/exec/" .. outputdir .. "/%{prj.name}" ) -- 输出的路径
	objdir("%{wks.location}/BuildResult/mid/" .. outputdir .. "/%{prj.name}" ) -- 中间文件输出的路径

	--预编译文件
	pchheader "potatopch.h"
	pchsource "%{wks.location}/PotatoEarth/source/potatopch.cpp"

	defines
	{
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
		"%{wks.location}/PotatoEarth/extern/Glad/include"
	}

	links 
	{
		"Glad",
		"opengl32.lib"
	}

	filter "system:windows"
		cppdialect "C++17"
		systemversion "latest"
		defines
		{
			"PO_PLATFORM_WINDOWS", -- 不同渲染库
			"PT_BUILD_DLL"
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