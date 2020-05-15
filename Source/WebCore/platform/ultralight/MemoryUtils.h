#pragma once

namespace WebCore {

class WEBCORE_EXPORT MemoryUtils {
public:
  MemoryUtils();
  ~MemoryUtils();

  void logMemoryStatistics();

  void beginSimulatedMemoryPressure();

  void endSimulatedMemoryPressure();
};

}  // namespace WebCore
