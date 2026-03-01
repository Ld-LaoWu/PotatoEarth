-- Qt����
include "../Premake/premake-Qt/qt.lua"
local qt = premake.extensions.qt

project "PotatoEarth-Qt"
	-- location "PotatoEarth-Qt" -- λ��
	cppdialect "C++17"
	kind "ConsoleApp"
	language "C++"
	targetdir("%{wks.location}/BuildResult/exec/" .. outputdir .. "/%{prj.name}" ) -- �����·��
	objdir("%{wks.location}/BuildResult/mid/" .. outputdir .. "/%{prj.name}" ) -- �м��ļ������·��


	includedirs
	{
		"source",
		"%{wks.location}/BuildResult/mid/%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}/%{prj.name}",
		"%{wks.location}/PotatoEarth/source",
		"%{wks.location}/PotatoEarth/extern/spdlog/include",
		"%{wks.location}/PotatoEarth/extern/glm",
		"%{wks.location}/PotatoEarth/extern/ImGui",
		"%{wks.location}/PotatoEarth/extern/ImGui/backends",
		"%{wks.location}/PotatoEarth/extern/Glad/include"
	}
	-- add the files
	files 
	{
		"**.cpp",
		"source/**.h",
		"source/**.cpp",
		"source/**.ts",
		"source/**.ui",
		"source/**.qrc"
	}

	links
	{
		"PotatoEarth"
	}

	qt.enable()
	qtuseexternalinclude ( true )
	qtpath "E:/InstallSoftware/ProgramDev/Qt/5.12.12/msvc2017_64"

	qtmodules 
	{ 
		"core", 
		"gui", 
		"widgets"
		-- "opengl" 
	}

	qtprefix "Qt5"

	filter "system:windows"
		systemversion "latest"
		libdirs { "%{wks.location}/BuildResult/exec/" .. outputdir .. "/%{prj.name}" }
		defines
		{
			"PTEARTH_PLATFORM_WINDOWS", -- ��ͬ��Ⱦ��
			"PTEARTH_DYNAMIC_LINK"
		}

	filter "configurations:Debug"
		defines { "PO_DEBUG" }
		qtsuffix "d"
		runtime "Debug"
		symbols "On"

	filter "configurations:Release"
		defines { "PO_RELEASE" }
		runtime "Release"
		optimize "On"

	filter "configurations:Dist"
		defines { "PO_DIST" }
		kind "WindowedApp"
		runtime "Release"
		optimize "On"