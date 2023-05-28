-- 核心库
project "PotatoEarth"
	cppdialect "C++17"
	kind "StaticLib"
	language "C++"
	staticruntime "off"

	targetdir("%{wks.location}/BuildResult/exec/" .. outputdir .. "/%{prj.name}" ) -- 输出的路径
	objdir("%{wks.location}/BuildResult/mid/" .. outputdir .. "/%{prj.name}" ) -- 中间文件输出的路径

	--预编译文件
	pchheader "potatopch.h"
	pchsource "potatopch.cpp"

	defines
	{
	}
	files
	{
		"**.h",
		"**.cpp",
		"source/**.h",
		"source/**.cpp"
	}

	includedirs
	{
		"./",
		"source"
	}

	links 
	{
	}

	filter "system:windows"
		systemversion "latest"
		defines
		{
			"PO_PLATFORM_WINDOWS" -- 不同渲染库
		}

	filter "configurations:Debug"
		defines { "PO_DEBUG" }
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines { "PO_RELEASE" }
		runtime "Release"
		optimize "On"

	filter "configurations:Dist"
		defines { "PO_DIST" }
		runtime "Release"
		optimize "on"