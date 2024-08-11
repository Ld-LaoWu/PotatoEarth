project "Glad"
    kind "StaticLib"
    language "C"
    staticruntime "on"
    
	targetdir("%{wks.location}/BuildResult/exec/" .. outputdir .. "/%{prj.name}" ) -- 输出的路径
	objdir("%{wks.location}/BuildResult/mid/" .. outputdir .. "/%{prj.name}" ) -- 中间文件输出的路径

	files
	{
        "include/glad/**.h",
        "include/KHR/**.h",
        "src/**.c"
    }

    includedirs
    {
        "include"
    }
    
	filter "system:windows"
        systemversion "latest"
        
    filter "configurations:Debug"
		runtime "Debug"
		symbols "on"
	filter "configurations:Release"
		runtime "Release"
		symbols "on"