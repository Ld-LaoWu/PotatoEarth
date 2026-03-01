-- PotatoEarth Project
project "PotatoEarth"
	cppdialect "C++17"
	kind "SharedLib"
	language "C++"
	staticruntime "on"
 
	targetdir("%{wks.location}/BuildResult/exec/" .. outputdir .. "/%{prj.name}")
	objdir("%{wks.location}/BuildResult/mid/" .. outputdir .. "/%{prj.name}")

	pchheader "potatopch.h"
	pchsource "%{wks.location}/PotatoEarth/source/potatopch.cpp"

	flags { "MultiProcessorCompile" }
	buildoptions { "/utf-8", "/wd4251" }

	defines
	{
		"PTEARTH_PLATFORM_WINDOWS",
		"PT_BUILD_DLL",
		"_CRT_SECURE_NO_WARNINGS"
	}

	files
	{
		"source/**.h",
		"source/**.cpp"
	}

	includedirs
	{
		"%{wks.location}/PotatoEarth/extern/Glad/include",
		"%{wks.location}/PotatoEarth/extern/stb",
		"%{wks.location}/PotatoEarth/extern/glm",
		"%{wks.location}/PotatoEarth/extern/spdlog/include",
		"source"
	}

	links
	{
		"Glad",
		"ImGui"
	}

	filter "configurations:Debug"
		defines "PT_DEBUG"
		symbols "On"

	filter "configurations:Release"
		defines "PT_RELEASE"
		optimize "On"

	filter "configurations:Dist"
		defines "PT_DIST"
		optimize "On"

	filter {}

