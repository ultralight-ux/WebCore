set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")

include_directories("${CMAKE_BINARY_DIR}/DerivedSources/ForwardingHeaders"
                    "${CMAKE_BINARY_DIR}/DerivedSources"
                    "${WEBKIT_LIBRARIES_DIR}/include")

link_directories("${WEBKIT_LIBRARIES_DIR}/lib")
link_directories("${ULTRALIGHTCORE_DIR}/lib")
link_directories("${ULTRALIGHTCORE_DIR}/bin")

set(JavaScriptCore_LIBRARY_TYPE SHARED)
set(WTF_LIBRARY_TYPE SHARED)

set(ENABLE_CHANNEL_MESSAGING ON)
set(ENABLE_DRAG_SUPPORT ON)
set(ENABLE_GRAPHICS_CONTEXT_3D 0)
set(ENABLE_REMOTE_INSPECTOR ON)
set(ENABLE_WEB_SOCKETS ON)
set(ENABLE_XSLT ON)
set(USE_CF 0)
set(USE_CURL 1)
set(USE_HARFBUZZ 1)
set(USE_ICU_UNICODE 1)
set(USE_TEXTURE_MAPPER_GL 0)
set(USE_TEXTURE_MAPPER_ULTRALIGHT 1)
set(USE_ULTRALIGHT 1)
add_definitions(-DWTF_PLATFORM_ULTRALIGHT=1)

if (UL_ENABLE_VIDEO)
    set(USE_GSTREAMER 1)
    SET_AND_EXPOSE_TO_BUILD(USE_GSTREAMER TRUE)

    link_directories("${GSTREAMER_DIR}/lib")
    link_directories("${GSTREAMER_DIR}/bin")
    set(GSTREAMER_INCLUDE_DIRS "${GSTREAMER_DIR}/include"
                               "${GSTREAMER_DIR}/include/gstreamer-1.0")

    if (ENABLE_WEB_AUDIO)
        SET_AND_EXPOSE_TO_BUILD(USE_WEBAUDIO_GSTREAMER TRUE)
    endif ()
    if (ENABLE_MEDIA_STREAM OR ENABLE_WEB_RTC)
        SET_AND_EXPOSE_TO_BUILD(USE_LIBWEBRTC TRUE)
        SET_AND_EXPOSE_TO_BUILD(WEBRTC_WEBKIT_BUILD TRUE)
    else ()
        SET_AND_EXPOSE_TO_BUILD(USE_LIBWEBRTC FALSE)
        SET_AND_EXPOSE_TO_BUILD(WEBRTC_WEBKIT_BUILD FALSE)
    endif ()
else ()
    set(USE_GSTREAMER 0)
    SET_AND_EXPOSE_TO_BUILD(USE_GSTREAMER FALSE)
endif ()

add_definitions(-DULTRALIGHT_MODULE_WEBCORE)

add_definitions(-DULTRALIGHT_MARKED_BLOCK_SIZE=${UL_MARKED_BLOCK_SIZE})

if (UL_PROFILE_PERFORMANCE OR UL_PROFILE_MEMORY)
    add_definitions(-DTRACY_ENABLE)
    add_definitions(-DTRACY_IMPORTS)
    add_definitions(-DTRACY_CALLSTACK_DEPTH=${UL_CALLSTACK_DEPTH})
endif ()

if (UL_PROFILE_PERFORMANCE)
    add_definitions(-DTRACY_PROFILE_PERFORMANCE)
endif ()

if (UL_PROFILE_MEMORY)
    add_definitions(-DTRACY_PROFILE_MEMORY)
endif ()

if (UL_ENABLE_MEMORY_STATS)
    add_definitions(-DULTRALIGHT_ENABLE_MEMORY_STATS)
endif ()

if (UL_PROFILE_MEMORY OR UL_ENABLE_MEMORY_STATS)
    if (NOT PORT MATCHES "UltralightWin")
        message(FATAL_ERROR "Memory profiling is only available for Windows at this time. Please disable memory profiling and memory stats before building again.")
    endif ()

    add_definitions(-DULTRALIGHT_ENABLE_MEMORY_PROFILER)
    
    if (PORT MATCHES "UltralightWin")
        # Disable exceptions for allocator shim
        add_compile_options(/EHa- /EHc- /EHs- /fp:except-)
    endif ()
endif ()

if (UL_ENABLE_ALLOCATOR_OVERRIDE)
    if (UL_PROFILE_MEMORY OR UL_ENABLE_MEMORY_STATS)
        message(FATAL_ERROR "Allocator override cannot be used when memory profiling or memory stats is enabled.")
    endif ()

    add_definitions(-DULTRALIGHT_ENABLE_ALLOCATOR_OVERRIDE)
endif ()