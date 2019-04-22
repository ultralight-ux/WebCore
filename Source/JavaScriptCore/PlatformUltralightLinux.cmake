set(JavaScriptCore_LIBRARY_TYPE STATIC)
add_definitions(-DSTATICALLY_LINKED_WITH_WTF -D__STDC_WANT_LIB_EXT1__)

#list(REMOVE_ITEM JavaScriptCore_SOURCES
#   inspector/JSGlobalObjectInspectorController.cpp
#)

file(MAKE_DIRECTORY ${DERIVED_SOURCES_DIR}/ForwardingHeaders/JavaScriptCore)

set(JavaScriptCore_PRE_BUILD_COMMAND "${CMAKE_BINARY_DIR}/DerivedSources/JavaScriptCore/preBuild.sh")
file(REMOVE "${JavaScriptCore_PRE_BUILD_COMMAND}")

file(WRITE "${JavaScriptCore_PRE_BUILD_COMMAND}" "#!/bin/bash\ncp -R ${JAVASCRIPTCORE_DIR}/ForwardingHeaders/ ${DERIVED_SOURCES_DIR}/ForwardingHeaders/JavaScriptCore 2>/dev/null\n")
foreach (_directory ${JavaScriptCore_FORWARDING_HEADERS_DIRECTORIES})
file(APPEND "${JavaScriptCore_PRE_BUILD_COMMAND}" "rsync -aqW ${JAVASCRIPTCORE_DIR}/${_directory}/*.h ${DERIVED_SOURCES_DIR}/ForwardingHeaders/JavaScriptCore 2>/dev/null\n")
endforeach ()

set(JavaScriptCore_POST_BUILD_COMMAND "${CMAKE_BINARY_DIR}/DerivedSources/JavaScriptCore/postBuild.sh")

file(WRITE "${JavaScriptCore_POST_BUILD_COMMAND}" "#!/bin/bash\nrsync -aqW ${DERIVED_SOURCES_DIR}/JavaScriptCore/*.h ${DERIVED_SOURCES_DIR}/ForwardingHeaders/JavaScriptCore 2>/dev/null\n")
file(APPEND "${JavaScriptCore_POST_BUILD_COMMAND}" "rsync -aqW ${DERIVED_SOURCES_DIR}/JavaScriptCore/inspector/*.h ${DERIVED_SOURCES_DIR}/ForwardingHeaders/JavaScriptCore 2>/dev/null\n")
