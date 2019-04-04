set(JavaScriptCore_LIBRARY_TYPE STATIC)
add_definitions(-DSTATICALLY_LINKED_WITH_WTF -D__STDC_WANT_LIB_EXT1__)

#list(REMOVE_ITEM JavaScriptCore_SOURCES
#   inspector/JSGlobalObjectInspectorController.cpp
#)

# FIXME: Make including these files consistent in the source so these forwarding headers are not needed.
if (NOT EXISTS ${DERIVED_SOURCES_JAVASCRIPTCORE_DIR}/InspectorBackendDispatchers.h)
file(WRITE ${DERIVED_SOURCES_JAVASCRIPTCORE_DIR}/InspectorBackendDispatchers.h "#include \"inspector/InspectorBackendDispatchers.h\"")
endif ()
if (NOT EXISTS ${DERIVED_SOURCES_JAVASCRIPTCORE_DIR}/InspectorFrontendDispatchers.h)
file(WRITE ${DERIVED_SOURCES_JAVASCRIPTCORE_DIR}/InspectorFrontendDispatchers.h "#include \"inspector/InspectorFrontendDispatchers.h\"")
endif ()
if (NOT EXISTS ${DERIVED_SOURCES_JAVASCRIPTCORE_DIR}/InspectorProtocolObjects.h)
file(WRITE ${DERIVED_SOURCES_JAVASCRIPTCORE_DIR}/InspectorProtocolObjects.h "#include \"inspector/InspectorProtocolObjects.h\"")
endif ()
