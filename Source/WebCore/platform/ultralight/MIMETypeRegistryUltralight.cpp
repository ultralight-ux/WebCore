#include "config.h"
#include "MIMETypeRegistry.h"
#include "NotImplemented.h"
#include <wtf/HashMap.h>

namespace WebCore {


static const std::initializer_list<TypeExtensionPair>& platformMediaTypes()
{
    static std::initializer_list<TypeExtensionPair> platformMediaTypes = {
        { "image/bmp"_s, "bmp"_s },
        { "text/css"_s, "css"_s },
        { "image/ico"_s, "cur"_s },
        { "image/gif"_s, "gif"_s },
        { "text/html"_s, "htm"_s },
        { "text/html"_s, "html"_s },
        { "image/ico"_s, "ico"_s },
        { "image/jpeg"_s, "jpeg"_s },
        { "image/jpeg"_s, "jpg"_s },
        { "application/x-javascript"_s, "js"_s },
        { "audio/x-m4a"_s, "m4a"_s },
        { "application/pdf"_s, "pdf"_s },
        { "image/png"_s, "png"_s },
        { "application/rss+xml"_s, "rss"_s },
        { "image/svg+xml"_s, "svg"_s },
        { "image/svg+xml"_s, "svgz"_s },
        { "application/x-shockwave-flash"_s, "swf"_s },
        { "text/plain"_s, "text"_s },
        { "text/plain"_s, "txt"_s },
        { "application/x-webarchive"_s, "webarchive"_s },
        { "text/vnd.wap.wml"_s, "wml"_s },
        { "application/vnd.wap.wmlc"_s, "wmlc"_s },
        { "image/x-xbitmap"_s, "xbm"_s },
        { "application/xhtml+xml"_s, "xhtml"_s },
        { "text/xml"_s, "xml"_s },
        { "text/xsl"_s, "xsl"_s },
    };
    return platformMediaTypes;
}

String MIMETypeRegistry::mimeTypeForExtension(StringView extension)
{
    for (auto& entry : platformMediaTypes()) {
        if (equalIgnoringASCIICase(extension, entry.extension))
            return entry.type;
    }
    return emptyString();
}

bool MIMETypeRegistry::isApplicationPluginMIMEType(const String&)
{
    return false;
}

String MIMETypeRegistry::preferredExtensionForMIMEType(const String& mimeType)
{
    for (auto& entry : platformMediaTypes()) {
        if (equalIgnoringASCIICase(mimeType, entry.type))
            return entry.extension;
    }
    return emptyString();
}

Vector<String> MIMETypeRegistry::extensionsForMIMEType(const String&)
{
    ASSERT_NOT_IMPLEMENTED_YET();
    return { };
}

}