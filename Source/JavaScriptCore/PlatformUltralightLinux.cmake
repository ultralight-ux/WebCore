include(${CMAKE_CURRENT_SOURCE_DIR}/PlatformUltralight.cmake)

add_definitions(-D__STDC_WANT_LIB_EXT1__)

if (ENABLE_REMOTE_INSPECTOR)
    list(APPEND JavaScriptCore_SOURCES
        inspector/remote/socket/posix/RemoteInspectorSocketPOSIX.cpp
    )
endif ()