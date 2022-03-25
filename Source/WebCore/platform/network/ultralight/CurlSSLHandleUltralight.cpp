#include "config.h"
#include "CurlSSLHandle.h"
#include "ResourceLoaderUltralight.h"
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
    ultralight::FileHandle handle = ResourceLoader::openFile("cacert.pem");
    ultralight::FileSystem* fs = ultralight::Platform::instance().file_system();
    int64_t fileSize = 0;
    CertificateInfo::Certificate buffer;

    if (handle == ultralight::invalidFileHandle)
        goto FAIL_LOAD;

    if (!fs)
        goto FAIL_LOAD;

    if (!fs->GetFileSize(handle, fileSize) || fileSize == 0)
        goto FAIL_LOAD;

    buffer.resize(fileSize);
    if (fs->ReadFromFile(handle, (char*)buffer.data(), fileSize) != fileSize)
        goto FAIL_LOAD;

    setCACertData(std::move(buffer));

    fs->CloseFile(handle);
    return;

FAIL_LOAD:
    if (fs && handle != ultralight::invalidFileHandle)
        fs->CloseFile(handle);
    UL_LOG_ERROR("Failed to load cacert.pem (SSL certificate chain), the library will be unable to make SSL/HTTPS requests.")
    return;
}

}
