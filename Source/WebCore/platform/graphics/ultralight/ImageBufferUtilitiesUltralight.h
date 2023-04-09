#pragma once

#if USE(ULTRALIGHT)

#include <Ultralight/private/Image.h>
#include <wtf/Vector.h>
#include <wtf/text/WTFString.h>

namespace WebCore {

Vector<uint8_t> encodeData(ultralight::Image*, const String& mimeType, std::optional<double> quality);

} // namespace WebCore

#endif // USE(ULTRALIGHT)
