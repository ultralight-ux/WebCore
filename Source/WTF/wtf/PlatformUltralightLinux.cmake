set(WTF_LIBRARY_TYPE STATIC)

list(APPEND WTF_SOURCES
    generic/MainThreadGeneric.cpp
	generic/WorkQueueGeneric.cpp

    posix/CPUTimePOSIX.cpp
    posix/FileSystemPOSIX.cpp
    posix/OSAllocatorPOSIX.cpp
    posix/ThreadingPOSIX.cpp
	
	text/ultralight/TextBreakIteratorInternalICUUltralight.cpp
    
    unix/LanguageUnix.cpp
    # unix/LoggingUnix.cpp
    unix/UniStdExtrasUnix.cpp
)

if (UL_ENABLE_ALLOCATOR_OVERRIDE)
    list(APPEND WTF_SOURCES
        AllocatorOverride.cpp
    )
endif ()

if (CMAKE_SYSTEM_NAME MATCHES "Linux")
    list(APPEND WTF_PUBLIC_HEADERS
        linux/CurrentProcessMemoryStatus.h
    )
    
    list(APPEND WTF_SOURCES
        linux/CurrentProcessMemoryStatus.cpp
        linux/MemoryFootprintLinux.cpp
        linux/RealTimeThreads.cpp
        unix/MemoryPressureHandlerUnix.cpp
    )
else ()
    list(APPEND WTF_SOURCES
        generic/MemoryFootprintGeneric.cpp
        generic/MemoryPressureHandlerGeneric.cpp
    )
endif ()

list(APPEND WTF_LIBRARIES
    pthread
    ${ICU_LIBRARIES}
)

list(APPEND WTF_INCLUDE_DIRECTORIES
    ${ULTRALIGHTCORE_DIR}/include
)

list(APPEND WTF_LIBRARIES
    UltralightCore
)

if (USE_GSTREAMER)
    list(APPEND WTF_SOURCES
        glib/GLibUtilities.cpp
        glib/GRefPtr.cpp
    )

    list(APPEND WTF_INCLUDE_DIRECTORIES
        ${GSTREAMER_DIR}/include
        ${GSTREAMER_DIR}/include/glib-2.0
        ${GSTREAMER_DIR}/lib/glib-2.0/include
    )
    
    list(APPEND WTF_LIBRARIES
        glib-2.0
        gobject-2.0
        gio-2.0
    )
endif ()

list(APPEND WTF_SOURCES
    generic/RunLoopGeneric.cpp
)

file(MAKE_DIRECTORY ${DERIVED_SOURCES_DIR}/wtf)
file(MAKE_DIRECTORY ${DERIVED_SOURCES_DIR}/ForwardingHeaders/wtf)

set(BUILD_TMP "${PROJECT_BINARY_DIR}/tmp/")
file(MAKE_DIRECTORY "${BUILD_TMP}")

set(WTF_PRE_BUILD_COMMAND "${DERIVED_SOURCES_DIR}/wtf/preBuild.sh")

# Write the pre-build bash script
file(WRITE "${BUILD_TMP}/preBuild.sh" "#!/bin/bash\nrsync -aqW ${WTF_DIR}/wtf/*.h ${DERIVED_SOURCES_DIR}/ForwardingHeaders/wtf/ 2>/dev/null\n")
file(APPEND "${BUILD_TMP}/preBuild.sh" "rsync -aqW ${WTF_DIR}/wtf/text/*.h ${DERIVED_SOURCES_DIR}/ForwardingHeaders/wtf/text/ 2>/dev/null\n")
file(APPEND "${BUILD_TMP}/preBuild.sh" "rsync -aqW ${WTF_DIR}/wtf/text/icu/*.h ${DERIVED_SOURCES_DIR}/ForwardingHeaders/wtf/text/icu/ 2>/dev/null\n")
file(APPEND "${BUILD_TMP}/preBuild.sh" "rsync -aqW ${WTF_DIR}/wtf/threads/*.h ${DERIVED_SOURCES_DIR}/ForwardingHeaders/wtf/threads/ 2>/dev/null\n")
file(APPEND "${BUILD_TMP}/preBuild.sh" "rsync -aqW ${WTF_DIR}/wtf/unicode/*.h ${DERIVED_SOURCES_DIR}/ForwardingHeaders/wtf/unicode/ 2>/dev/null\n")
file(APPEND "${BUILD_TMP}/preBuild.sh" "rsync -aqW ${WTF_DIR}/wtf/glib/*.h ${DERIVED_SOURCES_DIR}/ForwardingHeaders/wtf/glib/ 2>/dev/null\n")

# Copy bash scripts over and chmod to executable
file (COPY "${BUILD_TMP}/preBuild.sh" DESTINATION "${DERIVED_SOURCES_DIR}/wtf" FILE_PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ)

# Remove temp directory
file(REMOVE_RECURSE "${BUILD_TMP}")
