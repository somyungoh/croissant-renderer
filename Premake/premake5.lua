-- premake5.lua

-------------------------------------------------------
--  Global Definition
-------------------------------------------------------

_SRC_DIR = _PREMAKE_DIR.."/../Sources"
_OUT_DIR = _PREMAKE_DIR.."/../bin"
_OBJ_DIR = _PREMAKE_DIR.."/../bin/intermediate"

dofile ("external.lua")


-------------------------------------------------------
--  Croissant Workspace
-------------------------------------------------------

workspace ("Croissant")
    configurations { "Debug", "Release" }
    location ("../Projects/".._TARGET_OS)


-------------------------------------------------------
--  Projects
-------------------------------------------------------

project ("Croissant-Renderer")
    kind ("ConsoleApp")
    language ("C++")

    targetdir (_OUT_DIR)
    objdir (_OBJ_DIR)

    -- GLFW
    includeExternal()
    linkGLFW()

    -- Main source
    files {
        _SRC_DIR.."/*.h",
        _SRC_DIR.."/*.cpp"
    }

    -- macOS
    filter { "system:macosx" }
        defines { "GL_SILENCE_DEPRECATION" }
        links {
            "Cocoa.framework",
            "IOKit.framework",
            "OpenGL.framework",
        }

    filter { "configurations:Debug" }
        defines { "DEBUG" }
        symbols ("On")

    filter { "configurations:Release" }
        defines { "NDEBUG" }
        optimize ("On")

    filter {}

    cppdialect "c++17"
