
-- GLFW
function includeGLFW()
	includedirs { _SRC_DIR.."/External/GLFW/include" }
end

function linkGLFW()
    libdirs { _SRC_DIR.."/External/GLFW/lib" }
    links { "glfw3" }
end