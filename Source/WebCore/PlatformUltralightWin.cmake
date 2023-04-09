add_definitions(/bigobj -D__STDC_CONSTANT_MACROS -DSTATICALLY_LINKED_WITH_WTF
                -DSTATICALLY_LINKED_WITH_JavaScriptCore -DSTATICALLY_LINKED_WITH_PAL -DULTRALIGHT_IMPLEMENTATION
                -DCURL_STATICLIB)

include(platform/ImageDecoders.cmake)
include(platform/TextureMapper.cmake)
include(platform/Curl.cmake)
include(platform/Ultralight.cmake)

if (USE_GSTREAMER)
    include(platform/GStreamer.cmake)
endif ()

list(APPEND WebCore_PRIVATE_INCLUDE_DIRECTORIES
    "${CMAKE_BINARY_DIR}/../include/private"
    "${CMAKE_BINARY_DIR}/../include/private/JavaScriptCore"
    "${WEBCORE_DIR}/platform/graphics/egl"
    "${WEBCORE_DIR}/platform/graphics/opengl"
    "${WEBCORE_DIR}/platform/graphics/opentype"
    "${WEBCORE_DIR}/platform/mediacapabilities"
)

list(APPEND WebCore_INCLUDE_DIRECTORIES
    "${DERIVED_SOURCES_DIR}/ForwardingHeaders"
)

if (USE_CF)
    list(APPEND WebCore_PRIVATE_INCLUDE_DIRECTORIES
        "${WEBCORE_DIR}/platform/cf"
    )
endif ()

list(APPEND WebCore_INCLUDE_DIRECTORIES
    "${DirectX_INCLUDE_DIRS}"
    "${SQLITE_DIR}"
    "${ULTRALIGHTCORE_DIR}/include"
    "${WEBCORE_DIR}/platform/graphics/harfbuzz"
    "${WEBKIT_LIBRARIES_DIR}/include"
    "${WEBKIT_LIBRARIES_DIR}/include/freetype2"
    "${WEBKIT_LIBRARIES_DIR}/include/harfbuzz"
)

list(APPEND WebCore_SOURCES
    #platform/win/SSLKeyGeneratorWin.cpp
    page/win/ResourceUsageThreadWin.cpp
    platform/Cursor.cpp
    platform/generic/KeyedDecoderGeneric.cpp
    platform/generic/KeyedEncoderGeneric.cpp
    platform/graphics/harfbuzz/ComplexTextControllerHarfBuzz.cpp
    platform/graphics/harfbuzz/HarfBuzzFaceUltralight.cpp
    
    platform/LocalizedStrings.cpp

    platform/ScrollAnimationKinetic.cpp
    platform/ScrollAnimationSmooth.cpp
    platform/text/LocaleNone.cpp
    
    platform/win/WebCoreInstanceHandle.cpp
#    platform/generic/ScrollAnimatorGeneric.cpp
)

if (USE_GSTREAMER)
    list(APPEND WebCore_SOURCES
        platform/graphics/gstreamer/ImageGStreamerUltralight.cpp
        platform/audio/ultralight/AudioBusUltralight.cpp
    )
endif ()

list(APPEND WebCore_USER_AGENT_STYLE_SHEETS
    ${WEBCORE_DIR}/css/themeWin.css
    ${WEBCORE_DIR}/css/themeWinQuirks.css
)

list(APPEND WebCore_LIBRARIES
    brotlicommon
    brotlidec
    brotlienc
    crypto
    freetype
    harfbuzz
    harfbuzz-icu
    jpeg-static
    libcurl
    libpng16_static
    nghttp2_static
    sqlite3
    ssl
    tls
    xml2
    xslt
    zlibstatic
)

if (NOT UWP_PLATFORM)
    list(APPEND WebCore_LIBRARIES
        bcrypt
        comctl32
        crypt32
        iphlpapi
        normaliz
        rpcrt4
        shlwapi
        usp10
        version
        winmm
        wldap32
        ws2_32
    )
endif ()

if (USE_GSTREAMER)
    list(APPEND WebCore_LIBRARIES
        gstreamer-full-1.0
    )
endif ()

make_directory(${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/WebKit.resources/en.lproj)
file(COPY
    "${WEBCORE_DIR}/en.lproj/Localizable.strings"
    "${WEBCORE_DIR}/en.lproj/mediaControlsLocalizedStrings.js"
    DESTINATION
    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/WebKit.resources/en.lproj
)
file(COPY
    "${WEBCORE_DIR}/Modules/mediacontrols/mediaControlsApple.css"
    "${WEBCORE_DIR}/Modules/mediacontrols/mediaControlsApple.js"
    DESTINATION
    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/WebKit.resources
)
if (EXISTS ${WEBKIT_LIBRARIES_DIR}/cacert.pem)
    make_directory(${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/WebKit.resources/certificates)
    file(COPY
        ${WEBKIT_LIBRARIES_DIR}/cacert.pem
        DESTINATION
        ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/WebKit.resources/certificates
    )
endif ()

file(MAKE_DIRECTORY ${DERIVED_SOURCES_DIR}/ForwardingHeaders/WebCore)

set(WebCore_DERIVED_SOURCES_PRE_BUILD_COMMAND "${CMAKE_BINARY_DIR}/DerivedSources/WebCore/preBuild.cmd")
file(WRITE "${WebCore_DERIVED_SOURCES_PRE_BUILD_COMMAND}" "@xcopy /y /s /d /f \"${WEBCORE_DIR}/ForwardingHeaders/*.h\" \"${DERIVED_SOURCES_DIR}/ForwardingHeaders/WebCore\" >nul 2>nul\n")
foreach (_directory ${WebCore_FORWARDING_HEADERS_DIRECTORIES})
    file(APPEND "${WebCore_DERIVED_SOURCES_PRE_BUILD_COMMAND}" "@xcopy /y /d /f \"${WEBCORE_DIR}/${_directory}/*.h\" \"${DERIVED_SOURCES_DIR}/ForwardingHeaders/WebCore\" >nul 2>nul\n")
endforeach ()

set(WebCore_POST_BUILD_COMMAND "${CMAKE_BINARY_DIR}/DerivedSources/WebCore/postBuild.cmd")
file(WRITE "${WebCore_POST_BUILD_COMMAND}" "@xcopy /y /s /d /f \"${WebCore_DERIVED_SOURCES_DIR}/*.h\" \"${DERIVED_SOURCES_DIR}/ForwardingHeaders/WebCore\" >nul 2>nul\n")

set(WebCore_OUTPUT_NAME
    WebCore${DEBUG_SUFFIX}
)

if (${CMAKE_BUILD_TYPE} MATCHES Release OR ${CMAKE_BUILD_TYPE} MATCHES MinSizeRel)
  if (MSVC)
    if (NOT UL_ENABLE_STATIC_BUILD)
        list(APPEND WebCore_COMPILE_OPTIONS /GL)
        set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /LTCG")
        set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} /LTCG")
    endif ()
  endif()
endif()

if (UWP_PLATFORM)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /FI \"${WEBCORE_DIR}/UWPDefs.h\"")
endif ()

list(APPEND WebCore_LIBRARIES WTF${DEBUG_SUFFIX})
list(APPEND WebCore_LIBRARIES JavaScriptCore${DEBUG_SUFFIX})
#list(APPEND WebCore_LIBRARIES UltralightCore)
list(APPEND WebCoreTestSupport_LIBRARIES WTF${DEBUG_SUFFIX})
