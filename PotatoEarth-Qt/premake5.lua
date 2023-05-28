-- Qt界面
include "../Premake/premake-Qt/qt.lua"
local qt = premake.extensions.qt

project "PotatoEarth-Qt"
	cppdialect "C++17"
	kind "ConsoleApp"
	language "C++"
	targetdir("%{wks.location}/BuildResult/exec/" .. outputdir .. "/%{prj.name}" ) -- 输出的路径
	objdir("%{wks.location}BuildResult/mid/" .. outputdir .. "/%{prj.name}" ) -- 中间文件输出的路径


	includedirs
	{
		"source",
		"%{wks.location}/BuildResult/mid/%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}/%{prj.name}",
		"%{wks.location}/PotatoEarth/source"
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

	qt.enable()
	qtuseexternalinclude ( true )
	qtpath "E:/software/programDev/Qt/5.15.2/msvc2019_64"

	qtmodules 
	{ 
		"core", 
		"gui", 
		"widgets", 
		"opengl" 
	}

	qtprefix "Qt5"

	filter "system:windows"
		systemversion "latest"

	filter "configurations:Debug"
		defines { "PO_DEBUG" }
		qtsuffix "d"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines { "PO_RELEASE" }
		runtime "Release"
		optimize "On"

	filter "configurations:Dist"
		defines { "PO_DIST" }
		kind "WindowedApp"
		runtime "Release"
		optimize "on"