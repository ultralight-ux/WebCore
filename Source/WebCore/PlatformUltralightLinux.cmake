add_definitions(-D__STDC_CONSTANT_MACROS -DSTATICALLY_LINKED_WITH_WTF
                -DSTATICALLY_LINKED_WITH_JavaScriptCore -DULTRALIGHT_IMPLEMENTATION
                -DCURL_STATICLIB)

include(platform/ImageDecoders.cmake)
include(platform/TextureMapper.cmake)
include(platform/Curl.cmake)
include(platform/Ultralight.cmake)

if (USE_GSTREAMER)
    include(platform/GStreamer.cmake)
endif ()

list(APPEND WebCore_PRIVATE_INCLUDE_DIRECTORIES
    "${PROJECT_BINARY_DIR}/../include/private"
    "${PROJECT_BINARY_DIR}/../include/private/JavaScriptCore"
    "${WEBCORE_DIR}/platform/graphics/egl"
    "${WEBCORE_DIR}/platform/graphics/opengl"
    "${WEBCORE_DIR}/platform/graphics/opentype"
    "${WEBCORE_DIR}/platform/mediacapabilities"
)

list(APPEND WebCore_INCLUDE_DIRECTORIES
    "${DERIVED_SOURCES_DIR}/ForwardingHeaders"
)

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
    page/ultralight/ResourceUsageThreadUltralight.cpp

    platform/Cursor.cpp

    platform/generic/KeyedDecoderGeneric.cpp
    platform/generic/KeyedEncoderGeneric.cpp

    platform/graphics/harfbuzz/ComplexTextControllerHarfBuzz.cpp
    platform/graphics/harfbuzz/HarfBuzzFaceUltralight.cpp
    
    platform/LocalizedStrings.cpp
    
    platform/ScrollAnimationKinetic.cpp
    platform/ScrollAnimationSmooth.cpp
    platform/text/LocaleNone.cpp
    
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

include_directories(
)

list(APPEND WebCore_PRIVATE_LIBRARIES
    "${WEBKIT_LIBRARIES_DIR}/lib/libcurl.a"
    "${WEBKIT_LIBRARIES_DIR}/lib/libbrotlidec.a"
    "${WEBKIT_LIBRARIES_DIR}/lib/libbrotlienc.a"
    "${WEBKIT_LIBRARIES_DIR}/lib/libbrotlicommon.a"
    "${WEBKIT_LIBRARIES_DIR}/lib/libharfbuzz-icu.a"
    "${WEBKIT_LIBRARIES_DIR}/lib/libharfbuzz.a"
    "${WEBKIT_LIBRARIES_DIR}/lib/libfreetype.a"
    "${WEBKIT_LIBRARIES_DIR}/lib/libjpeg.a"
    "${WEBKIT_LIBRARIES_DIR}/lib/libnghttp2_static.a"
    "${WEBKIT_LIBRARIES_DIR}/lib/libpng16.a"
    "${WEBKIT_LIBRARIES_DIR}/lib/libssl.a"
    "${WEBKIT_LIBRARIES_DIR}/lib/libtls.a"
    "${WEBKIT_LIBRARIES_DIR}/lib/libcrypto.a"
    "${WEBKIT_LIBRARIES_DIR}/lib/libxml2.a"
    "${WEBKIT_LIBRARIES_DIR}/lib/libxslt.a"
    "${WEBKIT_LIBRARIES_DIR}/lib/libz.a"
    sqlite3
    # gcrypt
    rt  # needed on Ubuntu for clock_gettime
)

if (USE_GSTREAMER)
    list(APPEND WebCore_PRIVATE_LIBRARIES
        gstreamer-full-1.0
    )
endif ()

message(STATUS "Freetype include ${FREETYPE_INCLUDE_DIRS}")

file(MAKE_DIRECTORY ${DERIVED_SOURCES_DIR}/ForwardingHeaders/WebCore)

set(BUILD_TMP "${PROJECT_BINARY_DIR}/tmp/")
file(MAKE_DIRECTORY "${BUILD_TMP}")

set(WebCore_DERIVED_SOURCES_PRE_BUILD_COMMAND "${PROJECT_BINARY_DIR}/DerivedSources/WebCore/preBuild.sh")
set(WebCore_POST_BUILD_COMMAND "${PROJECT_BINARY_DIR}/DerivedSources/WebCore/postBuild.sh")

# Write the pre-build bash script
file(WRITE "${BUILD_TMP}/preBuild.sh" "#!/bin/bash\ncp -a ${WEBCORE_DIR}/ForwardingHeaders/* ${DERIVED_SOURCES_DIR}/ForwardingHeaders/WebCore 2>/dev/null\n")
foreach (_directory ${WebCore_FORWARDING_HEADERS_DIRECTORIES})
file(APPEND "${BUILD_TMP}/preBuild.sh" "rsync -aqW ${WEBCORE_DIR}/${_directory}/*.h ${DERIVED_SOURCES_DIR}/ForwardingHeaders/WebCore 2>/dev/null\n")
endforeach ()

# Write the post-build bash script
file(WRITE "${BUILD_TMP}/postBuild.sh" "#!/bin/bash\nrsync -aqW ${WebCore_DERIVED_SOURCES_DIR}/*.h ${DERIVED_SOURCES_DIR}/ForwardingHeaders/WebCore 2>/dev/null\n")

# Copy bash scripts over and chmod to executable
file (COPY "${BUILD_TMP}/preBuild.sh" DESTINATION "${PROJECT_BINARY_DIR}/DerivedSources/WebCore" FILE_PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ)
file (COPY "${BUILD_TMP}/postBuild.sh" DESTINATION "${PROJECT_BINARY_DIR}/DerivedSources/WebCore" FILE_PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ)

# Remove temp directory
file(REMOVE_RECURSE "${BUILD_TMP}")

list(APPEND WebCore_PRIVATE_LIBRARIES WTF)
list(APPEND WebCore_PRIVATE_LIBRARIES JavaScriptCore)
#list(APPEND WebCore_LIBRARIES UltralightCore)
list(APPEND WebCoreTestSupport_LIBRARIES WTF)
