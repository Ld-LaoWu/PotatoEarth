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

group "PT3rd"
	include "PotatoEarth/extern/Glad"
	include "PotatoEarth/extern/ImGui"
group ""

group "PTCore"
	include "PotatoEarth"
group ""
	
group "PTDeskTop"
	include "PotatoEarth-Qt"
group ""