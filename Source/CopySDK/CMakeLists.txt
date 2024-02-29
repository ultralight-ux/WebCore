set(SCRIPT_DIR ${WEBCORE_ROOT}/scripts)
set(SCRIPT_PATH ${SCRIPT_DIR}/copy_webcore_sdk.py)

add_custom_command(
    OUTPUT ${WEBCORE_BINARY_DIR}/DerivedSources/ForwardingHeaders/WebCore/config.h
    COMMAND ${CMAKE_COMMAND} -E env "PYTHONUNBUFFERED=1"
    ${Python3_EXECUTABLE} ${SCRIPT_PATH}
        --destination-dir ${WEBCORE_BINARY_DIR}
        --derived-sources-dir ${DERIVED_SOURCES_DIR}
        --project-source-dir ${PROJECT_SOURCE_DIR}
        --javascriptcore-derived-sources-dir ${JavaScriptCore_DERIVED_SOURCES_DIR}
    COMMENT "Running Python script to copy WebCore SDK files"
    DEPENDS WebCore JavaScriptCore ${SCRIPT_PATH}
    WORKING_DIRECTORY ${SCRIPT_DIR}
)

# Add a custom target that depends on the custom command
add_custom_target(
    CopyWebCoreSDK
    DEPENDS ${WEBCORE_BINARY_DIR}/DerivedSources/ForwardingHeaders/WebCore/config.h
)