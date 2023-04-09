#include "config.h"
#include "CurlSSLHandle.h"
#include "ResourceFileLoader.h"
#include "StringUltralight.h"
#include <Ultralight/platform/Config.h>
#include <Ultralight/platform/Logger.h>
#include <Ultralight/platform/Platform.h>
#include <Ultralight/private/util/Debug.h>
#include <wtf/FileSystem.h>
#include <wtf/MemoryProfiler.h>

namespace WebCore {

void CurlSSLHandle::platformInitialize()
{
    ProfiledMemoryZone(MemoryTag::Resource);
    auto& platform = ultralight::Platform::instance();
    auto config = platform.config();
    ultralight::RefPtr<ultralight::Buffer> certData = ResourceFileLoader::openFile("cacert.pem"_s);
    size_t fileSize = 0;
    CertificateInfo::Certificate buffer;

    if (!certData)
        goto FAIL_LOAD;

    fileSize = certData->size();
    if (fileSize == 0)
        goto FAIL_LOAD;

    buffer.append(static_cast<char*>(certData->data()), fileSize);

    setCACertData(std::move(buffer));

    return;

FAIL_LOAD:
    UL_LOG_ERROR("Failed to load cacert.pem (SSL certificate chain), the library will be unable to make SSL/HTTPS requests.")
    return;
}

}
