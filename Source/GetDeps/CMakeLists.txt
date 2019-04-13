cmake_minimum_required(VERSION 3.3.2)
project(GetDeps)

include(${CMAKE_ROOT}/Modules/ExternalProject.cmake)

# -----------------------------------------------------------------------------
# Determine the operating system and port
# -----------------------------------------------------------------------------
if (UNIX)
    if (APPLE)
		set(PORT UltralightMac)
    else ()
		set(PORT UltralightLinux)
    endif ()
elseif (CMAKE_SYSTEM_NAME MATCHES "Windows")
	set(PORT UltralightWin)
else ()
    message(FATAL_ERROR "Unknown OS '${CMAKE_SYSTEM_NAME}'")
endif ()

include(../../Deps.cmake)
add_executable(dummy dummy.c)
add_dependencies(dummy WebCoreDeps UltralightCoreBin)
