if (NOT BMALLOC_DIR)
    set(BMALLOC_DIR "${PROJECT_SOURCE_DIR}/Source/bmalloc")
endif ()
if (NOT MIMALLOC_DIR)
    set(MIMALLOC_DIR "${PROJECT_SOURCE_DIR}/Source/mimalloc")
endif ()
if (NOT WTF_DIR)
    set(WTF_DIR "${PROJECT_SOURCE_DIR}/Source/WTF")
endif ()
if (NOT JAVASCRIPTCORE_DIR)
    set(JAVASCRIPTCORE_DIR "${PROJECT_SOURCE_DIR}/Source/JavaScriptCore")
endif ()
if (NOT WEBCORE_DIR)
    set(WEBCORE_DIR "${PROJECT_SOURCE_DIR}/Source/WebCore")
endif ()
if (NOT PAL_DIR)
    set(PAL_DIR "${PROJECT_SOURCE_DIR}/Source/WebCore/PAL")
endif ()
if (NOT WEBKITLEGACY_DIR)
    set(WEBKITLEGACY_DIR "${PROJECT_SOURCE_DIR}/Source/WebKitLegacy")
endif ()
if (NOT WEBKIT_DIR)
    set(WEBKIT_DIR "${PROJECT_SOURCE_DIR}/Source/WebKit")
endif ()
if (NOT THIRDPARTY_DIR)
    set(THIRDPARTY_DIR "${PROJECT_SOURCE_DIR}/Source/ThirdParty")
endif ()
if (NOT TOOLS_DIR)
    set(TOOLS_DIR "${PROJECT_SOURCE_DIR}/Tools")
endif ()
if (NOT SQLITE_DIR)
    set(SQLITE_DIR "${PROJECT_SOURCE_DIR}/Source/SQLite")
endif ()
if (NOT ULTRALIGHTCORE_DIR)
    set(ULTRALIGHTCORE_DIR "${PROJECT_SOURCE_DIR}/deps/UltralightCore")
endif ()

if (NOT WEBDRIVER_DIR)
    set(WEBDRIVER_DIR "${PROJECT_SOURCE_DIR}/Source/WebDriver")
endif ()
if (NOT WEBINSPECTORUI_DIR)
    set(WEBINSPECTORUI_DIR "${PROJECT_SOURCE_DIR}/Source/WebInspectorUI")
endif ()

set(WTF_DERIVED_SOURCES_DIR "${PROJECT_BINARY_DIR}/WTF/DerivedSources")
set(JavaScriptCore_DERIVED_SOURCES_DIR "${PROJECT_BINARY_DIR}/JavaScriptCore/DerivedSources")
set(PAL_DERIVED_SOURCES_DIR "${PROJECT_BINARY_DIR}/PAL/DerivedSources")
set(WebCore_DERIVED_SOURCES_DIR "${PROJECT_BINARY_DIR}/WebCore/DerivedSources")
set(WebDriver_DERIVED_SOURCES_DIR "${PROJECT_BINARY_DIR}/WebDriver/DerivedSources")
set(WebKitLegacy_DERIVED_SOURCES_DIR "${PROJECT_BINARY_DIR}/WebKitLegacy/DerivedSources")
set(WebKit_DERIVED_SOURCES_DIR "${PROJECT_BINARY_DIR}/WebKit/DerivedSources")
set(WebInspectorUI_DERIVED_SOURCES_DIR "${PROJECT_BINARY_DIR}/WebInspectorUI/DerivedSources")
set(MiniBrowser_DERIVED_SOURCES_DIR "${PROJECT_BINARY_DIR}/MiniBrowser/DerivedSources")
set(TestRunnerShared_DERIVED_SOURCES_DIR "${PROJECT_BINARY_DIR}/TestRunnerShared/DerivedSources")
set(DumpRenderTree_DERIVED_SOURCES_DIR "${PROJECT_BINARY_DIR}/DumpRenderTree/DerivedSources")
set(WebKitTestRunner_DERIVED_SOURCES_DIR "${PROJECT_BINARY_DIR}/WebKitTestRunner/DerivedSources")

set(bmalloc_FRAMEWORK_HEADERS_DIR "${PROJECT_BINARY_DIR}/bmalloc/Headers")
set(ANGLE_FRAMEWORK_HEADERS_DIR "${PROJECT_BINARY_DIR}/ANGLE/Headers")
set(WTF_FRAMEWORK_HEADERS_DIR "${PROJECT_BINARY_DIR}/WTF/Headers")
set(JavaScriptCore_FRAMEWORK_HEADERS_DIR "${PROJECT_BINARY_DIR}/JavaScriptCore/Headers")
set(JavaScriptCore_PRIVATE_FRAMEWORK_HEADERS_DIR "${PROJECT_BINARY_DIR}/JavaScriptCore/PrivateHeaders")
set(PAL_FRAMEWORK_HEADERS_DIR "${PROJECT_BINARY_DIR}/PAL/Headers")
set(WebCore_PRIVATE_FRAMEWORK_HEADERS_DIR "${PROJECT_BINARY_DIR}/WebCore/PrivateHeaders")
set(WebKitLegacy_FRAMEWORK_HEADERS_DIR "${PROJECT_BINARY_DIR}/WebKitLegacy/Headers")
set(WebKit_FRAMEWORK_HEADERS_DIR "${PROJECT_BINARY_DIR}/WebKit/Headers")
set(WebKit_PRIVATE_FRAMEWORK_HEADERS_DIR "${PROJECT_BINARY_DIR}/WebKit/PrivateHeaders")

# Using DERIVED_SOURCES_DIR is deprecated
set(DERIVED_SOURCES_DIR "${PROJECT_BINARY_DIR}/DerivedSources")
set(DERIVED_SOURCES_WEBKIT_DIR ${DERIVED_SOURCES_DIR}/WebKit)
set(DERIVED_SOURCES_WEBINSPECTORUI_DIR ${DERIVED_SOURCES_DIR}/WebInspectorUI)

# Using FORWARDING_HEADERS_DIR is deprecated
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

#set(WTF_SCRIPTS_DIR "${PROJECT_BINARY_DIR}/WTF/Scripts")
#set(JavaScriptCore_SCRIPTS_DIR "${PROJECT_BINARY_DIR}/JavaScriptCore/Scripts")

set(WTF_SCRIPTS_DIR "${FORWARDING_HEADERS_DIR}/wtf/Scripts")
set(JavaScriptCore_SCRIPTS_DIR "${FORWARDING_HEADERS_DIR}/JavaScriptCore/Scripts")

file(MAKE_DIRECTORY ${DERIVED_SOURCES_WEBINSPECTORUI_DIR})
file(MAKE_DIRECTORY ${DERIVED_SOURCES_WEBINSPECTORUI_DIR}/Protocol)
file(MAKE_DIRECTORY ${DERIVED_SOURCES_WEBINSPECTORUI_DIR}/UserInterface/Protocol)

