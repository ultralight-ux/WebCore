#include "config.h"
#include "FileURLLoader.h"

#include "SharedBuffer.h"
#include <wtf/URL.h>
#include <wtf/MainThread.h>
#include <wtf/WorkQueue.h>
#include <Ultralight/platform/Platform.h>
#include <Ultralight/platform/FileSystem.h>
#include <Ultralight/private/util/Debug.h>
#include "StringUltralight.h"

namespace WebCore {
namespace FileURLLoader {

static WorkQueue& loadQueue()
{
  static auto& queue = WorkQueue::create("org.ultralight.FileURLLoader").leakRef();
  return queue;
}

struct LoadTask {
  WTF_MAKE_FAST_ALLOCATED;
public:
  LoadTask(const String& urlString, LoadCompletionHandler&& completionHandler)
    : urlString(urlString.isolatedCopy()), completionHandler(WTFMove(completionHandler))
  {
  }

  const String urlString;
  const LoadCompletionHandler completionHandler;

  Result result;
};

static std::unique_ptr<LoadTask> createLoadTask(const URL& url, LoadCompletionHandler&& completionHandler) {
  WTF::String path = url.fileSystemPath();
  if (!path.isEmpty()) {
    if (path[0] == '/')
      path = path.substring(1);
  }

  return std::make_unique<LoadTask>(path, WTFMove(completionHandler));
}

void load(const URL& url, LoadCompletionHandler&& completionHandler) {
  loadQueue().dispatch([loadTask = createLoadTask(url, WTFMove(completionHandler))]() mutable {
    auto fs = ultralight::Platform::instance().file_system();
    if (fs) {
      ultralight::String path = ultralight::Convert(loadTask->urlString);
      ultralight::FileHandle file = fs->OpenFile(path, false);
      if (file != ultralight::invalidFileHandle) {
        int64_t fileSize = 0;
        if (fs->GetFileSize(file, fileSize) && fileSize > 0) {
          char* buffer = new char[fileSize];
          if (fs->ReadFromFile(file, buffer, fileSize) > 0) {
            loadTask->result.data = SharedBuffer::create(buffer, fileSize);
            ultralight::String mimeType = "application/unknown";
            fs->GetFileMimeType(path, mimeType);
            loadTask->result.mimeType = ultralight::Convert(mimeType);
            ultralight::String charset = "utf-8";
            loadTask->result.charset = ultralight::Convert(charset);
          }
        }
        fs->CloseFile(file);
      }
    }
    else {
      UL_LOG_ERROR("Error, NULL FileSystem encountered while attempting to load a file:/// URL. Did you forget to call Platform::set_file_system()?");
    }

    callOnMainThread([loadTask = WTFMove(loadTask)]{
      if (!loadTask->result.data) {
        loadTask->completionHandler({});
        return;
      }

      loadTask->completionHandler(WTFMove(loadTask->result));
    });
  });
}

}  // namespace FileURLLoader
}  // namespace WebCore
