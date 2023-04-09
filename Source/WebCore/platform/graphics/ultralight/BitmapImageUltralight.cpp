#include "config.h"
#include "BitmapImage.h"
#include "NotImplemented.h"
#include "platform/graphics/GraphicsContext.h"
#include "ImageObserver.h"

namespace WebCore {

  /*
  void BitmapImage::draw(GraphicsContext& context, const FloatRect& dest, const FloatRect& src, CompositeOperator, BlendMode, ImageOrientationDescription) {
    PlatformContextUltralight* platformContext = context.platformContext();
    PlatformCanvas canvas = platformContext->canvas();
    ultralight::Paint paint;
    paint.color = UltralightColorWHITE;

    // We call this to ensure the frame is cached
    NativeImagePtr image = nativeImageForCurrentFrame();

    ultralight::Surface* surface = canvas->top_surface();
    surface->DrawImage(image->first, image->second, src, dest, paint);

    if (imageObserver())
      imageObserver()->didDraw(this);
  }
  */

  /*
  bool FrameData::clear(bool clearMetadata)
  {
    if (clearMetadata)
      m_haveMetadata = false;

    if (m_image) {
      m_image = nullptr;
      return true;
    }
    return false;
  }
  */

} // namespace WebCore
