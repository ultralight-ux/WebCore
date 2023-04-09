#include "config.h"

#if USE(ULTRALIGHT)

namespace WebCore {

Color::Color(const ultralight::Color& color)
    : Color(SRGBA<uint8_t>(UltralightColorGetR(color), UltralightColorGetG(color), UltralightColorGetB(color), UltralightColorGetA(color)))
{
}

Color::operator ultralight::Color() const {
    auto srgba = toColorTypeLossy<SRGBA<uint8_t>>().resolved();
    return UltralightRGBA(srgba.red, srgba.green, srgba.blue, srgba.alpha);
}

} // namespace WebCore

#endif