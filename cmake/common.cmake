if(DEFINED COMMON_CMAKE)
  return()
else()
  set(COMMON_CMAKE 1)
endif()

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR})
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/compiler)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/flags)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/toolchains)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/util)

# Common utilities
include(load_sources)
include(show_fatal_error)
include(show_status)
include(show_warning)

# Load build options
include(build_options)

# Autodetect the toolchain (if none provided)
include(autodetect_toolchain)