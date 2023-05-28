-- ���Ŀ�
project "PotatoEarth"
	cppdialect "C++17"
	kind "StaticLib"
	language "C++"
	staticruntime "off"

	targetdir("%{wks.location}/BuildResult/exec/" .. outputdir .. "/%{prj.name}" ) -- �����·��
	objdir("%{wks.location}/BuildResult/mid/" .. outputdir .. "/%{prj.name}" ) -- �м��ļ������·��

	--Ԥ�����ļ�
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
			"PO_PLATFORM_WINDOWS" -- ��ͬ��Ⱦ��
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