-- premake5.lua
workspace ("Croissant")
    configurations { "Debug", "Release" }
    location ("../Projects/".._TARGET_OS)

project ("Croissant-Renderer")
    kind ("ConsoleApp")
    language ("C++")
    targetdir ("bin/%{cfg.buildcfg}")

    files { "**.h", "**.cpp" }

    filter { "configurations:Debug" }
        defines { "DEBUG" }
        symbols ("On")

    filter { "configurations:Release" }
        defines { "NDEBUG" }
        optimize ("On")