#include "config.h"

#if ENABLE(WEB_AUDIO)

#include "AudioBus.h"

#include "AudioFileReader.h"


namespace WebCore {

RefPtr<AudioBus> AudioBus::loadPlatformResource(const char* name, float sampleRate)
{
    return nullptr;
}

} // namespace WebCore

#endif // ENABLE(WEB_AUDIO)
