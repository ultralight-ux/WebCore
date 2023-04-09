# -----------------------------------------------------------------------------
# Add module directories
# -----------------------------------------------------------------------------
# FIXME: Port bmalloc to Windows. https://bugs.webkit.org/show_bug.cgi?id=143310
if (NOT WIN32)
    add_subdirectory(bmalloc)
endif ()

add_subdirectory(mimalloc)
add_subdirectory(WTF)
add_subdirectory(JavaScriptCore)
add_subdirectory(SQLite)
add_subdirectory(WebInspectorUI)
add_subdirectory(WebCore)

WEBKIT_INCLUDE_CONFIG_FILES_IF_EXISTS()