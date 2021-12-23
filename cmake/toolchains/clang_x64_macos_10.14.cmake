if (DEFINED TOOLCHAIN_MACOS_10_14_CLANG_CMAKE)
  return ()
else ()
  set(TOOLCHAIN_MACOS_10_14_CLANG_CMAKE 1)
endif ()

set(CMAKE_SYSROOT "/Library/Developer/CommandLineTools/SDKs/MacOSX10.14.sdk"  CACHE PATH    "(macOS only) The path to the macOS platform SDK.")
set(CMAKE_OSX_DEPLOYMENT_TARGET "10.14"                                       CACHE STRING  "(macOS only) The minimum macOS deployment target.")
set(CMAKE_POSITION_INDEPENDENT_CODE CACHE BOOL ON)
set(PORT "UltralightMac")

if (NOT EXISTS ${CMAKE_SYSROOT})
    message(FATAL_ERROR "macOS Platform SDK not found at the following path: ${CMAKE_SYSROOT}")
endif ()

# Set up FFmpeg args
set(UL_FFMPEG_ARGS --target-os=darwin --enable-asm --cpu=haswell --sysroot=${CMAKE_SYSROOT})

# Set up GStreamer C flags
set(UL_GSTREAMER_C_FLAGS --sysroot=${CMAKE_SYSROOT})