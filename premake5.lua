workspace "Minecraft"
	architecture "x86_64"
	startproject "Minecraft"

	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

-- Include directories relative to root folder (solution directory)
IncludeDir = {}
IncludeDir["GLFW"] = "Minecraft/Dependencies/GLFW/include"
IncludeDir["Glad"] = "Minecraft/Dependencies/Glad/include"
IncludeDir["glm"] = "Minecraft/Dependencies/glm"
IncludeDir["stb_image"] = "Minecraft/Dependencies/stb_image"
IncludeDir["PerlinNoise"] = "Minecraft/Dependencies/PerlinNoise"

group "Dependencies"
	include "Minecraft/Dependencies/GLFW"
	include "Minecraft/Dependencies/Glad"
group ""

project "Minecraft"
	location "Minecraft"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("obj/" .. outputdir .. "/%{prj.name}")

	pchheader "pch.h"
	pchsource "Minecraft/src/pch.cpp"

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
		"%{prj.name}/Dependencies/stb_image/**.h",
		"%{prj.name}/Dependencies/stb_image/**.cpp",
		"%{prj.name}/Dependencies/glm/glm/**.hpp",
		"%{prj.name}/Dependencies/glm/glm/**.inl",
		"%{prj.name}/Dependencies/PerlinNoise/PerlinNoise.h"
	}

	includedirs
	{
		"%{prj.name}/src",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.Glad}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.stb_image}",
		"%{IncludeDir.PerlinNoise}"
	}

	links
	{
		"GLFW",
		"Glad",
		"opengl32.lib"
	}

	filter "system:windows"
		systemversion "latest"

		defines
		{
			"GLFW_INCLUDE_NONE"
		}

	filter "configurations:Debug"
		defines "DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "RELEASE"
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		defines "DIST"
		runtime "Release"
		optimize "on"