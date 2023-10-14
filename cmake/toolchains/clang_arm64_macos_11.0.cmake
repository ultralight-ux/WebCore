if (DEFINED TOOLCHAIN_ARM64_MACOS_11_0_CMAKE)
  return ()
else ()
  set(TOOLCHAIN_ARM64_MACOS_11_0_CMAKE 1)
endif ()

set(CMAKE_SYSTEM_NAME Darwin CACHE INTERNAL "" FORCE)
set(CMAKE_SYSTEM_PROCESSOR arm64 CACHE INTERNAL "" FORCE)
set(CMAKE_CROSSCOMPILING TRUE CACHE INTERNAL "" FORCE)
set(UNIX TRUE CACHE INTERNAL "" FORCE)
set(APPLE TRUE CACHE INTERNAL "" FORCE)
set(MSVC FALSE CACHE INTERNAL "" FORCE)

set(triple arm64-apple-macos11)

set(CMAKE_C_COMPILER_TARGET ${triple} CACHE INTERNAL "" FORCE)
set(CMAKE_CXX_COMPILER_TARGET ${triple} CACHE INTERNAL "" FORCE)
set(CMAKE_ASM_COMPILER_TARGET ${triple} CACHE INTERNAL "" FORCE)

find_program(CMAKE_C_COMPILER clang)
find_program(CMAKE_CXX_COMPILER clang++)

EXECUTE_PROCESS( COMMAND ${CMAKE_CXX_COMPILER} --version OUTPUT_VARIABLE clang_full_version_string )
string (REGEX REPLACE ".*clang version ([0-9]+\\.[0-9]+).*" "\\1" CLANG_VERSION_STRING ${clang_full_version_string})
message(STATUS "Clang version string is ${CLANG_VERSION_STRING}")
string(REGEX MATCH "^([0-9]+)\\.([0-9]+)" CLANG_VERSION_MATCH ${CLANG_VERSION_STRING})
set(CLANG_VERSION_MAJOR ${CMAKE_MATCH_1})
set(CLANG_VERSION_MINOR ${CMAKE_MATCH_2})

set(CMAKE_SYSROOT "/Library/Developer/CommandLineTools/SDKs/MacOSX11.0.sdk"      CACHE PATH    "(macOS only) The path to the macOS platform SDK." FORCE)
set(CMAKE_OSX_DEPLOYMENT_TARGET "11.0"                                           CACHE STRING  "(macOS only) The minimum macOS deployment target." FORCE)
set(CMAKE_POSITION_INDEPENDENT_CODE CACHE BOOL ON FORCE)
set(PORT "UltralightMac" CACHE INTERNAL "" FORCE)
set(CMAKE_OSX_SYSROOT ${CMAKE_SYSROOT} CACHE PATH "(macOS only) The path to the macOS platform SDK." FORCE)
set(CMAKE_OSX_ARCHITECTURES "arm64" CACHE STRING "" FORCE)

if (NOT EXISTS ${CMAKE_SYSROOT})
    message(FATAL_ERROR "macOS Platform SDK not found at the following path: ${CMAKE_SYSROOT}")
endif ()

set(SKIA_CPU "arm64")
set(UL_SKIA_CFLAGS -arch arm64 -mmacosx-version-min=11.0)

# Set up FFmpeg args
#set(UL_FFMPEG_ARGS --target-os=darwin --enable-asm --cpu=haswell --sysroot=${CMAKE_SYSROOT})

# Set up GStreamer C flags
#set(UL_GSTREAMER_C_FLAGS --sysroot=${CMAKE_SYSROOT})