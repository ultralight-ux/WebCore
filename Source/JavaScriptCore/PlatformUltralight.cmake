set(JavaScriptCore_LIBRARY_TYPE STATIC)
add_definitions(-DSTATICALLY_LINKED_WITH_WTF)

list(APPEND JavaScriptCore_INCLUDE_DIRECTORIES
	${PROJECT_BINARY_DIR}/../include/private
    ${WEBKIT_LIBRARIES_DIR}/include
)

list(APPEND JavaScriptCore_LIBRARIES
    WTF
)

if (ENABLE_REMOTE_INSPECTOR)
    list(APPEND JavaScriptCore_PRIVATE_INCLUDE_DIRECTORIES
        "${JAVASCRIPTCORE_DIR}/inspector/remote/socket"
    )

    list(APPEND JavaScriptCore_PRIVATE_FRAMEWORK_HEADERS
        inspector/remote/RemoteConnectionToTarget.h
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
    )
else ()
    list(REMOVE_ITEM JavaScriptCore_SOURCES
        inspector/JSGlobalObjectInspectorController.cpp
    )
endif ()
