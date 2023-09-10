#include "config.h"
#include "ResourceUsageOverlay.h"

#if ENABLE(RESOURCE_USAGE) && PLATFORM(ULTRALIGHT)

namespace WebCore {

void ResourceUsageOverlay::platformInitialize()
{
}

void ResourceUsageOverlay::platformDestroy()
{
}

} // namespace WebCore

#endif // ENABLE(RESOURCE_USAGE)
