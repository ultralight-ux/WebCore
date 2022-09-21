if (CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(ARCHITECTURE "x64")
else ()
    set(ARCHITECTURE "x86")
endif ()

get_filename_component(WEBKIT_LIBRARIES_DIR "${CMAKE_CURRENT_LIST_DIR}/deps/WebKitLibraries" REALPATH)
get_filename_component(ULTRALIGHTCORE_DIR "${CMAKE_CURRENT_LIST_DIR}/deps/UltralightCore" REALPATH)
get_filename_component(GSTREAMER_DIR "${CMAKE_CURRENT_LIST_DIR}/deps/GStreamer" REALPATH)

if (PORT MATCHES "UltralightLinux")
    set(PLATFORM "linux")
    set(WEBCORE_DEPS_PRODUCTS "")
elseif (PORT MATCHES "UltralightMac")
    set(PLATFORM "mac")
    set(WEBCORE_DEPS_PRODUCTS "")
elseif (PORT MATCHES "UltralightWin")
    set(PLATFORM "win")
    set(WEBCORE_DEPS_PRODUCTS "")
endif ()

set(WEBCORE_DEPS_REV "df325984")
set(GSTREAMER_REV "e78cae57")
set(ULTRALIGHTCORE_REV "76195313")

if (${ALLINONE_BUILD})
  message("Using local deps from all-in-one build.")
  get_filename_component(WEBKIT_LIBRARIES_DIR "${CMAKE_INSTALL_PREFIX}" REALPATH)
  get_filename_component(ULTRALIGHTCORE_DIR "${CMAKE_INSTALL_PREFIX}" REALPATH)
  get_filename_component(GSTREAMER_DIR "${CMAKE_INSTALL_PREFIX}" REALPATH)
  add_custom_target(WebCoreDeps)
  add_custom_target(UltralightCoreBin)
  add_custom_target(GStreamerBin)
elseif(${USE_LOCAL_DEPS})
  message("Using local deps.")
  add_custom_target(WebCoreDeps)
  add_custom_target(UltralightCoreBin)
  add_custom_target(GStreamerBin)
else ()
  ExternalProject_Add(WebCoreDeps
    URL https://webcore-deps-bin.sfo2.digitaloceanspaces.com/webcore-deps-bin-${WEBCORE_DEPS_REV}-${PLATFORM}-${ARCHITECTURE}.7z
    SOURCE_DIR "${WEBKIT_LIBRARIES_DIR}"
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ${CMAKE_COMMAND} -E echo_append #dummy command
    BUILD_BYPRODUCTS ${WEBCORE_DEPS_PRODUCTS}
    INSTALL_COMMAND ""
    INSTALL_DIR ${PROJECT_BINARY_DIR}/dummyInstall
  )

  ExternalProject_Add(UltralightCoreBin
    URL https://ultralightcore-bin.sfo2.digitaloceanspaces.com/ultralightcore-bin-${ULTRALIGHTCORE_REV}-${PLATFORM}-${ARCHITECTURE}.7z
    SOURCE_DIR "${ULTRALIGHTCORE_DIR}"
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ${CMAKE_COMMAND} -E echo_append #dummy command
    INSTALL_COMMAND ""
    INSTALL_DIR ${PROJECT_BINARY_DIR}/dummyInstall
  )

  ExternalProject_Add(GStreamerBin
    URL https://gstreamer-bin.sfo2.digitaloceanspaces.com/gstreamer-bin-${GSTREAMER_REV}-${PLATFORM}-${ARCHITECTURE}.7z
    SOURCE_DIR "${GSTREAMER_DIR}"
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ${CMAKE_COMMAND} -E echo_append #dummy command
    INSTALL_COMMAND ""
    INSTALL_DIR ${PROJECT_BINARY_DIR}/dummyInstall
  )
endif ()