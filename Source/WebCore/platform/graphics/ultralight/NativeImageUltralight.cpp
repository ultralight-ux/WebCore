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

    uint32_t width, height;
    if (!image->first->GetFrameSize(image->second, width, height))
      return IntSize();


    return IntSize(width, height);
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

  void drawNativeImage(const NativeImagePtr& image, GraphicsContext& context, const FloatRect& destRect, const FloatRect& srcRect, const IntSize&, CompositeOperator op, BlendMode mode, const ImageOrientation& orientation)
  {
    if (!image)
      return;

    context.save();

    // Set the compositing operation.
    if (op == CompositeSourceOver && mode == BlendModeNormal && !nativeImageHasAlpha(image))
      context.setCompositeOperation(CompositeCopy);
    else
      context.setCompositeOperation(op, mode);

#if ENABLE(IMAGE_DECODER_DOWN_SAMPLING)
    IntSize scaledSize = nativeImageSize(image);
    FloatRect adjustedSrcRect = adjustSourceRectForDownSampling(srcRect, scaledSize);
#else
    FloatRect adjustedSrcRect(srcRect);
#endif

    FloatRect adjustedDestRect = destRect;

    if (orientation != DefaultImageOrientation) {
      // ImageOrientation expects the origin to be at (0, 0).
      context.translate(destRect.x(), destRect.y());
      adjustedDestRect.setLocation(FloatPoint());
      context.concatCTM(orientation.transformFromDefault(adjustedDestRect.size()));
      if (orientation.usesWidthAsHeight()) {
        // The destination rectangle will have it's width and height already reversed for the orientation of
        // the image, as it was needed for page layout, so we need to reverse it back here.
        adjustedDestRect.setSize(adjustedDestRect.size().transposedSize());
      }
    }

    ultralight::Paint paint;
    paint.color = UltralightColorWHITE;
    context.platformContext()->canvas()->DrawImage(image->first, image->second, adjustedSrcRect,
      adjustedDestRect, paint);
    context.restore();
  }

  void clearNativeImageSubimages(const NativeImagePtr& image)
  {
    if (image)
      image->first->ClearFrame(image->second);
  }

} // namespace WebCore
