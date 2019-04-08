INSTALL(TARGETS WebCore
  RUNTIME DESTINATION "bin"
  LIBRARY DESTINATION "bin"
  ARCHIVE DESTINATION "lib"
)

INSTALL(DIRECTORY "${DERIVED_SOURCES_DIR}" DESTINATION "." FILES_MATCHING PATTERN "*.h" PATTERN "*.js")
INSTALL(DIRECTORY "${PROJECT_SOURCE_DIR}/Source/WebCore/platform/ultralight"
        DESTINATION "DerivedSources/ForwardingHeaders/WebCore/platform" FILES_MATCHING PATTERN "*.h")
INSTALL(DIRECTORY "${PROJECT_SOURCE_DIR}/Source/WebCore/platform/graphics/ultralight"
        DESTINATION "DerivedSources/ForwardingHeaders/WebCore/platform/graphics" FILES_MATCHING PATTERN "*.h")
INSTALL(DIRECTORY "${PROJECT_SOURCE_DIR}/Source/WebCore/platform/graphics/harfbuzz"
        DESTINATION "DerivedSources/ForwardingHeaders/WebCore/platform/graphics" FILES_MATCHING PATTERN "*.h")
INSTALL(FILES "${PROJECT_SOURCE_DIR}/Source/WebCore/platform/MIMETypeRegistry.h"
        DESTINATION "DerivedSources/ForwardingHeaders/WebCore/platform/")
INSTALL(FILES "${PROJECT_SOURCE_DIR}/Source/WebCore/platform/FileChooser.h"
        DESTINATION "DerivedSources/ForwardingHeaders/WebCore/platform/")
INSTALL(DIRECTORY "${PROJECT_SOURCE_DIR}/Source/SQLite"
        DESTINATION "DerivedSources/ForwardingHeaders" FILES_MATCHING PATTERN "*.h")
INSTALL(DIRECTORY "${PROJECT_SOURCE_DIR}/Source/WebCore/storage"
        DESTINATION "DerivedSources/ForwardingHeaders/WebCore" FILES_MATCHING PATTERN "*.h")
        
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
  COMMAND git rev-parse --short HEAD
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
    
if (PORT MATCHES "UltralightLinux")
    set(PLATFORM "linux")
elseif (PORT MATCHES "UltralightMac")
    set(PLATFORM "mac")
elseif (PORT MATCHES "UltralightWin")
    set(PLATFORM "win")
endif ()

if (APPLE)
    # Strip non-essential symbols from dylib on macOS
    # You should disable this is you need debug symbols.
    add_custom_command(TARGET create_sdk POST_BUILD
        COMMAND strip -u -r bin/libWebCore.dylib
        WORKING_DIRECTORY ${INSTALL_DIR}
    )
endif ()

set(PKG_FILENAME "webcore-bin-${GIT_COMMIT_HASH}-${PLATFORM}-${ARCHITECTURE}.7z")

if (NOT GIT_BRANCH MATCHES "master")
    add_custom_command(TARGET create_sdk POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E echo "NOTE: No release archive created, not currently on master branch."
    )
elseif (NOT GIT_STATUS STREQUAL "")
    add_custom_command(TARGET create_sdk POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E echo "NOTE: No release archive created, working directory not clean."
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