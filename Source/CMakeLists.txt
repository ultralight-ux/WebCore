# -----------------------------------------------------------------------------
# Add module directories
# -----------------------------------------------------------------------------
# FIXME: Port bmalloc to Windows. https://bugs.webkit.org/show_bug.cgi?id=143310
if (NOT WIN32)
#    add_subdirectory(bmalloc)
endif ()

add_subdirectory(WTF)
add_subdirectory(JavaScriptCore)
add_subdirectory(SQLite)
add_subdirectory(WebCore)

WEBKIT_INCLUDE_CONFIG_FILES_IF_EXISTS()

# -----------------------------------------------------------------------------
# Set compiler flags for all targets
# -----------------------------------------------------------------------------
WEBKIT_SET_EXTRA_COMPILER_FLAGS(bmalloc ${ADDITIONAL_COMPILER_FLAGS})
WEBKIT_SET_EXTRA_COMPILER_FLAGS(WTF ${ADDITIONAL_COMPILER_FLAGS})
WEBKIT_SET_EXTRA_COMPILER_FLAGS(JavaScriptCore ${ADDITIONAL_COMPILER_FLAGS})
WEBKIT_SET_EXTRA_COMPILER_FLAGS(WebCore ${ADDITIONAL_COMPILER_FLAGS})
