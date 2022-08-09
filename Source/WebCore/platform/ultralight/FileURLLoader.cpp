#include "config.h"
#include "FileURLLoader.h"

#include "SharedBuffer.h"
#include "StringUltralight.h"
#include <Ultralight/Buffer.h>
#include <Ultralight/platform/FileSystem.h>
#include <Ultralight/platform/Platform.h>
#include <Ultralight/private/util/Debug.h>
#include <vector>
#include <wtf/MainThread.h>
#include <wtf/MemoryProfiler.h>
#include <wtf/URL.h>
#include <wtf/WorkQueue.h>
#include <wtf/Shutdown.h>

namespace WebCore {
namespace FileURLLoader {

    static WTF::RefPtr<WorkQueue> g_loadQueue;

    static WorkQueue& loadQueue()
    {
        if (!g_loadQueue)
        {
            g_loadQueue = WorkQueue::create("org.ultralight.FileURLLoader");
            WTF::CallOnShutdown([]() mutable {
                g_loadQueue = nullptr;
            });
        }
        
        return *g_loadQueue.get();
    }

    struct LoadTask {
        WTF_MAKE_FAST_ALLOCATED;

    public:
        LoadTask(const String& urlString, LoadCompletionHandler&& completionHandler)
            : urlString(urlString.isolatedCopy())
            , completionHandler(WTFMove(completionHandler))
        {
        }

        const String urlString;
        const LoadCompletionHandler completionHandler;

        Result result;
    };

    static std::unique_ptr<LoadTask> createLoadTask(const URL& url, LoadCompletionHandler&& completionHandler)
    {
        WTF::String path = url.fileSystemPath();
        if (!path.isEmpty()) {
            if (path[0] == '/')
                path = path.substring(1);
        }

        return std::make_unique<LoadTask>(path, WTFMove(completionHandler));
    }

    void load(const URL& url, LoadCompletionHandler&& completionHandler)
    {
        loadQueue().dispatch([loadTask = createLoadTask(url, WTFMove(completionHandler))]() mutable {
            ProfiledMemoryZone(MemoryTag::Resource);
            auto fs = ultralight::Platform::instance().file_system();
            if (fs) {
                ultralight::String path = ultralight::Convert(loadTask->urlString);
                if (fs->FileExists(path)) {
                    auto mimeType = fs->GetFileMimeType(path);
                    auto charset = fs->GetFileCharset(path);
                    ultralight::RefPtr<ultralight::Buffer> buffer = fs->OpenFile(path);
                    if (buffer) {
                        loadTask->result.data = SharedBuffer::create(WTFMove(buffer));
                        loadTask->result.mimeType = ultralight::Convert(mimeType);
                        loadTask->result.charset = ultralight::Convert(charset);
                    }
                }
            } else {
                UL_LOG_ERROR("Error, NULL FileSystem encountered while attempting to load a file:/// URL. Did you forget to call Platform::set_file_system()?");
            }

            callOnMainThread([loadTask = WTFMove(loadTask)] {
                if (!loadTask->result.data) {
                    loadTask->completionHandler({});
                    return;
                }

                loadTask->completionHandler(WTFMove(loadTask->result));
            });
        });
    }

} // namespace FileURLLoader
} // namespace WebCore
