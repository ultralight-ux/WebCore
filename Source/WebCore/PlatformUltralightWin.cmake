add_definitions(-D__STDC_CONSTANT_MACROS -DSTATICALLY_LINKED_WITH_WTF
                -DSTATICALLY_LINKED_WITH_JavaScriptCore -DSTATICALLY_LINKED_WITH_PAL -DULTRALIGHT_IMPLEMENTATION
                -DCURL_STATICLIB)

include(platform/ImageDecoders.cmake)
include(platform/TextureMapper.cmake)
include(platform/Curl.cmake)

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
    "${WEBKIT_LIBRARIES_DIR}/include"
    "${WEBKIT_LIBRARIES_DIR}/include/SQLite"
    "${WEBKIT_LIBRARIES_DIR}/include/zlib"
	"${WEBKIT_LIBRARIES_DIR}/include/harfbuzz"
	"${WEBKIT_LIBRARIES_DIR}/include/libjpeg"
	"${WEBKIT_LIBRARIES_DIR}/include/libpng"
    "${WEBCORE_DIR}/platform/ultralight"
	"${WEBCORE_DIR}/platform/graphics/ultralight"
	"${WEBCORE_DIR}/platform/graphics/harfbuzz"

	"${ULTRALIGHTCORE_DIR}/include"

	"${SQLITE_DIR}"
)

list(APPEND WebCore_SOURCES
    platform/win/WebCoreInstanceHandle.cpp
    platform/win/SSLKeyGeneratorWin.cpp
    platform/generic/KeyedEncoderGeneric.cpp
    platform/generic/KeyedDecoderGeneric.cpp
    platform/generic/ScrollAnimatorGeneric.cpp
    platform/ScrollAnimationKinetic.cpp
    platform/ScrollAnimationSmooth.cpp
    platform/network/ultralight/CurlSSLHandleUltralight.cpp
    platform/network/win/NetworkStateNotifierWin.cpp
    page/win/ResourceUsageThreadWin.cpp

    platform/graphics/harfbuzz/ComplexTextControllerHarfBuzz.cpp

    platform/ultralight/AXObjectCacheUltralight.cpp
    platform/ultralight/CryptoDigestUltralight.cpp
    platform/ultralight/CursorUltralight.cpp
	platform/ultralight/DNSResolveQueueUltralight.cpp
	platform/ultralight/EditorUltralight.cpp
    platform/ultralight/EventHandlerUltralight.cpp
	platform/ultralight/EventLoopUltralight.cpp
	platform/ultralight/FileSystemUltralight.cpp
	platform/ultralight/IconUltralight.cpp
	platform/ultralight/ImageUltralight.cpp
	platform/ultralight/KeyEventUltralight.cpp
	platform/ultralight/LoggingUltralight.cpp
	platform/ultralight/MIMETypeRegistryUltralight.cpp
	platform/ultralight/PasteboardUltralight.cpp
	platform/ultralight/PlatformMouseEventUltralight.cpp
	platform/ultralight/PlatformScreenUltralight.cpp
	platform/ultralight/RenderThemeUltralight.cpp
	platform/ultralight/ScrollbarThemeUltralight.cpp
	platform/ultralight/SharedBufferUltralight.cpp
	platform/ultralight/SoundUltralight.cpp
	platform/ultralight/SSLKeyGeneratorUltralight.cpp
	platform/ultralight/SynchronousLoaderClientUltralight.cpp
	platform/ultralight/TextBreakIteratorInternalICUUltralight.cpp
	platform/ultralight/WidgetUltralight.cpp
	platform/ultralight/LocalizedStringsUltralight.cpp
	platform/ultralight/HyphenationUltralight.cpp
	platform/ultralight/PreserveSymbolsUltralight.cpp
	platform/ultralight/FileURLLoader.h
	platform/ultralight/FileURLLoader.cpp
	platform/ultralight/DragImageUltralight.cpp
	platform/ultralight/DragDataUltralight.cpp
	platform/ultralight/DragControllerUltralight.cpp
    platform/ultralight/UTFUltralight.cpp
    platform/ultralight/ICUUtils.cpp
    platform/ultralight/MemoryUtils.cpp
    platform/ultralight/ResourceUsageOverlayUltralight.cpp

	platform/graphics/ultralight/GradientUltralight.cpp
	platform/graphics/ultralight/GraphicsContextPlatformPrivateUltralight.h
	platform/graphics/ultralight/GraphicsContextUltralight.cpp
	platform/graphics/ultralight/ImageBufferDataUltralight.h
	platform/graphics/ultralight/ImageBufferUltralight.cpp
	platform/graphics/ultralight/ImageUltralight.cpp
	platform/graphics/ultralight/IntRectUltralight.cpp
	platform/graphics/ultralight/PathUltralight.cpp
	platform/graphics/ultralight/PatternUltralight.cpp
	platform/graphics/ultralight/PlatformContextUltralight.cpp
	platform/graphics/ultralight/PlatformContextUltralight.h
	platform/graphics/ultralight/RefPtrUltralight.cpp
	platform/graphics/ultralight/RefPtrUltralight.h
	platform/graphics/ultralight/TransformationMatrixUltralight.cpp
	platform/graphics/ultralight/BitmapImageUltralight.cpp
	platform/graphics/ultralight/FloatRectUltralight.cpp
	platform/graphics/ultralight/FontCacheUltralight.cpp
	platform/graphics/ultralight/FontCustomPlatformData.cpp
	platform/graphics/ultralight/FontCustomPlatformData.h
	platform/graphics/ultralight/FontPlatformDataUltralight.cpp
	platform/graphics/ultralight/FontUltralight.cpp
	platform/graphics/ultralight/FreeTypeLib.cpp
	platform/graphics/ultralight/GlyphPageUltralight.cpp
	platform/graphics/ultralight/ImageDecoderUltralight.cpp
	platform/graphics/ultralight/UltralightStubs.cpp
	platform/graphics/ultralight/NativeImageUltralight.cpp
	platform/graphics/ultralight/FontRenderer.h
	platform/graphics/ultralight/FontRenderer.cpp
	platform/graphics/ultralight/CanvasImage.h
	platform/graphics/ultralight/CanvasImage.cpp
    platform/graphics/ultralight/RefPtrFreeTypeFace.cpp
    platform/graphics/ultralight/RefPtrFreeTypeFace.h

	platform/image-decoders/ultralight/ImageBackingStoreUltralight.cpp
	platform/graphics/harfbuzz/HarfBuzzFaceUltralight.cpp
    platform/Cursor.cpp
    platform/LocalizedStrings.cpp
	platform/text/LocaleNone.cpp
)

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
    ssl
    tls
    xml2
    xslt
    zlibstatic
    sqlite3
)

if (UWP_PLATFORM)
    list(APPEND WebCore_LIBRARIES
        Pathcch
    )
endif ()

if (NOT UWP_PLATFORM)
    list(APPEND WebCore_LIBRARIES
        comctl32
        crypt32
        iphlpapi
        rpcrt4
        shlwapi
        usp10
        version
        winmm
        ws2_32
        wldap32
        normaliz
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

if (MSVC)
	if (${CMAKE_BUILD_TYPE} MATCHES Release OR ${CMAKE_BUILD_TYPE} MATCHES MinSizeRel)
		add_compile_options(/GL)
		set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /LTCG")
		set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} /LTCG")
	endif ()
endif ()

if (UWP_PLATFORM)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /FI \"${WEBCORE_DIR}/UWPDefs.h\"")
endif ()

list(APPEND WebCore_LIBRARIES WTF${DEBUG_SUFFIX})
list(APPEND WebCore_LIBRARIES JavaScriptCore${DEBUG_SUFFIX})
list(APPEND WebCore_LIBRARIES UltralightCore)
list(APPEND WebCoreTestSupport_LIBRARIES WTF${DEBUG_SUFFIX})
