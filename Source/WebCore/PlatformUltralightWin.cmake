add_definitions(/bigobj -D__STDC_CONSTANT_MACROS -DSTATICALLY_LINKED_WITH_WTF
                -DSTATICALLY_LINKED_WITH_JavaScriptCore -DULTRALIGHT_IMPLEMENTATION
                -DCURL_STATICLIB)

include(platform/ImageDecoders.cmake)
include(platform/TextureMapper.cmake)

list(APPEND WebCore_INCLUDE_DIRECTORIES
    "${CMAKE_BINARY_DIR}/../include/private"
    "${CMAKE_BINARY_DIR}/../include/private/JavaScriptCore"
    "${DERIVED_SOURCES_DIR}/ForwardingHeaders/ANGLE"
    "${DERIVED_SOURCES_DIR}/ForwardingHeaders/ANGLE/include/KHR"
    "${DERIVED_SOURCES_DIR}/ForwardingHeaders/JavaScriptCore"
    "${DERIVED_SOURCES_DIR}/ForwardingHeaders/JavaScriptCore/ForwardingHeaders"
    "${DERIVED_SOURCES_DIR}/ForwardingHeaders/JavaScriptCore/API"
    "${DERIVED_SOURCES_DIR}/ForwardingHeaders/JavaScriptCore/assembler"
    "${DERIVED_SOURCES_DIR}/ForwardingHeaders/JavaScriptCore/builtins"
    "${DERIVED_SOURCES_DIR}/ForwardingHeaders/JavaScriptCore/bytecode"
    "${DERIVED_SOURCES_DIR}/ForwardingHeaders/JavaScriptCore/bytecompiler"
    "${DERIVED_SOURCES_DIR}/ForwardingHeaders/JavaScriptCore/dfg"
    "${DERIVED_SOURCES_DIR}/ForwardingHeaders/JavaScriptCore/disassembler"
    "${DERIVED_SOURCES_DIR}/ForwardingHeaders/JavaScriptCore/heap"
    "${DERIVED_SOURCES_DIR}/ForwardingHeaders/JavaScriptCore/debugger"
    "${DERIVED_SOURCES_DIR}/ForwardingHeaders/JavaScriptCore/interpreter"
    "${DERIVED_SOURCES_DIR}/ForwardingHeaders/JavaScriptCore/jit"
    "${DERIVED_SOURCES_DIR}/ForwardingHeaders/JavaScriptCore/llint"
    "${DERIVED_SOURCES_DIR}/ForwardingHeaders/JavaScriptCore/parser"
    "${DERIVED_SOURCES_DIR}/ForwardingHeaders/JavaScriptCore/profiler"
    "${DERIVED_SOURCES_DIR}/ForwardingHeaders/JavaScriptCore/runtime"
    "${DERIVED_SOURCES_DIR}/ForwardingHeaders/JavaScriptCore/yarr"
    "${DERIVED_SOURCES_DIR}/ForwardingHeaders/WTF"
    "${WEBCORE_DIR}/ForwardingHeaders"
    "${WEBCORE_DIR}/platform/cf"
	
    "${DERIVED_SOURCES_DIR}/ForwardingHeaders/JavaScriptCore"
    "${DirectX_INCLUDE_DIRS}"
    "${WEBKIT_LIBRARIES_DIR}/include"
    "${WEBKIT_LIBRARIES_DIR}/include/SQLite"
    "${WEBKIT_LIBRARIES_DIR}/include/zlib"
	"${WEBKIT_LIBRARIES_DIR}/include/harfbuzz"
	"${WEBKIT_LIBRARIES_DIR}/include/libjpeg"
	"${WEBKIT_LIBRARIES_DIR}/include/libpng"
    "${JAVASCRIPTCORE_DIR}/wtf/text"
    "${WEBCORE_DIR}/loader/archive/cf"
    "${WEBCORE_DIR}/platform/cf"
    "${WEBCORE_DIR}/platform/network/curl"
	
	"${WTF_DIR}"
    "${WEBCORE_DIR}/platform/ultralight"
	"${WEBCORE_DIR}/platform/graphics/ultralight"
	"${WEBCORE_DIR}/platform/graphics/harfbuzz"
	
	"${ULTRALIGHTCORE_DIR}/include"
	
	"${SQLITE_DIR}"
)

list(APPEND WebCore_SOURCES
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
	platform/ultralight/MainThreadSharedTimerUltralight.cpp
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
	platform/ultralight/KeyedEncoderUltralight.cpp
	platform/ultralight/KeyedDecoderUltralight.cpp
	platform/ultralight/PreserveSymbolsUltralight.cpp
	platform/ultralight/FileURLLoader.h
	platform/ultralight/FileURLLoader.cpp
	platform/ultralight/DragImageUltralight.cpp
	platform/ultralight/DragDataUltralight.cpp
	platform/ultralight/DragControllerUltralight.cpp
    platform/ultralight/UTFUltralight.cpp

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
	
	platform/image-decoders/ultralight/ImageBackingStoreUltralight.cpp
	
	platform/graphics/harfbuzz/HarfBuzzFace.cpp
	platform/graphics/harfbuzz/HarfBuzzFace.h
	platform/graphics/harfbuzz/HarfBuzzFaceUltralight.cpp
	platform/graphics/harfbuzz/HarfBuzzShaper.cpp
	platform/graphics/harfbuzz/HarfBuzzShaper.h
	
    platform/Cursor.cpp
    platform/KillRingNone.cpp
    platform/LocalizedStrings.cpp
	
	platform/text/LocaleNone.cpp
	
	platform/network/NetworkStorageSessionStub.cpp
	
	platform/network/curl/CookieJarCurl.cpp
    platform/network/curl/CredentialStorageCurl.cpp
    platform/network/curl/CurlCacheEntry.cpp
    platform/network/curl/CurlCacheManager.cpp
    platform/network/curl/CurlDownload.cpp
    platform/network/curl/DNSCurl.cpp
    platform/network/curl/FormDataStreamCurl.cpp
    platform/network/curl/MultipartHandle.cpp
    platform/network/curl/ProxyServerCurl.cpp
    platform/network/curl/ResourceHandleCurl.cpp
    platform/network/curl/ResourceHandleManager.cpp
    platform/network/curl/SSLHandle.cpp
    platform/network/curl/SocketStreamHandleImplCurl.cpp
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
    ${DirectX_LIBRARIES}
    CFLite
	harfbuzz
	freetype
    comctl32
    crypt32
    iphlpapi
    libcurl_a
    jpeg-static
    libpng16
    libxml2_a
	libxslt_a
    rpcrt4
    shlwapi
	ssleay32MD
	libeay32MD
    usp10
    version
    winmm
    ws2_32
	wldap32
	normaliz
    zlibstat
	sqlite3
)

make_directory(${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/WebKit.resources/en.lproj)
file(COPY
    "${WEBCORE_DIR}/English.lproj/Localizable.strings"
    "${WEBCORE_DIR}/English.lproj/mediaControlsLocalizedStrings.js"
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
	add_compile_options(/GL)
	set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /LTCG")
	set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} /LTCG")
endif ()

list(APPEND WebCore_LIBRARIES WTF${DEBUG_SUFFIX})
list(APPEND WebCore_LIBRARIES JavaScriptCore${DEBUG_SUFFIX})
list(APPEND WebCore_LIBRARIES WebCoreDerivedSources)
list(APPEND WebCore_LIBRARIES UltralightCore)
list(APPEND WebCore_LIBRARIES bmalloc)
list(APPEND WebCoreTestSupport_LIBRARIES WTF${DEBUG_SUFFIX})
