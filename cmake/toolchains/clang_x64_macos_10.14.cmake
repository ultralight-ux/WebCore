if (DEFINED TOOLCHAIN_MACOS_10_14_CLANG_CMAKE)
  return ()
else ()
  set(TOOLCHAIN_MACOS_10_14_CLANG_CMAKE 1)
endif ()

find_program(CMAKE_C_COMPILER /usr/local/opt/llvm@11/bin/clang)
find_program(CMAKE_CXX_COMPILER /usr/local/opt/llvm@11/bin/clang++)

EXECUTE_PROCESS( COMMAND ${CMAKE_CXX_COMPILER} --version OUTPUT_VARIABLE clang_full_version_string )
string (REGEX REPLACE ".*clang version ([0-9]+\\.[0-9]+).*" "\\1" CLANG_VERSION_STRING ${clang_full_version_string})
message(STATUS "Clang version string is ${CLANG_VERSION_STRING}")
string(REGEX MATCH "^([0-9]+)\\.([0-9]+)" CLANG_VERSION_MATCH ${CLANG_VERSION_STRING})
set(CLANG_VERSION_MAJOR ${CMAKE_MATCH_1})
set(CLANG_VERSION_MINOR ${CMAKE_MATCH_2})

set(CMAKE_SYSROOT "/Library/Developer/CommandLineTools/SDKs/MacOSX10.14.sdk"      CACHE PATH    "(macOS only) The path to the macOS platform SDK.")
set(CMAKE_OSX_DEPLOYMENT_TARGET "10.14"                                           CACHE STRING  "(macOS only) The minimum macOS deployment target.")
set(CMAKE_POSITION_INDEPENDENT_CODE CACHE BOOL ON)
set(PORT "UltralightMac")
set(CMAKE_OSX_SYSROOT ${CMAKE_SYSROOT} CACHE PATH "(macOS only) The path to the macOS platform SDK." FORCE)

if (NOT CLANG_VERSION_MAJOR VERSION_EQUAL 11)
    message(FATAL_ERROR "The macOS build toolchain requires LLVM @ 11.0 to be installed via homebrew.")
endif ()

if (NOT EXISTS ${CMAKE_SYSROOT})
    message(FATAL_ERROR "macOS Platform SDK not found at the following path: ${CMAKE_SYSROOT}")
endif ()

# Set up FFmpeg args
set(UL_FFMPEG_ARGS --target-os=darwin --enable-asm --cpu=haswell --sysroot=${CMAKE_SYSROOT})

# Set up GStreamer C flags
set(UL_GSTREAMER_C_FLAGS --sysroot=${CMAKE_SYSROOT})