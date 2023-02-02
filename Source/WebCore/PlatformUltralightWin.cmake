add_definitions(/bigobj -D__STDC_CONSTANT_MACROS -DSTATICALLY_LINKED_WITH_WTF
                -DSTATICALLY_LINKED_WITH_JavaScriptCore -DSTATICALLY_LINKED_WITH_PAL -DULTRALIGHT_IMPLEMENTATION
                -DCURL_STATICLIB)

include(platform/ImageDecoders.cmake)
include(platform/TextureMapper.cmake)
include(platform/Curl.cmake)

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
    "${WEBCORE_DIR}/platform/graphics/ultralight"
    "${WEBCORE_DIR}/platform/ultralight"
    "${WEBKIT_LIBRARIES_DIR}/include"
    "${WEBKIT_LIBRARIES_DIR}/include/freetype2"
    "${WEBKIT_LIBRARIES_DIR}/include/harfbuzz"
)

list(APPEND WebCore_SOURCES
    page/win/ResourceUsageThreadWin.cpp
    platform/Cursor.cpp
    platform/generic/KeyedDecoderGeneric.cpp
    platform/generic/KeyedEncoderGeneric.cpp
    platform/generic/ScrollAnimatorGeneric.cpp
    platform/graphics/harfbuzz/ComplexTextControllerHarfBuzz.cpp
    platform/graphics/harfbuzz/HarfBuzzFaceUltralight.cpp
    platform/graphics/ultralight/BitmapImageUltralight.cpp
    platform/graphics/ultralight/CanvasImage.cpp
    platform/graphics/ultralight/CanvasImage.h
    platform/graphics/ultralight/FloatRectUltralight.cpp
    platform/graphics/ultralight/FontCacheUltralight.cpp
    platform/graphics/ultralight/FontCustomPlatformData.cpp
    platform/graphics/ultralight/FontCustomPlatformData.h
    platform/graphics/ultralight/FontPlatformDataUltralight.cpp
    platform/graphics/ultralight/FontRenderer.cpp
    platform/graphics/ultralight/FontRenderer.h
    platform/graphics/ultralight/FontUltralight.cpp
    platform/graphics/ultralight/FreeTypeLib.cpp
    platform/graphics/ultralight/GlyphPageUltralight.cpp
    platform/graphics/ultralight/GradientUltralight.cpp
    platform/graphics/ultralight/GraphicsContextPlatformPrivateUltralight.h
    platform/graphics/ultralight/GraphicsContextUltralight.cpp
    platform/graphics/ultralight/ImageBufferDataUltralight.h
    platform/graphics/ultralight/ImageBufferUltralightBackend.cpp
    platform/graphics/ultralight/ImageBufferUltralightBackend.h
    platform/graphics/ultralight/ImageDecoderUltralight.cpp
    platform/graphics/ultralight/ImageUltralight.cpp
    platform/graphics/ultralight/IntRectUltralight.cpp
    platform/graphics/ultralight/NativeImageUltralight.cpp
    platform/graphics/ultralight/PathUltralight.cpp
    platform/graphics/ultralight/PatternUltralight.cpp
    platform/graphics/ultralight/PlatformContextUltralight.cpp
    platform/graphics/ultralight/PlatformContextUltralight.h
    platform/graphics/ultralight/PlatformFontFreeType.cpp
    platform/graphics/ultralight/PlatformFontFreeType.h
    platform/graphics/ultralight/RefPtrFreeTypeFace.cpp
    platform/graphics/ultralight/RefPtrFreeTypeFace.h
    platform/graphics/ultralight/RefPtrUltralight.cpp
    platform/graphics/ultralight/RefPtrUltralight.h
    platform/graphics/ultralight/TransformationMatrixUltralight.cpp
    platform/graphics/ultralight/UltralightStubs.cpp
    platform/image-decoders/ultralight/ImageBackingStoreUltralight.cpp
    platform/LocalizedStrings.cpp
    platform/network/ultralight/CurlSSLHandleUltralight.cpp
    platform/network/win/NetworkStateNotifierWin.cpp
    platform/ScrollAnimationKinetic.cpp
    platform/ScrollAnimationSmooth.cpp
    platform/text/LocaleNone.cpp
    platform/ultralight/AXObjectCacheUltralight.cpp
    platform/ultralight/CryptoDigestUltralight.cpp
    platform/ultralight/CursorUltralight.cpp
    platform/ultralight/DNSResolveQueueUltralight.cpp
    platform/ultralight/DragControllerUltralight.cpp
    platform/ultralight/DragDataUltralight.cpp
    platform/ultralight/DragImageUltralight.cpp
    platform/ultralight/EditorUltralight.cpp
    platform/ultralight/EventHandlerUltralight.cpp
    platform/ultralight/EventLoopUltralight.cpp
    platform/ultralight/FileSystemUltralight.cpp
    platform/ultralight/FileURLLoader.cpp
    platform/ultralight/FileURLLoader.h
    platform/ultralight/GamepadProviderUltralight.cpp
    platform/ultralight/GamepadProviderUltralight.h
    platform/ultralight/GamepadUltralight.h
    platform/ultralight/GamepadUltralight.cpp
    platform/ultralight/HyphenationUltralight.cpp
    platform/ultralight/IconUltralight.cpp
    platform/ultralight/ICUUtils.cpp
    platform/ultralight/ImageUltralight.cpp
    platform/ultralight/KeyEventUltralight.cpp
    platform/ultralight/LocalizedStringsUltralight.cpp
    platform/ultralight/LoggingUltralight.cpp
    platform/ultralight/MemoryUtils.cpp
    platform/ultralight/MIMETypeRegistryUltralight.cpp
    platform/ultralight/PasteboardUltralight.cpp
    platform/ultralight/PlatformMouseEventUltralight.cpp
    platform/ultralight/PlatformScreenUltralight.cpp
    platform/ultralight/PreserveSymbolsUltralight.cpp
    platform/ultralight/RenderThemeUltralight.cpp
    platform/ultralight/ResourceLoaderUltralight.cpp
    platform/ultralight/ResourceUsageOverlayUltralight.cpp
    platform/ultralight/ScrollbarThemeUltralight.cpp
    platform/ultralight/SharedBufferUltralight.cpp
    platform/ultralight/SoundUltralight.cpp
    platform/ultralight/SSLKeyGeneratorUltralight.cpp
    platform/ultralight/SynchronousLoaderClientUltralight.cpp
    platform/ultralight/TextBreakIteratorInternalICUUltralight.cpp
    platform/ultralight/UTFUltralight.cpp
    platform/ultralight/WidgetUltralight.cpp
    platform/win/SSLKeyGeneratorWin.cpp
    platform/win/WebCoreInstanceHandle.cpp
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

set(WebCore_FORWARDING_HEADERS_DIRECTORIES
    .
    accessibility
    bindings
    bridge
    contentextensions
    css
    dom
    editing
    fileapi
    history
    html
    inspector
    loader
    page
    platform
    plugins
    rendering
    storage
    style
    svg
    websockets
    workers
    xml
    Modules/geolocation
    Modules/indexeddb
    Modules/websockets
    Modules/indexeddb/client
    Modules/indexeddb/legacy
    Modules/indexeddb/server
    Modules/indexeddb/shared
    Modules/notifications
    Modules/webdatabase
    bindings/js
    bridge/c
    bridge/jsc
    css/parser
    history/cf
    html/forms
    html/parser
    html/shadow
    html/track
    loader/appcache
    loader/archive
    loader/cache
    loader/icon
    loader/archive/cf
    page/animation
    page/csp
    page/scrolling
    platform/animation
    platform/audio
    platform/cf
    platform/graphics
    platform/mock
    platform/network
    platform/network/curl
    platform/sql
    platform/text
    platform/ultralight
    platform/graphics/filters
    platform/graphics/opengl
    platform/graphics/opentype
    platform/graphics/texmap
    platform/graphics/transforms
    platform/mediastream/libwebrtc
    platform/text/transcoder
    rendering/line
    rendering/shapes
    rendering/style
    rendering/svg
    svg/animation
    svg/graphics
    svg/properties
    svg/graphics/filters
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
file(WRITE "${WebCore_POST_BUILD_COMMAND}" "@xcopy /y /s /d /f \"${DERIVED_SOURCES_WEBCORE_DIR}/*.h\" \"${DERIVED_SOURCES_DIR}/ForwardingHeaders/WebCore\" >nul 2>nul\n")

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
