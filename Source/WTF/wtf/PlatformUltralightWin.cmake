set(WTF_LIBRARY_TYPE STATIC)

list(APPEND WTF_SOURCES
    generic/RunLoopGeneric.cpp
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

    # Needed for loading ICU data at runtime instead of from shared lib
    unicode/icu/stubdata.cpp
)

list(APPEND WTF_LIBRARIES
   DbgHelp
   winmm
   shlwapi
)

set(WTF_PRE_BUILD_COMMAND "${CMAKE_BINARY_DIR}/DerivedSources/wtf/preBuild.cmd")
file(WRITE "${WTF_PRE_BUILD_COMMAND}" "@xcopy /y /s /d /f \"${WTF_DIR}/wtf/*.h\" \"${DERIVED_SOURCES_DIR}/ForwardingHeaders/wtf\" >nul 2>nul\n@xcopy /y /s /d /f \"${DERIVED_SOURCES_DIR}/wtf/*.h\" \"${DERIVED_SOURCES_DIR}/ForwardingHeaders/wtf\" >nul 2>nul\n")
file(MAKE_DIRECTORY ${DERIVED_SOURCES_DIR}/ForwardingHeaders/WTF)

set(WTF_OUTPUT_NAME WTF${DEBUG_SUFFIX})
