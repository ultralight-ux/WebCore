set(WTF_LIBRARY_TYPE STATIC)

list(APPEND WTF_SOURCES
    generic/WorkQueueGeneric.cpp
    text/ultralight/TextBreakIteratorInternalICUUltralight.cpp
    text/win/StringWin.cpp
    win/CPUTimeWin.cpp
    win/DbgHelperWin.cpp
    win/FileSystemWin.cpp
    win/LanguageWin.cpp
    win/MainThreadWin.cpp
    win/MemoryFootprintWin.cpp
    win/MemoryPressureHandlerWin.cpp
    win/OSAllocatorWin.cpp
    win/PathWalker.cpp
    win/ThreadingWin.cpp
    win/ThreadSpecificWin.cpp
)

list(APPEND WTF_LIBRARIES
   DbgHelp
   winmm
   shlwapi
)

if (USE_GSTREAMER)
    list(APPEND WTF_SOURCES
        glib/GLibUtilities.cpp
        glib/GRefPtr.cpp
        glib/RunLoopGLib.cpp
    )

    list(APPEND WTF_INCLUDE_DIRECTORIES
        ${GSTREAMER_DIR}/include/glib-2.0
        ${GSTREAMER_DIR}/lib/glib-2.0/include
        ${ULTRALIGHTCORE_DIR}/include
    )
    
    list(APPEND WTF_LIBRARIES
        glib-2.0
        gobject-2.0
        gio-2.0
        UltralightCore
    )
else ()
    list(APPEND WTF_SOURCES
        generic/RunLoopGeneric.cpp
    )
endif ()

if (${CMAKE_BUILD_TYPE} MATCHES Release OR ${CMAKE_BUILD_TYPE} MATCHES MinSizeRel)
  if (MSVC)
    add_compile_options(/GL)
  endif()
endif()

set(WTF_PRE_BUILD_COMMAND "${CMAKE_BINARY_DIR}/DerivedSources/wtf/preBuild.cmd")
file(WRITE "${WTF_PRE_BUILD_COMMAND}" "@xcopy /y /s /d /f \"${WTF_DIR}/wtf/*.h\" \"${DERIVED_SOURCES_DIR}/ForwardingHeaders/wtf\" >nul 2>nul\n@xcopy /y /s /d /f \"${DERIVED_SOURCES_DIR}/wtf/*.h\" \"${DERIVED_SOURCES_DIR}/ForwardingHeaders/wtf\" >nul 2>nul\n")
file(MAKE_DIRECTORY ${DERIVED_SOURCES_DIR}/ForwardingHeaders/WTF)

set(WTF_OUTPUT_NAME WTF${DEBUG_SUFFIX})
