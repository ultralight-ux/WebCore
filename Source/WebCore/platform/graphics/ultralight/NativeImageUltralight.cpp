#include "config.h"
#include "Color.h"
#include "GraphicsContext.h"
#include "IntSize.h"
#include "NativeImage.h"

namespace WebCore {

IntSize NativeImage::size() const
{
    return IntSize(m_platformImage->bitmap()->width(), m_platformImage->bitmap()->height());
}

bool NativeImage::hasAlpha() const
{
    // all image formats have alpha
    return true;
}

Color NativeImage::singlePixelSolidColor() const
{
    // AFAIK, this is only used as an optimization for drawing bitmaps
    // that are only a single pixel to avoid texture sampling. We don't
    // really care in our port since we always sample texture anyways.
    return Color();
}

DestinationColorSpace NativeImage::colorSpace() const
{
    return DestinationColorSpace::SRGB();
}

void NativeImage::clearSubimages()
{
    // FIXME, handle this
}

} // namespace WebCore
