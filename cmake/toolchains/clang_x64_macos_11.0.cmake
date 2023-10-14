if (DEFINED TOOLCHAIN_X64_MACOS_11_0_CMAKE)
  return ()
else ()
  set(TOOLCHAIN_X64_MACOS_11_0_CMAKE 1)
endif ()

find_program(CMAKE_C_COMPILER clang)
find_program(CMAKE_CXX_COMPILER clang++)

EXECUTE_PROCESS( COMMAND ${CMAKE_CXX_COMPILER} --version OUTPUT_VARIABLE clang_full_version_string )
string (REGEX REPLACE ".*clang version ([0-9]+\\.[0-9]+).*" "\\1" CLANG_VERSION_STRING ${clang_full_version_string})
message(STATUS "Clang version string is ${CLANG_VERSION_STRING}")
string(REGEX MATCH "^([0-9]+)\\.([0-9]+)" CLANG_VERSION_MATCH ${CLANG_VERSION_STRING})
set(CLANG_VERSION_MAJOR ${CMAKE_MATCH_1})
set(CLANG_VERSION_MINOR ${CMAKE_MATCH_2})

set(CMAKE_SYSROOT "/Library/Developer/CommandLineTools/SDKs/MacOSX11.0.sdk"      CACHE PATH    "(macOS only) The path to the macOS platform SDK.")
set(CMAKE_OSX_DEPLOYMENT_TARGET "11.0"                                           CACHE STRING  "(macOS only) The minimum macOS deployment target.")
set(CMAKE_POSITION_INDEPENDENT_CODE CACHE BOOL ON)
set(PORT "UltralightMac")
set(CMAKE_OSX_SYSROOT ${CMAKE_SYSROOT} CACHE PATH "(macOS only) The path to the macOS platform SDK." FORCE)
set(CMAKE_OSX_ARCHITECTURES "x86_64" CACHE STRING "" FORCE)

if (NOT EXISTS ${CMAKE_SYSROOT})
    message(FATAL_ERROR "macOS Platform SDK not found at the following path: ${CMAKE_SYSROOT}")
endif ()

set(UL_SKIA_CFLAGS -arch x86_64 -mmacosx-version-min=11.0)

# Set up FFmpeg args
#set(UL_FFMPEG_ARGS --target-os=darwin --enable-asm --cpu=haswell --sysroot=${CMAKE_SYSROOT})

# Set up GStreamer C flags
#set(UL_GSTREAMER_C_FLAGS --sysroot=${CMAKE_SYSROOT})