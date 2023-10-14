set(JavaScriptCore_LIBRARY_TYPE STATIC)
add_definitions(-DSTATICALLY_LINKED_WITH_WTF -D__STDC_WANT_LIB_EXT1__)

#list(REMOVE_ITEM JavaScriptCore_SOURCES
#   inspector/JSGlobalObjectInspectorController.cpp
#)

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

        inspector/remote/socket/posix/RemoteInspectorSocketPOSIX.cpp
    )

    #list(APPEND JavaScriptCore_LIBRARIES
    #    wsock32 ws2_32
    #)
endif ()

file(MAKE_DIRECTORY ${DERIVED_SOURCES_DIR}/ForwardingHeaders/JavaScriptCore)

#set(BUILD_TMP "${CMAKE_BINARY_DIR}/tmp/")
#file(MAKE_DIRECTORY "${BUILD_TMP}")

#set(JavaScriptCore_PRE_BUILD_COMMAND "${CMAKE_BINARY_DIR}/DerivedSources/JavaScriptCore/preBuild.sh")
#set(JavaScriptCore_POST_BUILD_COMMAND "${CMAKE_BINARY_DIR}/DerivedSources/JavaScriptCore/postBuild.sh")

# Write the pre-build bash script
#file(WRITE "${BUILD_TMP}/preBuild.sh" "#!/bin/bash\ncp -R ${JAVASCRIPTCORE_DIR}/ForwardingHeaders/ ${DERIVED_SOURCES_DIR}/ForwardingHeaders/JavaScriptCore 2>/dev/null\n")
#foreach (_directory ${JavaScriptCore_FORWARDING_HEADERS_DIRECTORIES})
#file(APPEND "${BUILD_TMP}/preBuild.sh" "rsync -aqW ${JAVASCRIPTCORE_DIR}/${_directory}/*.h ${DERIVED_SOURCES_DIR}/ForwardingHeaders/JavaScriptCore 2>/dev/null\n")
#endforeach ()

# Write the post-build bash script
#file(WRITE "${BUILD_TMP}/postBuild.sh" "#!/bin/bash\nrsync -aqW ${DERIVED_SOURCES_DIR}/JavaScriptCore/*.h ${DERIVED_SOURCES_DIR}/ForwardingHeaders/JavaScriptCore 2>/dev/null\n")
#file(APPEND "${BUILD_TMP}/postBuild.sh" "rsync -aqW ${DERIVED_SOURCES_DIR}/JavaScriptCore/inspector/*.h ${DERIVED_SOURCES_DIR}/ForwardingHeaders/JavaScriptCore 2>/dev/null\n")

# Copy bash scripts over and chmod to executable
#file (COPY "${BUILD_TMP}/preBuild.sh" DESTINATION "${CMAKE_BINARY_DIR}/DerivedSources/JavaScriptCore" FILE_PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ)
#file (COPY "${BUILD_TMP}/postBuild.sh" DESTINATION "${CMAKE_BINARY_DIR}/DerivedSources/JavaScriptCore" FILE_PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ)

# Remove temp directory
#file(REMOVE_RECURSE "${BUILD_TMP}")