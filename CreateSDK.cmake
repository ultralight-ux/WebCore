INSTALL(TARGETS WebCore
  RUNTIME DESTINATION "bin"
  LIBRARY DESTINATION "bin"
  ARCHIVE DESTINATION "lib"
)

if (STATIC_BUILD)
  INSTALL(TARGETS WTF JavaScriptCore PAL sqlite3
    RUNTIME DESTINATION "bin"
    LIBRARY DESTINATION "bin"
    ARCHIVE DESTINATION "lib"
  )
endif ()

INSTALL(DIRECTORY "${DERIVED_SOURCES_DIR}" DESTINATION "." FILES_MATCHING PATTERN "*.h" PATTERN "*.js")
INSTALL(DIRECTORY "${PROJECT_SOURCE_DIR}/Source/WebCore/platform/ultralight"
        DESTINATION "DerivedSources/ForwardingHeaders/WebCore/platform" FILES_MATCHING PATTERN "*.h")
INSTALL(FILES "${PROJECT_SOURCE_DIR}/Source/WebCore/platform/graphics/ShadowBlur.h"
        DESTINATION "DerivedSources/ForwardingHeaders/WebCore/platform/graphics/")
INSTALL(DIRECTORY "${PROJECT_SOURCE_DIR}/Source/WebCore/platform/graphics/ultralight"
        DESTINATION "DerivedSources/ForwardingHeaders/WebCore/platform/graphics" FILES_MATCHING PATTERN "*.h")
INSTALL(DIRECTORY "${PROJECT_SOURCE_DIR}/Source/WebCore/platform/graphics/harfbuzz"
        DESTINATION "DerivedSources/ForwardingHeaders/WebCore/platform/graphics" FILES_MATCHING PATTERN "*.h")
INSTALL(DIRECTORY "${PROJECT_SOURCE_DIR}/Source/WebCore/platform/graphics/texmap"
        DESTINATION "DerivedSources/ForwardingHeaders/WebCore/platform/graphics" FILES_MATCHING PATTERN "*.h")
INSTALL(FILES "${PROJECT_SOURCE_DIR}/Source/WebCore/platform/MIMETypeRegistry.h"
        DESTINATION "DerivedSources/ForwardingHeaders/WebCore/platform/")
INSTALL(FILES "${PROJECT_SOURCE_DIR}/Source/WebCore/platform/FileChooser.h"
        DESTINATION "DerivedSources/ForwardingHeaders/WebCore/platform/")
INSTALL(DIRECTORY "${PROJECT_SOURCE_DIR}/Source/SQLite"
        DESTINATION "DerivedSources/ForwardingHeaders" FILES_MATCHING PATTERN "*.h")
INSTALL(DIRECTORY "${PROJECT_SOURCE_DIR}/Source/WebCore/storage"
        DESTINATION "DerivedSources/ForwardingHeaders/WebCore" FILES_MATCHING PATTERN "*.h")
INSTALL(FILES "${PROJECT_SOURCE_DIR}/Source/WebCore/config.h"
        DESTINATION "DerivedSources/ForwardingHeaders/WebCore/")

set(JAVASCRIPTCORE_HEADERS
  "${PROJECT_SOURCE_DIR}/Source/JavaScriptCore/API/JavaScript.h"
  "${PROJECT_SOURCE_DIR}/Source/JavaScriptCore/API/JSBase.h"
  "${PROJECT_SOURCE_DIR}/Source/JavaScriptCore/API/JSContextRef.h"
  "${PROJECT_SOURCE_DIR}/Source/JavaScriptCore/API/JSStringRef.h"
  "${PROJECT_SOURCE_DIR}/Source/JavaScriptCore/API/JSObjectRef.h"
  "${PROJECT_SOURCE_DIR}/Source/JavaScriptCore/API/JSTypedArray.h"
  "${PROJECT_SOURCE_DIR}/Source/JavaScriptCore/API/JSValueRef.h"
  "${PROJECT_SOURCE_DIR}/Source/JavaScriptCore/API/WebKitAvailability.h"
  "${PROJECT_SOURCE_DIR}/Source/JavaScriptCore/API/JSObjectRefPrivate.h"
  "${PROJECT_SOURCE_DIR}/Source/JavaScriptCore/API/JSRetainPtr.h"
  )

INSTALL(FILES ${JAVASCRIPTCORE_HEADERS} DESTINATION "include/JavaScriptCore/")

INSTALL(FILES "${CMAKE_SOURCE_DIR}/cacert.pem" DESTINATION "bin/resources/")

set(WEBINSPECTORUI_DIR "${CMAKE_SOURCE_DIR}/Source/WebInspectorUI")

INSTALL(DIRECTORY "${WEBINSPECTORUI_DIR}/UserInterface/" DESTINATION "inspector")
INSTALL(FILES "${WEBINSPECTORUI_DIR}/Localizations/en.lproj/localizedStrings.js"
              DESTINATION "inspector")
INSTALL(FILES "${DERIVED_SOURCES_JAVASCRIPTCORE_DIR}/inspector/InspectorBackendCommands.js"
              DESTINATION "inspector/Protocol")

if (PORT MATCHES "UltralightWin")
  if (${CMAKE_BUILD_TYPE} MATCHES Debug OR ${CMAKE_BUILD_TYPE} MATCHES RelWithDebInfo)
    INSTALL(FILES "${PROJECT_BINARY_DIR}/bin/WebCore.pdb" DESTINATION "bin")
  endif ()
endif ()

if (CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(ARCHITECTURE "x64")
else ()
    set(ARCHITECTURE "x86")
endif ()

set(WEBKITLIBRARIES_DIR "${CMAKE_SOURCE_DIR}/deps/WebKitLibraries")

INSTALL(FILES "${WEBKITLIBRARIES_DIR}/bin/icudt67l.dat" DESTINATION "bin/resources/")

if (USE_GSTREAMER)
    set(MEDIACONTROLS_ASSETS
      "${PROJECT_SOURCE_DIR}/Source/WebCore/Modules/mediacontrols/mediaControls.css"
      "${PROJECT_SOURCE_DIR}/Source/WebCore/Modules/mediacontrols/mediaControls.js"
      "${PROJECT_SOURCE_DIR}/Source/WebCore/Modules/mediacontrols/mediaControlsLocalizedStrings.js"
    )
    INSTALL(FILES ${MEDIACONTROLS_ASSETS} DESTINATION "bin/resources/")
endif ()

if (PORT MATCHES "UltralightLinux")
    set(PLATFORM "linux")
elseif (PORT MATCHES "UltralightMac")
    set(PLATFORM "mac")
elseif (PORT MATCHES "UltralightWin")
    if (UWP_PLATFORM)
        set(PLATFORM "win-uwp")
    else ()
        set(PLATFORM "win")
    endif ()
endif ()

set(INSTALL_DIR ${PROJECT_BINARY_DIR}/out)
    
add_custom_target(create_sdk ALL "${CMAKE_COMMAND}" 
    -D CMAKE_INSTALL_PREFIX:string=${INSTALL_DIR}
    -P "${CMAKE_CURRENT_BINARY_DIR}/cmake_install.cmake" 
    DEPENDS WebCore) 
    
# Get name of current branch
execute_process(
  COMMAND git rev-parse --abbrev-ref HEAD
  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
  OUTPUT_VARIABLE GIT_BRANCH
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

# Get status of current branch
execute_process(
  COMMAND git status --untracked-files=no --porcelain
  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
  OUTPUT_VARIABLE GIT_STATUS
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

# Get abbreviated commit hash of current branch
execute_process(
  COMMAND git rev-parse --short=8 HEAD
  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
  OUTPUT_VARIABLE GIT_COMMIT_HASH
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

# Get list of local, unpushed commits
execute_process(
  COMMAND git cherry -v
  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
  OUTPUT_VARIABLE GIT_CHERRY
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

if (APPLE)
  if (NOT ${BUILD_DBG})
    # Strip non-essential symbols from dylib in release build on macOS
    add_custom_command(TARGET create_sdk POST_BUILD
        COMMAND strip -u -r bin/libWebCore.dylib
        WORKING_DIRECTORY ${INSTALL_DIR}
    )
  endif ()
endif ()

set(PKG_FILENAME "webcore-bin-${GIT_COMMIT_HASH}-${PLATFORM}-${ARCHITECTURE}.7z")

if (NOT GIT_STATUS STREQUAL "")
    add_custom_command(TARGET create_sdk POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E echo "NOTE: No release archive created, working directory has been modified."
    )
elseif (NOT GIT_CHERRY STREQUAL "")
    add_custom_command(TARGET create_sdk POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E echo "NOTE: No release archive created, branch needs to be pushed to remote repository."
    )
else ()
    add_custom_command(TARGET create_sdk POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E tar "cf" ${PROJECT_BINARY_DIR}/${PKG_FILENAME} --format=7zip -- .
        WORKING_DIRECTORY ${INSTALL_DIR}
    )
    add_custom_command(TARGET create_sdk POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E echo "Created release archive: ${PROJECT_BINARY_DIR}/${PKG_FILENAME}"
    )
endif ()