#include "config.h"
#include "MemoryUtils.h"
#include "ResourceUsageThread.h"
#include <wtf/MemoryPressureHandler.h>
#include <Ultralight/private/util/Debug.h>
#include <sstream>

namespace WebCore {

static ResourceUsageData gData;

static const char* fmtBytes(uint64_t bytes) {
  char* suffix[] = { "B", "KB", "MB", "GB", "TB" };
  char length = sizeof(suffix) / sizeof(suffix[0]);

  int i = 0;
  double dblBytes = (double)bytes;

  if (bytes > 1024) {
    for (i = 0; (bytes / 1024) > 0 && i < length - 1; i++, bytes /= 1024)
      dblBytes = bytes / 1024.0;
  }

  static char output[200];
  sprintf(output, "%.02lf %s", dblBytes, suffix[i]);
  return output;
}

MemoryUtils::MemoryUtils() {
  ResourceUsageThread::addObserver(this, Memory, [this](const ResourceUsageData& data) {
    gData = data;
  });
}

MemoryUtils::~MemoryUtils() {
  ResourceUsageThread::removeObserver(this);
}

#define PRINT_STATS(str, obj) \
  stream << str << fmtBytes(gData.categories[MemoryCategory::obj].totalSize()) << std::endl;

void MemoryUtils::logMemoryStatistics() {
  std::ostringstream stream;
  stream << "Memory Usage (WebCore): " << std::endl;
  PRINT_STATS("    JavaScript GC Heap:  ", GCHeap);
  PRINT_STATS("    JavaScript GC Owned: ", GCOwned);
  PRINT_STATS("    JavaScript JIT:      ", JSJIT);
  PRINT_STATS("    Images:              ", Images);
  PRINT_STATS("    Layers:              ", Layers);
  stream <<   "    Page:                " << 
    fmtBytes(gData.categories[MemoryCategory::bmalloc].totalSize() + 
      gData.categories[MemoryCategory::LibcMalloc].totalSize()) << std::endl;
  PRINT_STATS("    Other:               ", Other);
  UL_LOG_INFO(stream.str().c_str());
}

void MemoryUtils::beginSimulatedMemoryPressure() {
  WTF::MemoryPressureHandler::singleton().beginSimulatedMemoryPressure();
}

void MemoryUtils::endSimulatedMemoryPressure() {
  WTF::MemoryPressureHandler::singleton().endSimulatedMemoryPressure();
}

}  // namespace WebCore
