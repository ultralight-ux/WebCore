set(WTF_LIBRARY_TYPE STATIC)

list(APPEND WTF_PUBLIC_HEADERS
    WeakObjCPtr.h

    cf/CFURLExtras.h
    cf/SpanCF.h
    cf/TypeCastsCF.h
    cf/VectorCF.h

    cocoa/CrashReporter.h
    cocoa/Entitlements.h
    cocoa/NSURLExtras.h
    cocoa/RuntimeApplicationChecksCocoa.h
    cocoa/SoftLinking.h
    cocoa/SpanCocoa.h
    cocoa/TollFreeBridging.h
    cocoa/TypeCastsCocoa.h
    cocoa/VectorCocoa.h

    darwin/WeakLinking.h

    spi/cf/CFBundleSPI.h
    spi/cf/CFStringSPI.h

    spi/cocoa/CFXPCBridgeSPI.h
    spi/cocoa/CrashReporterClientSPI.h
    spi/cocoa/MachVMSPI.h
    spi/cocoa/NSLocaleSPI.h
    spi/cocoa/NSObjCRuntimeSPI.h
    spi/cocoa/SecuritySPI.h
    spi/cocoa/objcSPI.h

    spi/darwin/CodeSignSPI.h
    spi/darwin/DataVaultSPI.h
    spi/darwin/OSVariantSPI.h
    spi/darwin/ProcessMemoryFootprint.h
    spi/darwin/SandboxSPI.h
    spi/darwin/XPCSPI.h
    spi/darwin/dyldSPI.h

    spi/mac/MetadataSPI.h

    text/cf/StringConcatenateCF.h
    text/cf/TextBreakIteratorCF.h
)

list(APPEND WTF_SOURCES   
    generic/MainThreadGeneric.cpp
	generic/WorkQueueGeneric.cpp

    posix/CPUTimePOSIX.cpp
    posix/FileSystemPOSIX.cpp
    posix/OSAllocatorPOSIX.cpp
    posix/ThreadingPOSIX.cpp

    cocoa/AutodrainedPool.cpp
    cocoa/MemoryFootprintCocoa.cpp
    cocoa/MemoryPressureHandlerCocoa.mm
    cocoa/MachSendRight.cpp
    cocoa/FileSystemCocoa.mm
    cocoa/URLCocoa.mm

    cf/FileSystemCF.cpp
    cf/URLCF.cpp
    cf/CFURLExtras.cpp
    cf/LanguageCF.cpp

    text/cf/AtomStringImplCF.cpp
    text/cf/StringCF.cpp
    text/cf/StringImplCF.cpp
    text/cf/StringViewCF.cpp

    text/cocoa/StringViewCocoa.mm
	
    text/ultralight/TextBreakIteratorInternalICUUltralight.cpp
)


if (UL_ENABLE_ALLOCATOR_OVERRIDE)
    list(APPEND WTF_SOURCES
        AllocatorOverride.cpp
    )
endif ()

if (ENABLE_REMOTE_INSPECTOR)
    list(APPEND WTF_SOURCES   
        unix/UniStdExtrasUnix.cpp
    )
endif ()

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

file(COPY mac/MachExceptions.defs DESTINATION ${WTF_DERIVED_SOURCES_DIR})

add_custom_command(
    OUTPUT
        ${WTF_DERIVED_SOURCES_DIR}/MachExceptionsServer.h
        ${WTF_DERIVED_SOURCES_DIR}/mach_exc.h
        ${WTF_DERIVED_SOURCES_DIR}/mach_excServer.c
        ${WTF_DERIVED_SOURCES_DIR}/mach_excUser.c
    MAIN_DEPENDENCY mac/MachExceptions.defs
    WORKING_DIRECTORY ${WTF_DERIVED_SOURCES_DIR}
    COMMAND mig -sheader MachExceptionsServer.h MachExceptions.defs
    VERBATIM)
list(APPEND WTF_SOURCES
    ${WTF_DERIVED_SOURCES_DIR}/mach_excServer.c
    ${WTF_DERIVED_SOURCES_DIR}/mach_excUser.c
)

find_library(FOUNDATION Foundation)
find_library(CORE_FOUNDATION CoreFoundation)

list(APPEND WTF_LIBRARIES
   ${FOUNDATION}
   ${CORE_FOUNDATION}
   edit
)

file(MAKE_DIRECTORY ${DERIVED_SOURCES_DIR}/wtf)
file(MAKE_DIRECTORY ${DERIVED_SOURCES_DIR}/ForwardingHeaders/wtf)

set(BUILD_TMP "${CMAKE_BINARY_DIR}/tmp/")
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
