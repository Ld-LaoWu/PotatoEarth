-- 解决方案名称

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

workspace "PotatoEarth"
	architecture "x64"
	startproject "PotatoEarth-Qt"

	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}


group "PTCore"
	include "PotatoEarth"
group ""
	
group "DeskTop"
	include "PotatoEarth-Qt"
group ""