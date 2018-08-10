#pragma once

#include "GraphicsContext.h"

#if USE(ULTRALIGHT)

#include "PlatformContextUltralight.h"
#include "RefPtrUltralight.h"

namespace WebCore {

class GraphicsContextPlatformPrivate {
public:
  GraphicsContextPlatformPrivate(PlatformContextUltralight* context) : platformContext(context)
  {
  }

  virtual ~GraphicsContextPlatformPrivate()
  {
  }

  PlatformContextUltralight* platformContext;
};

// This is a specialized private section for the Ultralight GraphicsContext, which knows how
// to clean up the heap allocated PlatformContextUltralight that we must use for the top-level
// GraphicsContext.
class GraphicsContextPlatformPrivateTopLevel : public GraphicsContextPlatformPrivate {
public:
  GraphicsContextPlatformPrivateTopLevel(PlatformContextUltralight* context) : GraphicsContextPlatformPrivate(context)
  {
  }

  virtual ~GraphicsContextPlatformPrivateTopLevel()
  {
    delete platformContext;
  }
};

} // namespace WebCore

#endif // USE(ULTRALIGHT)
