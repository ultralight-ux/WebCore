cmake_minimum_required(VERSION 2.8.12)
project(WebCore)

# This should be removed once we require CMake 3.3.
if (POLICY CMP0058)
    cmake_policy(SET CMP0058 NEW)
endif ()

if(NOT ${CMAKE_VERSION} VERSION_LESS "3.1")
    cmake_policy(SET CMP0054 NEW)
endif()

include(${CMAKE_ROOT}/Modules/ExternalProject.cmake)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/Source/cmake")

set(ENABLE_WEBCORE ON)
set(ENABLE_WEBKIT OFF)
set(ENABLE_TOOLS OFF)

# -----------------------------------------------------------------------------
# Determine the target processor
# -----------------------------------------------------------------------------
# Use MSVC_CXX_ARCHITECTURE_ID instead of CMAKE_SYSTEM_PROCESSOR when defined,
# since the later one just resolves to the host processor on Windows.
if (MSVC_CXX_ARCHITECTURE_ID)
    string(TOLOWER ${MSVC_CXX_ARCHITECTURE_ID} LOWERCASE_CMAKE_SYSTEM_PROCESSOR)
else ()
    string(TOLOWER ${CMAKE_SYSTEM_PROCESSOR} LOWERCASE_CMAKE_SYSTEM_PROCESSOR)
endif ()
if (LOWERCASE_CMAKE_SYSTEM_PROCESSOR MATCHES "^arm")
    set(WTF_CPU_ARM 1)
elseif (LOWERCASE_CMAKE_SYSTEM_PROCESSOR MATCHES "^aarch64")
    set(WTF_CPU_ARM64 1)
elseif (LOWERCASE_CMAKE_SYSTEM_PROCESSOR MATCHES "^mips")
    set(WTF_CPU_MIPS 1)
elseif (LOWERCASE_CMAKE_SYSTEM_PROCESSOR MATCHES "sh4")
    set(WTF_CPU_SH4 1)
elseif (LOWERCASE_CMAKE_SYSTEM_PROCESSOR MATCHES "(x64|x86_64|amd64)")
    set(WTF_CPU_X86_64 1)
elseif (LOWERCASE_CMAKE_SYSTEM_PROCESSOR MATCHES "(i[3-6]86|x86)")
    set(WTF_CPU_X86 1)
elseif (LOWERCASE_CMAKE_SYSTEM_PROCESSOR MATCHES "ppc")
    set(WTF_CPU_PPC 1)
elseif (LOWERCASE_CMAKE_SYSTEM_PROCESSOR MATCHES "ppc64")
    set(WTF_CPU_PPC64 1)
elseif (LOWERCASE_CMAKE_SYSTEM_PROCESSOR MATCHES "ppc64le")
    set(WTF_CPU_PPC64LE 1)
elseif (LOWERCASE_CMAKE_SYSTEM_PROCESSOR MATCHES "parisc*")
    set(WTF_CPU_HPPA 1)
elseif (LOWERCASE_CMAKE_SYSTEM_PROCESSOR MATCHES "s390")
    set(WTF_CPU_S390 1)
elseif (LOWERCASE_CMAKE_SYSTEM_PROCESSOR MATCHES "s390x")
    set(WTF_CPU_S390X 1)
else ()
    message(FATAL_ERROR "Unknown CPU '${LOWERCASE_CMAKE_SYSTEM_PROCESSOR}'")
endif ()

# -----------------------------------------------------------------------------
# Determine the operating system and port
# -----------------------------------------------------------------------------
if (UNIX)
    if (APPLE)
        set(WTF_OS_MAC_OS_X 1)
		set(PORT UltralightMac)
    else ()
        set(WTF_OS_UNIX 1)
		set(PORT UltralightLinux)
    endif ()
elseif (CMAKE_SYSTEM_NAME MATCHES "Windows")
    set(WTF_OS_WINDOWS 1)
	set(PORT UltralightWin)
else ()
    message(FATAL_ERROR "Unknown OS '${CMAKE_SYSTEM_NAME}'")
endif ()

if (PORT MATCHES "UltralightLinux")
    ExternalProject_Add(WebCoreDeps
      URL https://github.com/ultralight-ux/WebCore-deps/releases/download/v1.0/webcore-deps-bin-1.0-linux.tgz
      URL_MD5 a661c1422127a63d528d572e10f895e6
      SOURCE_DIR "${CMAKE_SOURCE_DIR}/deps/WebKitLibraries/"
      BUILD_IN_SOURCE 1
      CONFIGURE_COMMAND ""
      BUILD_COMMAND ""
      INSTALL_COMMAND ""
    )
    ExternalProject_Add(UltralightCoreBin
      URL https://github.com/ultralight-ux/UltralightCore-bin/releases/download/r24a222c/ultralightcore-bin-24a222c-linux.tgz
      URL_MD5 053806e3072b126360ae62e028a78bff
      SOURCE_DIR "${CMAKE_SOURCE_DIR}/deps/UltralightCore/"
      BUILD_IN_SOURCE 1
      CONFIGURE_COMMAND ""
      BUILD_COMMAND ""
      INSTALL_COMMAND ""
    )
elseif (PORT MATCHES "UltralightMac")
    ExternalProject_Add(WebCoreDeps
      URL https://github.com/ultralight-ux/WebCore-deps/releases/download/v1.0/webcore-deps-bin-1.0-mac.tgz
      URL_MD5 946e6c0bb2490e343181f9ae28314ac5
      SOURCE_DIR "${CMAKE_SOURCE_DIR}/deps/WebKitLibraries/"
      BUILD_IN_SOURCE 1
      CONFIGURE_COMMAND ""
      BUILD_COMMAND ""
      INSTALL_COMMAND ""
    )
    ExternalProject_Add(UltralightCoreBin
      URL https://github.com/ultralight-ux/UltralightCore-bin/releases/download/r24a222c/ultralightcore-bin-24a222c-mac.tgz
      URL_MD5 b4d91a48c527a9eb06421bf1ca98575f
      SOURCE_DIR "${CMAKE_SOURCE_DIR}/deps/UltralightCore/"
      BUILD_IN_SOURCE 1
      CONFIGURE_COMMAND ""
      BUILD_COMMAND ""
      INSTALL_COMMAND ""
    )
elseif (PORT MATCHES "UltralightWin")
    ExternalProject_Add(WebCoreDeps
      URL https://github.com/ultralight-ux/WebCore-deps/releases/download/v1.0/webcore-deps-bin-1.0-win.zip
      URL_MD5 3e4e28ffc8ee9277285340942c6c21a1
      SOURCE_DIR "${CMAKE_SOURCE_DIR}/deps/WebKitLibraries/"
      BUILD_IN_SOURCE 1
      CONFIGURE_COMMAND ""
      BUILD_COMMAND ""
      INSTALL_COMMAND ""
    )
    ExternalProject_Add(UltralightCoreBin
      URL https://github.com/ultralight-ux/UltralightCore-bin/releases/download/r24a222c/ultralightcore-bin-24a222c-win.zip
      URL_MD5 de7ee483da84f8a30bfb1e76fc4a3930
      SOURCE_DIR "${CMAKE_SOURCE_DIR}/deps/UltralightCore/"
      BUILD_IN_SOURCE 1
      CONFIGURE_COMMAND ""
      BUILD_COMMAND ""
      INSTALL_COMMAND ""
    )
endif ()

set(WTF_PLATFORM_ULTRALIGHT 1)

string(TOLOWER ${PORT} WEBKIT_PORT_DIR)

# -----------------------------------------------------------------------------
# Default library types
# -----------------------------------------------------------------------------
set(JavaScriptCore_LIBRARY_TYPE SHARED)
set(WebCore_LIBRARY_TYPE SHARED)
set(WebKit_LIBRARY_TYPE SHARED)
set(WebKit2_LIBRARY_TYPE SHARED)
set(WebCoreTestSupport_LIBRARY_TYPE STATIC)

# -----------------------------------------------------------------------------
# Install JavaScript shell
# -----------------------------------------------------------------------------
option(SHOULD_INSTALL_JS_SHELL "generate an installation rule to install the built JavaScript shell")

# -----------------------------------------------------------------------------
# Default output directories, which can be overwritten by ports
#------------------------------------------------------------------------------
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# -----------------------------------------------------------------------------
# Common configuration
#------------------------------------------------------------------------------
include(WebKitCommon)

# -----------------------------------------------------------------------------
# Add module directories
# -----------------------------------------------------------------------------
add_subdirectory(Source)

# -----------------------------------------------------------------------------
# Print the features list last, for maximum visibility.
# -----------------------------------------------------------------------------
PRINT_WEBKIT_OPTIONS()
