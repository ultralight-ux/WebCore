include(${CMAKE_CURRENT_SOURCE_DIR}/PlatformUltralight.cmake)

if (ENABLE_REMOTE_INSPECTOR)
    list(APPEND JavaScriptCore_SOURCES
        inspector/remote/socket/win/RemoteInspectorSocketWin.cpp
    )

    list(APPEND JavaScriptCore_LIBRARIES
        wsock32 ws2_32
    )
endif ()

file(COPY
    "${JAVASCRIPTCORE_DIR}/JavaScriptCore.vcxproj/JavaScriptCore.resources"
    DESTINATION
    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
)