#include "config.h"
#include "NativeImage.h"
#include "IntSize.h"
#include "Color.h"
#include "GraphicsContext.h"
#include "graphics/ultralight/PlatformContextUltralight.h"

namespace WebCore {

  IntSize nativeImageSize(const NativeImagePtr& image)
  {
    if (!image)
      return IntSize();

    return IntSize(image->bitmap()->width(), image->bitmap()->height());
  }

  bool nativeImageHasAlpha(const NativeImagePtr& image)
  {
    // all image formats have alpha
    return true;
  }

  Color nativeImageSinglePixelSolidColor(const NativeImagePtr& image)
  {
    // AFAIK, this is only used as an optimization for drawing bitmaps
    // that are only a single pixel to avoid texture sampling. We don't
    // really care in our port since we always sample texture anyways.
    return Color();
  }

  float subsamplingScale(GraphicsContext&, const FloatRect&, const FloatRect&)
  {
    return 1;
  }

  void drawNativeImage(const NativeImagePtr& image, GraphicsContext& context, const FloatRect& destRect, const FloatRect& srcRect, const IntSize& imageSize, const ImagePaintingOptions& options)
  {
      context.drawNativeImage(image, imageSize, destRect, srcRect, options);
  }

  void clearNativeImageSubimages(const NativeImagePtr& image)
  {
    // FIXME, handle this
  }

} // namespace WebCore
