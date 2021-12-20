if (DEFINED MSVC_X86_WINDOWS_CMAKE)
  return ()
else ()
  set (MSVC_X86_WINDOWS_CMAKE 1)
endif ()

include(${CMAKE_CURRENT_LIST_DIR}/../common.cmake)
include(add_cache_flag)
include(msvc_runtime_library_to_meson_flags)
include(cl)

set(CMAKE_SYSTEM "Windows-10.0.19043")
set(CMAKE_SYSTEM_NAME "Windows")
set(CMAKE_SYSTEM_VERSION "10.0.19043")
set(CMAKE_SYSTEM_PROCESSOR "x86")
set(CMAKE_CROSSCOMPILING "FALSE")
set(CMAKE_POLICY_DEFAULT_CMP0091 NEW)
set(PORT "UltralightWin")

message(STATUS "MSVC runtime library: '${CMAKE_MSVC_RUNTIME_LIBRARY}'")

# Set up FFmpeg configure args
set(UL_FFMPEG_ARGS --toolchain=msvc --enable-asm --cpu=haswell --target-os=win32 --arch=x86)

# Set up GStreamer C flags
# set(UL_GSTREAMER_C_FLAGS ...)

# Set up GStreamer link flags
# set(UL_GSTREAMER_LINK_FLAGS ...)

# Set up GStreamer Meson flags (passed to meson command line)
msvc_runtime_library_to_meson_flags(UL_GSTREAMER_MESON_FLAGS ${CMAKE_MSVC_RUNTIME_LIBRARY})

# Set up Skia C flags
set(UL_SKIA_CFLAGS -march=core2 -m32)

get_filename_component(WIN_PLATFORM_DEFINES_H "win_platform_defines.h" REALPATH BASE_DIR "${CMAKE_CURRENT_LIST_DIR}")

add_cache_flag(CMAKE_C_FLAGS_INIT "/FI ${WIN_PLATFORM_DEFINES_H}")                       
add_cache_flag(CMAKE_CXX_FLAGS_INIT "/FI ${WIN_PLATFORM_DEFINES_H}")
