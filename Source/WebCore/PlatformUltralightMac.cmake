add_definitions(-D__STDC_CONSTANT_MACROS -DSTATICALLY_LINKED_WITH_WTF
                -DSTATICALLY_LINKED_WITH_JavaScriptCore -DSTATICALLY_LINKED_WITH_PAL -DULTRALIGHT_IMPLEMENTATION
                -DCURL_STATICLIB)

include(platform/ImageDecoders.cmake)
include(platform/TextureMapper.cmake)
include(platform/Curl.cmake)

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
    editing/SmartReplaceCF.cpp
    page/cocoa/ResourceUsageThreadCocoa.mm
    platform/cf/KeyedDecoderCF.cpp
    platform/cf/KeyedEncoderCF.cpp
    platform/cf/SharedBufferCF.cpp
    platform/Cursor.cpp
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
    platform/graphics/ultralight/ImageBufferUltralight.cpp
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
    platform/network/mac/NetworkStateNotifierMac.cpp
    platform/network/ultralight/CurlSSLHandleUltralight.cpp
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
    platform/ultralight/SSLKeyGeneratorUltralightMac.mm
    platform/ultralight/SynchronousLoaderClientUltralight.cpp
    platform/ultralight/TextBreakIteratorInternalICUUltralight.cpp
    platform/ultralight/UTFUltralight.cpp
    platform/ultralight/WidgetUltralight.cpp
)

list(APPEND WebCore_USER_AGENT_STYLE_SHEETS
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

set_source_files_properties("platform/ultralight/SSLKeyGeneratorUltralightMac.mm" PROPERTIES COMPILE_FLAGS "-x objective-c++")

find_library(SECURITY_LIBRARY Security) # used in SSLKeyGeneratorUltralightMac
find_library(ACCELERATE Accelerate)
find_library(APPKIT AppKit) # used for NSBeep in PAL
find_library(SYSCONFIG SystemConfiguration) # used in NetworkStateNotifierMac

list(APPEND WebCore_LIBRARIES
    brotlicommon
    brotlidec
    brotlienc
    bz2
    crypto
    curl
    freetype
    harfbuzz
    harfbuzz-icu
    jpeg
    nghttp2_static
    png16
    sqlite3
    ssl
    tls
    xml2
    xslt
    z
    ${ACCELERATE}
    ${APPKIT}
    ${SYSCONFIG}
    ${SECURITY_LIBRARY}
)

file(MAKE_DIRECTORY ${DERIVED_SOURCES_DIR}/ForwardingHeaders/WebCore)

set(BUILD_TMP "${CMAKE_BINARY_DIR}/tmp/")
file(MAKE_DIRECTORY "${BUILD_TMP}")

set(WebCore_DERIVED_SOURCES_PRE_BUILD_COMMAND "${CMAKE_BINARY_DIR}/DerivedSources/WebCore/preBuild.sh")
set(WebCore_POST_BUILD_COMMAND "${CMAKE_BINARY_DIR}/DerivedSources/WebCore/postBuild.sh")

# Write the pre-build bash script
file(WRITE "${BUILD_TMP}/preBuild.sh" "#!/bin/bash\ncp -R ${WEBCORE_DIR}/ForwardingHeaders/ ${DERIVED_SOURCES_DIR}/ForwardingHeaders/WebCore 2>/dev/null\n")
foreach (_directory ${WebCore_FORWARDING_HEADERS_DIRECTORIES})
file(APPEND "${BUILD_TMP}/preBuild.sh" "rsync -aqW ${WEBCORE_DIR}/${_directory}/*.h ${DERIVED_SOURCES_DIR}/ForwardingHeaders/WebCore 2>/dev/null\n")
endforeach ()

# Write the post-build bash script
file(WRITE "${BUILD_TMP}/postBuild.sh" "#!/bin/bash\nrsync -aqW ${DERIVED_SOURCES_WEBCORE_DIR}/*.h ${DERIVED_SOURCES_DIR}/ForwardingHeaders/WebCore 2>/dev/null\n")

# Copy bash scripts over and chmod to executable
file (COPY "${BUILD_TMP}/preBuild.sh" DESTINATION "${CMAKE_BINARY_DIR}/DerivedSources/WebCore" FILE_PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ)
file (COPY "${BUILD_TMP}/postBuild.sh" DESTINATION "${CMAKE_BINARY_DIR}/DerivedSources/WebCore" FILE_PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ)

# Remove temp directory
file(REMOVE_RECURSE "${BUILD_TMP}")

list(APPEND WebCore_LIBRARIES WTF)
list(APPEND WebCore_LIBRARIES JavaScriptCore)
list(APPEND WebCore_LIBRARIES UltralightCore)
list(APPEND WebCoreTestSupport_LIBRARIES WTF)
