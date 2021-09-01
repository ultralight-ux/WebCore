if (NOT BMALLOC_DIR)
    set(BMALLOC_DIR "${CMAKE_SOURCE_DIR}/Source/bmalloc")
endif ()
if (NOT MIMALLOC_DIR)
    set(MIMALLOC_DIR "${CMAKE_SOURCE_DIR}/Source/mimalloc")
endif ()
if (NOT WTF_DIR)
    set(WTF_DIR "${CMAKE_SOURCE_DIR}/Source/WTF")
endif ()
if (NOT JAVASCRIPTCORE_DIR)
    set(JAVASCRIPTCORE_DIR "${CMAKE_SOURCE_DIR}/Source/JavaScriptCore")
endif ()
if (NOT WEBCORE_DIR)
    set(WEBCORE_DIR "${CMAKE_SOURCE_DIR}/Source/WebCore")
endif ()
if (NOT PAL_DIR)
    set(PAL_DIR "${CMAKE_SOURCE_DIR}/Source/WebCore/PAL")
endif ()
if (NOT WEBKITLEGACY_DIR)
    set(WEBKITLEGACY_DIR "${CMAKE_SOURCE_DIR}/Source/WebKitLegacy")
endif ()
if (NOT WEBKIT_DIR)
    set(WEBKIT_DIR "${CMAKE_SOURCE_DIR}/Source/WebKit")
endif ()
if (NOT THIRDPARTY_DIR)
    set(THIRDPARTY_DIR "${CMAKE_SOURCE_DIR}/Source/ThirdParty")
endif ()
if (NOT TOOLS_DIR)
    set(TOOLS_DIR "${CMAKE_SOURCE_DIR}/Tools")
endif ()
if (NOT SQLITE_DIR)
    set(SQLITE_DIR "${CMAKE_SOURCE_DIR}/Source/SQLite")
endif ()
if (NOT ULTRALIGHTCORE_DIR)
    set(ULTRALIGHTCORE_DIR "${CMAKE_SOURCE_DIR}/deps/UltralightCore")
endif ()

if (NOT WEBDRIVER_DIR)
    set(WEBDRIVER_DIR "${CMAKE_SOURCE_DIR}/Source/WebDriver")
endif ()

set(DERIVED_SOURCES_DIR "${CMAKE_BINARY_DIR}/DerivedSources")
set(DERIVED_SOURCES_JAVASCRIPTCORE_DIR "${CMAKE_BINARY_DIR}/DerivedSources/JavaScriptCore")
set(DERIVED_SOURCES_PAL_DIR "${CMAKE_BINARY_DIR}/DerivedSources/PAL")
set(DERIVED_SOURCES_WEBCORE_DIR "${CMAKE_BINARY_DIR}/DerivedSources/WebCore")
set(DERIVED_SOURCES_WEBDRIVER_DIR "${CMAKE_BINARY_DIR}/DerivedSources/WebDriver")
set(DERIVED_SOURCES_WEBKITLEGACY_DIR "${CMAKE_BINARY_DIR}/DerivedSources/WebKitLegacy")
set(DERIVED_SOURCES_WEBKIT_DIR "${CMAKE_BINARY_DIR}/DerivedSources/WebKit")
set(DERIVED_SOURCES_WEBINSPECTORUI_DIR "${CMAKE_BINARY_DIR}/DerivedSources/WebInspectorUI")

# These values will replace the above after refactoring is complete
set(WTF_DERIVED_SOURCES_DIR "${CMAKE_BINARY_DIR}/DerivedSources/WTF")
set(JavaScriptCore_DERIVED_SOURCES_DIR "${CMAKE_BINARY_DIR}/DerivedSources/JavaScriptCore")
set(PAL_DERIVED_SOURCES_DIR "${CMAKE_BINARY_DIR}/DerivedSources/PAL")
set(WebCore_DERIVED_SOURCES_DIR "${CMAKE_BINARY_DIR}/DerivedSources/WebCore")
set(WebDriver_DERIVED_SOURCES_DIR "${CMAKE_BINARY_DIR}/DerivedSources/WebDriver")
set(WebKitLegacy_DERIVED_SOURCES_DIR "${CMAKE_BINARY_DIR}/DerivedSources/WebKitLegacy")
set(WebKit_DERIVED_SOURCES_DIR "${CMAKE_BINARY_DIR}/DerivedSources/WebKit")
set(WebInspectorUI_DERIVED_SOURCES_DIR "${CMAKE_BINARY_DIR}/DerivedSources/WebInspectorUI")

set(FORWARDING_HEADERS_DIR ${DERIVED_SOURCES_DIR}/ForwardingHeaders)

set(bmalloc_FRAMEWORK_HEADERS_DIR ${FORWARDING_HEADERS_DIR})
set(mimalloc_FRAMEWORK_HEADERS_DIR ${FORWARDING_HEADERS_DIR})
set(ANGLE_FRAMEWORK_HEADERS_DIR ${FORWARDING_HEADERS_DIR})
set(WTF_FRAMEWORK_HEADERS_DIR ${FORWARDING_HEADERS_DIR})
set(JavaScriptCore_FRAMEWORK_HEADERS_DIR ${FORWARDING_HEADERS_DIR})
set(JavaScriptCore_PRIVATE_FRAMEWORK_HEADERS_DIR ${FORWARDING_HEADERS_DIR})
set(PAL_FRAMEWORK_HEADERS_DIR ${FORWARDING_HEADERS_DIR})
set(WebCore_PRIVATE_FRAMEWORK_HEADERS_DIR ${FORWARDING_HEADERS_DIR})
set(WebKitLegacy_FRAMEWORK_HEADERS_DIR ${FORWARDING_HEADERS_DIR})
set(WebKit_FRAMEWORK_HEADERS_DIR ${FORWARDING_HEADERS_DIR})
set(WebKit_PRIVATE_FRAMEWORK_HEADERS_DIR ${FORWARDING_HEADERS_DIR})

set(WTF_SCRIPTS_DIR "${FORWARDING_HEADERS_DIR}/wtf/Scripts")
set(JavaScriptCore_SCRIPTS_DIR "${FORWARDING_HEADERS_DIR}/JavaScriptCore/Scripts")

file(MAKE_DIRECTORY ${DERIVED_SOURCES_WEBINSPECTORUI_DIR})
file(MAKE_DIRECTORY ${DERIVED_SOURCES_WEBINSPECTORUI_DIR}/Protocol)
file(MAKE_DIRECTORY ${DERIVED_SOURCES_WEBINSPECTORUI_DIR}/UserInterface/Protocol)
