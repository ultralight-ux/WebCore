if (CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(ARCHITECTURE "x64")
else ()
    set(ARCHITECTURE "x86")
endif ()

if (PORT MATCHES "UltralightLinux")
    set(PLATFORM "linux")
elseif (PORT MATCHES "UltralightMac")
    set(PLATFORM "mac")
elseif (PORT MATCHES "UltralightWin")
    set(PLATFORM "win")
endif ()

set(WEBCORE_DEPS_REV "1.0")
set(ULTRALIGHTCORE_REV "84bd860")

set(WEBCORE_DEPS_DIR "${CMAKE_SOURCE_DIR}/deps/WebKitLibraries/")
set(ULTRALIGHTCORE_DIR "${CMAKE_SOURCE_DIR}/deps/UltralightCore/")

ExternalProject_Add(WebCoreDeps
  URL https://github.com/ultralight-ux/WebCore-deps/releases/download/${WEBCORE_DEPS_REV}/webcore-deps-bin-${WEBCORE_DEPS_REV}-${PLATFORM}-${ARCHITECTURE}.7z
  SOURCE_DIR "${WEBCORE_DEPS_DIR}"
  BUILD_IN_SOURCE 1
  CONFIGURE_COMMAND ""
  BUILD_COMMAND ""
  INSTALL_COMMAND ""
)

ExternalProject_Add(UltralightCoreBin
  URL https://github.com/ultralight-ux/UltralightCore-bin/releases/download/${ULTRALIGHTCORE_REV}/ultralightcore-bin-${ULTRALIGHTCORE_REV}-${PLATFORM}-${ARCHITECTURE}.7z
  SOURCE_DIR "${ULTRALIGHT_CORE_DIR}"
  BUILD_IN_SOURCE 1
  CONFIGURE_COMMAND ""
  BUILD_COMMAND ""
  INSTALL_COMMAND ""
)
