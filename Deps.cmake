if (CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(ARCHITECTURE "x64")
else ()
    set(ARCHITECTURE "x86")
endif ()

get_filename_component(WEBCORE_DEPS_DIR "${CMAKE_CURRENT_LIST_DIR}/deps/WebKitLibraries" REALPATH)
get_filename_component(ULTRALIGHTCORE_DIR "${CMAKE_CURRENT_LIST_DIR}/deps/UltralightCore/" REALPATH)

if (PORT MATCHES "UltralightLinux")
    set(PLATFORM "linux")
    set(WEBCORE_DEPS_PRODUCTS "${WEBCORE_DEPS_DIR}/lib/libicuuc.a ${WEBCORE_DEPS_DIR}/lib/libicui18n.a ${WEBCORE_DEPS_DIR}/lib/libicudata.a")
elseif (PORT MATCHES "UltralightMac")
    set(PLATFORM "mac")
    set(WEBCORE_DEPS_PRODUCTS "${WEBCORE_DEPS_DIR}/lib/libicuuc.a ${WEBCORE_DEPS_DIR}/lib/libicui18n.a ${WEBCORE_DEPS_DIR}/lib/libicudata.a")
elseif (PORT MATCHES "UltralightWin")
    set(PLATFORM "win")
    set(WEBCORE_DEPS_PRODUCTS "")
endif ()

set(WEBCORE_DEPS_REV "1.0")
set(ULTRALIGHTCORE_REV "84bd860")

ExternalProject_Add(WebCoreDeps
  URL https://github.com/ultralight-ux/WebCore-deps/releases/download/${WEBCORE_DEPS_REV}/webcore-deps-bin-${WEBCORE_DEPS_REV}-${PLATFORM}-${ARCHITECTURE}.7z
  SOURCE_DIR "${WEBCORE_DEPS_DIR}"
  CONFIGURE_COMMAND ""
  BUILD_COMMAND ${CMAKE_COMMAND} -E echo_append #dummy command
  BUILD_BYPRODUCTS ${WEBCORE_DEPS_PRODUCTS}
  INSTALL_COMMAND ""
  INSTALL_DIR ${PROJECT_BINARY_DIR}/dummyInstall
)

ExternalProject_Add(UltralightCoreBin
  URL https://github.com/ultralight-ux/UltralightCore-bin/releases/download/${ULTRALIGHTCORE_REV}/ultralightcore-bin-${ULTRALIGHTCORE_REV}-${PLATFORM}-${ARCHITECTURE}.7z
  SOURCE_DIR "${ULTRALIGHT_CORE_DIR}"
  BUILD_IN_SOURCE 1
  CONFIGURE_COMMAND ""
  BUILD_COMMAND ""
  INSTALL_COMMAND ""
)
