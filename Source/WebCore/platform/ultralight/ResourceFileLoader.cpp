#include "config.h"
#include "ResourceFileLoader.h"
#include "StringUltralight.h"
#include <Ultralight/platform/Config.h>
#include <Ultralight/platform/Platform.h>
#include <Ultralight/private/util/Debug.h>

namespace WebCore {
namespace ResourceFileLoader {

    ultralight::RefPtr<ultralight::Buffer> openFile(const String& filePath)
    {
        ultralight::FileSystem* fs = ultralight::Platform::instance().file_system();
        auto config = ultralight::Platform::instance().config();
        ultralight::String filePathStr = config.resource_path_prefix + ultralight::Convert(filePath);

        if (!fs) {
            ultralight::String err_msg = "Could not load resource: " + filePathStr + ", no FileSystem instance set, make sure that you've called ultralight::Platform::instance().set_file_system().";
            UL_LOG_ERROR(err_msg);
            return nullptr;
        }

        if (!fs->FileExists(filePathStr)) {
            ultralight::String err_msg = "Could not load resource: " + filePathStr + ", FileSystem::FileExists() returned false.";
            UL_LOG_ERROR(err_msg);
            return nullptr;
        }

        ultralight::RefPtr<ultralight::Buffer> result = fs->OpenFile(filePathStr);

        if (!result) {
            ultralight::String err_msg = "Could not load resource: " + filePathStr + ", FileSystem::OpenFile() returned a nullptr.";
            UL_LOG_ERROR(err_msg);
            return nullptr;
        }

        return result;
    }

    String readFileToString(const String& filePath)
    {
        ultralight::FileSystem* fs = ultralight::Platform::instance().file_system();
        if (!fs)
            return String();

        ultralight::RefPtr<ultralight::Buffer> fileData = openFile(filePath);
        if (!fileData)
            return String();

        size_t fileSize = fileData->size();

        if (fileSize == 0)
            return String();

        return String(static_cast<char*>(fileData->data()), fileSize);
    }

} // namespace ResourceFileLoader
} // namespace WebCore
