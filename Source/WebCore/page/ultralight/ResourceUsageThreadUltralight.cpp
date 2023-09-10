#include "config.h"
#include "ResourceUsageThread.h"

#if ENABLE(RESOURCE_USAGE) && PLATFORM(ULTRALIGHT)

#include <JavaScriptCore/GCActivityCallback.h>
#include <JavaScriptCore/VM.h>

namespace WebCore {

static float cpuUsage()
{
    // FIXME: Need a way to calculate cpu usage
    return 0;
}

void ResourceUsageThread::platformSaveStateBeforeStarting()
{
}

void ResourceUsageThread::platformCollectCPUData(JSC::VM*, ResourceUsageData& data)
{
    data.cpu = cpuUsage();
    data.cpuExcludingDebuggerThreads = data.cpu;
}

void ResourceUsageThread::platformCollectMemoryData(JSC::VM* vm, ResourceUsageData& data)
{
    data.totalDirtySize = 0;

    size_t currentGCHeapCapacity = vm->heap.blockBytesAllocated();
    size_t currentGCOwnedExtra = vm->heap.extraMemorySize();
    size_t currentGCOwnedExternal = vm->heap.externalMemorySize();
    RELEASE_ASSERT(currentGCOwnedExternal <= currentGCOwnedExtra);

    data.categories[MemoryCategory::GCHeap].dirtySize = currentGCHeapCapacity;
    data.categories[MemoryCategory::GCOwned].dirtySize = currentGCOwnedExtra - currentGCOwnedExternal;
    data.categories[MemoryCategory::GCOwned].externalSize = currentGCOwnedExternal;

    data.totalExternalSize = currentGCOwnedExternal;

    data.timeOfNextEdenCollection = data.timestamp + vm->heap.edenActivityCallback()->timeUntilFire().value_or(Seconds(std::numeric_limits<double>::infinity()));
    data.timeOfNextFullCollection = data.timestamp + vm->heap.fullActivityCallback()->timeUntilFire().value_or(Seconds(std::numeric_limits<double>::infinity()));
}

} // namespace WebCore

#endif // ENABLE(RESOURCE_USAGE)
