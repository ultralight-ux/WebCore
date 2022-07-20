set(JavaScriptCore_LIBRARY_TYPE STATIC)
add_definitions(-DSTATICALLY_LINKED_WITH_WTF)

list(APPEND JavaScriptCore_INCLUDE_DIRECTORIES
    #"${WTF_DIR}"
	${CMAKE_BINARY_DIR}/../include/private
)

list(APPEND JavaScriptCore_LIBRARIES
    WTF
)

if (ENABLE_REMOTE_INSPECTOR)
    list(APPEND JavaScriptCore_PRIVATE_INCLUDE_DIRECTORIES
        "${JAVASCRIPTCORE_DIR}/inspector/remote/socket"
    )

    list(APPEND JavaScriptCore_PRIVATE_FRAMEWORK_HEADERS
        inspector/remote/RemoteAutomationTarget.h
        inspector/remote/RemoteConnectionToTarget.h
        inspector/remote/RemoteControllableTarget.h
        inspector/remote/RemoteInspectionTarget.h
        inspector/remote/RemoteInspector.h

        inspector/remote/socket/RemoteInspectorConnectionClient.h
        inspector/remote/socket/RemoteInspectorMessageParser.h
        inspector/remote/socket/RemoteInspectorServer.h
        inspector/remote/socket/RemoteInspectorSocket.h
        inspector/remote/socket/RemoteInspectorSocketEndpoint.h
    )

    list(APPEND JavaScriptCore_SOURCES
        API/JSRemoteInspector.cpp

        inspector/remote/RemoteAutomationTarget.cpp
        inspector/remote/RemoteConnectionToTarget.cpp
        inspector/remote/RemoteControllableTarget.cpp
        inspector/remote/RemoteInspectionTarget.cpp
        inspector/remote/RemoteInspector.cpp

        inspector/remote/socket/RemoteInspectorConnectionClient.cpp
        inspector/remote/socket/RemoteInspectorMessageParser.cpp
        inspector/remote/socket/RemoteInspectorServer.cpp
        inspector/remote/socket/RemoteInspectorSocket.cpp
        inspector/remote/socket/RemoteInspectorSocketEndpoint.cpp

        inspector/remote/socket/win/RemoteInspectorSocketWin.cpp
    )

    list(APPEND JavaScriptCore_LIBRARIES
        wsock32 ws2_32
    )
else ()
    list(REMOVE_ITEM JavaScriptCore_SOURCES
        inspector/JSGlobalObjectInspectorController.cpp
    )
endif ()

file(COPY
    "${JAVASCRIPTCORE_DIR}/JavaScriptCore.vcxproj/JavaScriptCore.resources"
    DESTINATION
    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
)

file(MAKE_DIRECTORY ${DERIVED_SOURCES_DIR}/ForwardingHeaders/JavaScriptCore)

#set(JavaScriptCore_PRE_BUILD_COMMAND "${CMAKE_BINARY_DIR}/DerivedSources/JavaScriptCore/preBuild.cmd")
#file(REMOVE "${JavaScriptCore_PRE_BUILD_COMMAND}")
#foreach (_directory ${JavaScriptCore_FORWARDING_HEADERS_DIRECTORIES})
#    file(APPEND "${JavaScriptCore_PRE_BUILD_COMMAND}" "@xcopy /y /d /f \"${JAVASCRIPTCORE_DIR}/${_directory}/*.h\" \"${DERIVED_SOURCES_DIR}/ForwardingHeaders/JavaScriptCore\" >nul 2>nul\n")
#endforeach ()

set(JavaScriptCore_POST_BUILD_COMMAND "${CMAKE_BINARY_DIR}/DerivedSources/JavaScriptCore/postBuild.cmd")
file(WRITE "${JavaScriptCore_POST_BUILD_COMMAND}" "@xcopy /y /d /f \"${DERIVED_SOURCES_DIR}/JavaScriptCore/*.h\" \"${DERIVED_SOURCES_DIR}/ForwardingHeaders/JavaScriptCore\" >nul 2>nul\n")
file(APPEND "${JavaScriptCore_POST_BUILD_COMMAND}" "@xcopy /y /d /f \"${DERIVED_SOURCES_DIR}/JavaScriptCore/inspector/*.h\" \"${DERIVED_SOURCES_DIR}/ForwardingHeaders/JavaScriptCore\" >nul 2>nul\n")

set(JavaScriptCore_OUTPUT_NAME JavaScriptCore${DEBUG_SUFFIX})