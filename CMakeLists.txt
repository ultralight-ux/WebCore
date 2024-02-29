cmake_minimum_required(VERSION 3.15.0)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake)

include(common)

project(WebCore)

# This should be removed once we require CMake 3.3.
if (POLICY CMP0058)
    cmake_policy(SET CMP0058 NEW)
endif ()

if (POLICY CMP0038)
#    cmake_policy(SET CMP0038 OLD)
endif ()

if(NOT ${CMAKE_VERSION} VERSION_LESS "3.1")
    cmake_policy(SET CMP0054 NEW)
endif()

include(${CMAKE_ROOT}/Modules/ExternalProject.cmake)

set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/Source/cmake")

get_filename_component(WEBCORE_ROOT "./" REALPATH)
get_filename_component(WEBCORE_BINARY_DIR "${PROJECT_BINARY_DIR}" REALPATH)

set(WEBCORE_ROOT "${WEBCORE_ROOT}" CACHE INTERNAL "" FORCE)
set(WEBCORE_BINARY_DIR "${WEBCORE_BINARY_DIR}" CACHE INTERNAL "" FORCE)
    
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
# This should be set in platform toolchain.
if (NOT PORT)
    message(FATAL_ERROR "Unknown PORT '${PORT}'")
endif ()

set(WTF_PLATFORM_ULTRALIGHT 1)

string(TOLOWER ${PORT} WEBKIT_PORT_DIR)

include(Deps.cmake)

# -----------------------------------------------------------------------------
# Default library types
# -----------------------------------------------------------------------------

if (UL_ENABLE_STATIC_BUILD)
    set(JavaScriptCore_LIBRARY_TYPE STATIC)
    set(WebCore_LIBRARY_TYPE STATIC)
    set(WebKit_LIBRARY_TYPE STATIC)
    set(WebKit2_LIBRARY_TYPE STATIC)
    set(WebCoreTestSupport_LIBRARY_TYPE STATIC)
else ()
    set(JavaScriptCore_LIBRARY_TYPE SHARED)
    set(WebCore_LIBRARY_TYPE SHARED)
    set(WebKit_LIBRARY_TYPE SHARED)
    set(WebKit2_LIBRARY_TYPE SHARED)
    set(WebCoreTestSupport_LIBRARY_TYPE STATIC)
endif ()

# -----------------------------------------------------------------------------
# Install JavaScript shell
# -----------------------------------------------------------------------------
option(SHOULD_INSTALL_JS_SHELL "generate an installation rule to install the built JavaScript shell")

# -----------------------------------------------------------------------------
# Default output directories, which can be overwritten by ports
#------------------------------------------------------------------------------
if (NOT ALLINONE_BUILD)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)
endif ()

# -----------------------------------------------------------------------------
# Common configuration
#------------------------------------------------------------------------------
find_package (Python3)

include(WebKitCommon)

# -----------------------------------------------------------------------------
# Add module directories
# -----------------------------------------------------------------------------
add_subdirectory(Source)

# -----------------------------------------------------------------------------
# Print the features list last, for maximum visibility.
# -----------------------------------------------------------------------------
PRINT_WEBKIT_OPTIONS()
