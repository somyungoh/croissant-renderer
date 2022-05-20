-- premake5.lua
workspace ("Croissant")
    configurations { "Debug", "Release" }
    location ("../Projects/".._TARGET_OS)

project ("Croissant-Renderer")
    kind ("ConsoleApp")
    language ("C++")

    local out_dir = "../bin"
    local obj_dir = "../bin/intermediate"
    local src_dir = "../Sources"

    targetdir (out_dir)
    objdir (obj_dir)

    -- GLFW
    includedirs { src_dir.."/External/GLFW/include" }
    libdirs { src_dir.."/External/GLFW/lib" }
    links { "glfw3" }

    filter { "system:macosx" }
        links {
            "Cocoa.framework",
            "IOKit.framework",
            "OpenGL.framework",
        }

    -- Main source
    files {
        src_dir.."/*.h",
        src_dir.."/*.cpp"
    }

    -- macOS
    filter { "system:macosx" }
        defines { "GL_SILENCE_DEPRECATION" }

    filter { "configurations:Debug" }
        defines { "DEBUG" }
        symbols ("On")

    filter { "configurations:Release" }
        defines { "NDEBUG" }
        optimize ("On")