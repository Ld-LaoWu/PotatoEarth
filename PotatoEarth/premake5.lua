-- ���Ŀ�
project "PotatoEarth"
	-- location "PotatoEarth" -- λ��
	cppdialect "C++17"
	kind "SharedLib" -- dll
	--kind "StaticLib"
	language "C++"
	staticruntime "on"
 
	targetdir("%{wks.location}/BuildResult/exec/" .. outputdir .. "/%{prj.name}" ) -- �����·��
	objdir("%{wks.location}/BuildResult/mid/" .. outputdir .. "/%{prj.name}" ) -- �м��ļ������·��

	--Ԥ�����ļ�
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
			"PTEARTH_PLATFORM_WINDOWS", -- ��ͬ��Ⱦ��
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