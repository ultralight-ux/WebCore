set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

option(MI_SECURE            "Use full security mitigations (like guard pages, allocation randomization, double-free mitigation, and free-list corruption detection)" OFF)
option(MI_DEBUG_FULL        "Use full internal heap invariant checking in DEBUG mode (expensive)" OFF)
option(MI_PADDING           "Enable padding to detect heap block overflow (used only in DEBUG mode)" OFF)
option(MI_XMALLOC           "Enable abort() call on memory allocation failure by default" OFF)
option(MI_SHOW_ERRORS       "Show error and warning messages by default (only enabled by default in DEBUG mode)" OFF)
option(MI_USE_CXX           "Use the C++ compiler to compile the library (instead of the C compiler)" OFF)
option(MI_SEE_ASM           "Generate assembly files" OFF)
option(MI_INTERPOSE         "Use interpose to override standard malloc on macOS" OFF)
option(MI_OSX_ZONE          "Use malloc zone to override standard malloc on macOS" OFF) 
option(MI_LOCAL_DYNAMIC_TLS "Use slightly slower, dlopen-compatible TLS mechanism (Unix)" OFF)
option(MI_DEBUG_TSAN        "Build with thread sanitizer (needs clang)" OFF)
option(MI_DEBUG_UBSAN       "Build with undefined-behavior sanitizer (needs clang++)" OFF)
option(MI_CHECK_FULL        "Use full internal invariant checking in DEBUG mode (deprecated, use MI_DEBUG_FULL instead)" OFF)

set_property(DIRECTORY . PROPERTY FOLDER "mimalloc")

set(mimalloc_SOURCES
    src/stats.c
    src/random.c
    src/os.c
    src/bitmap.c
    src/arena.c
    src/region.c
    src/segment.c
    src/page.c
    src/alloc.c
    src/alloc-aligned.c
    src/alloc-posix.c
    src/heap.c
    src/options.c
    src/init.c
)

set(mimalloc_PUBLIC_HEADERS
    include/mimalloc.h
    include/mimalloc-override.h
    include/mimalloc-new-delete.h
)

# -----------------------------------------------------------------------------
# Process options
# -----------------------------------------------------------------------------

if(CMAKE_C_COMPILER_ID MATCHES "MSVC|Intel")
  set(MI_USE_CXX "ON")
endif()

if(MI_SECURE)
  message(STATUS "Set full secure build (MI_SECURE=ON)")
  list(APPEND mi_defines MI_SECURE=4)
endif()

if(MI_SEE_ASM)
  message(STATUS "Generate assembly listings (MI_SEE_ASM=ON)")
  list(APPEND mi_cflags -save-temps)
endif()

if(MI_CHECK_FULL)
  message(STATUS "The MI_CHECK_FULL option is deprecated, use MI_DEBUG_FULL instead")
  set(MI_DEBUG_FULL "ON")
endif()

if(MI_DEBUG_FULL)
  message(STATUS "Set debug level to full internal invariant checking (MI_DEBUG_FULL=ON)")
  list(APPEND mi_defines MI_DEBUG=3)   # full invariant checking
endif()

if(NOT MI_PADDING)
  message(STATUS "Disable padding of heap blocks in debug mode (MI_PADDING=OFF)")
  list(APPEND mi_defines MI_PADDING=0)
endif()

if(MI_XMALLOC)
  message(STATUS "Enable abort() calls on memory allocation failure (MI_XMALLOC=ON)")
  list(APPEND mi_defines MI_XMALLOC=1)
endif()

if(MI_SHOW_ERRORS)
  message(STATUS "Enable printing of error and warning messages by default (MI_SHOW_ERRORS=ON)")
  list(APPEND mi_defines MI_SHOW_ERRORS=1)
endif()

if(MI_DEBUG_TSAN)
  if(CMAKE_C_COMPILER_ID MATCHES "Clang")
    message(STATUS "Build with thread sanitizer (MI_DEBUG_TSAN=ON)")
    list(APPEND mi_defines MI_TSAN=1)
    list(APPEND mi_cflags -fsanitize=thread -g -O1)
    list(APPEND CMAKE_EXE_LINKER_FLAGS -fsanitize=thread)
  else()
    message(WARNING "Can only use thread sanitizer with clang (MI_DEBUG_TSAN=ON but ignored)")    
  endif()  
endif()

if(MI_DEBUG_UBSAN)
  if(CMAKE_BUILD_TYPE MATCHES "Debug")    
    if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
      message(STATUS "Build with undefined-behavior sanitizer (MI_DEBUG_UBSAN=ON)")
      list(APPEND mi_cflags -fsanitize=undefined -g)
      list(APPEND CMAKE_EXE_LINKER_FLAGS -fsanitize=undefined)
      if (NOT MI_USE_CXX)
        message(STATUS "(switch to use C++ due to MI_DEBUG_UBSAN)")
        set(MI_USE_CXX "ON")
      endif()
    else()
      message(WARNING "Can only use undefined-behavior sanitizer with clang++ (MI_DEBUG_UBSAN=ON but ignored)")    
    endif()  
  else()
    message(WARNING "Can only use thread sanitizer with a debug build (CMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE})")    
  endif()
endif()

if(MI_USE_CXX)
  message(STATUS "Use the C++ compiler to compile (MI_USE_CXX=ON)")
  set_source_files_properties(${mi_sources} PROPERTIES LANGUAGE CXX )
  set_source_files_properties(src/static.c test/test-api.c test/test-stress PROPERTIES LANGUAGE CXX )
  if(CMAKE_CXX_COMPILER_ID MATCHES "AppleClang|Clang")
    list(APPEND mi_cflags -Wno-deprecated)
  endif()
  if(CMAKE_CXX_COMPILER_ID MATCHES "Intel")
    list(APPEND mi_cflags -Kc++)
  endif()
endif()

# Compiler flags
if(CMAKE_C_COMPILER_ID MATCHES "AppleClang|Clang|GNU")
  list(APPEND mi_cflags -Wall -Wextra -Wno-unknown-pragmas -fvisibility=hidden)
  if(CMAKE_C_COMPILER_ID MATCHES "GNU")
    list(APPEND mi_cflags -Wno-invalid-memory-model)
  endif()
  if(CMAKE_C_COMPILER_ID MATCHES "AppleClang|Clang")
    list(APPEND mi_cflags -Wpedantic -Wno-static-in-inline)
  endif()
endif()

if(CMAKE_C_COMPILER_ID MATCHES "Intel")
  list(APPEND mi_cflags -Wall -fvisibility=hidden)
endif()

if(CMAKE_C_COMPILER_ID MATCHES "AppleClang|Clang|GNU|Intel" AND NOT CMAKE_SYSTEM_NAME MATCHES "Haiku")
  if(MI_LOCAL_DYNAMIC_TLS)
    list(APPEND mi_cflags -ftls-model=local-dynamic)
  else()
    list(APPEND mi_cflags -ftls-model=initial-exec)
  endif()
endif()

if (MSVC AND MSVC_VERSION GREATER_EQUAL 1914)
  list(APPEND mi_cflags /Zc:__cplusplus)
endif()

if (${CMAKE_BUILD_TYPE} MATCHES Release OR ${CMAKE_BUILD_TYPE} MATCHES MinSizeRel)
  if (MSVC)
    list(APPEND mi_cflags /GL)
  endif()
endif()

# extra needed libraries
if(WIN32)
  list(APPEND mi_libraries psapi shell32 user32 advapi32 bcrypt)
else()
  if(NOT ${CMAKE_C_COMPILER} MATCHES "android")
    list(APPEND mi_libraries pthread)
    find_library(LIBRT rt)
    if(LIBRT)
      list(APPEND mi_libraries ${LIBRT})
    endif()
  endif()
endif()

# -----------------------------------------------------------------------------
# Install and output names
# -----------------------------------------------------------------------------

message(STATUS "")
message(STATUS "Build type       : ${CMAKE_BUILD_TYPE_LC}")
if(MI_USE_CXX)
  message(STATUS "Compiler         : ${CMAKE_CXX_COMPILER}")
else()
  message(STATUS "Compiler         : ${CMAKE_C_COMPILER}")
endif()
message(STATUS "")

set(mimalloc_LIBRARIES ${mi_libraries})
set(mimalloc_INTERFACE_LIBRARIES mimalloc)
set(mimalloc_INTERFACE_INCLUDE_DIRECTORIES ${mimalloc_FRAMEWORK_HEADERS_DIR})
set(mimalloc_INTERFACE_DEPENDENCIES mimalloc_CopyHeaders)
set(mimalloc_PRIVATE_DEFINITIONS ${mi_defines} MI_STATIC_LIB)
set(mimalloc_COMPILE_OPTIONS ${mi_cflags})
set(mimalloc_INCLUDE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR}/include)

WEBKIT_FRAMEWORK_DECLARE(mimalloc)
WEBKIT_INCLUDE_CONFIG_FILES_IF_EXISTS()

WEBKIT_COPY_FILES(mimalloc_CopyHeaders
    DESTINATION ${mimalloc_FRAMEWORK_HEADERS_DIR}
    FILES ${mimalloc_PUBLIC_HEADERS}
)

WEBKIT_FRAMEWORK(mimalloc)
