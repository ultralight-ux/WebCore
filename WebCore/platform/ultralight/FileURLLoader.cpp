#include "config.h"
#include "FileURLLoader.h"

#include "SharedBuffer.h"
#include "URL.h"
#include <wtf/MainThread.h>
#include <wtf/WorkQueue.h>
#include <Ultralight/Platform/Platform.h>
#include <Ultralight/Platform/FileSystem.h>
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

  return std::make_unique<LoadTask>(LoadTask{
    path.isolatedCopy(),
    WTFMove(completionHandler)
  });
}

void load(const URL& url, LoadCompletionHandler&& completionHandler) {
  loadQueue().dispatch([loadTask = createLoadTask(url, WTFMove(completionHandler))]() mutable {
    auto fs = ultralight::Platform::instance().file_system();
    if (fs) {
      ultralight::String16 path = ultralight::Convert(loadTask->urlString);
      ultralight::FileHandle file = fs->OpenFile(path, false);
      if (file != ultralight::invalidFileHandle) {
        int64_t fileSize = 0;
        if (fs->GetFileSize(file, fileSize) && fileSize > 0) {
          char* buffer = new char[fileSize];
          if (fs->ReadFromFile(file, buffer, fileSize) > 0) {
            loadTask->result.data = SharedBuffer::create(buffer, fileSize);
            ultralight::String16 mimeType = "application/unknown";
            fs->GetFileMimeType(path, mimeType);
            loadTask->result.mimeType = ultralight::Convert(mimeType);

          }
        }
        fs->CloseFile(file);
      }
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
