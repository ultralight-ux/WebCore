#include "config.h"
#include "ImageBackingStore.h"
#include <Ultralight/Bitmap.h>
#include <Ultralight/private/Image.h>
#include <ultralight/private/Paint.h>

namespace WebCore {

NativeImagePtr ImageBackingStore::image(bool is_complete) const
{
  if (!m_nativeImage) {
    const_cast<ImageBackingStore*>(this)->m_nativeImage = std::make_shared<FramePair>(ultralight::Image::Create(), 0);
  }

  uint32_t width = size().width();
  uint32_t height = size().height();
  uint32_t bpp = sizeof(RGBA32);
  static_assert(sizeof(RGBA32) == 4, "We expect sizeof(RGBA32) to be 4 on all platforms.");
  ultralight::Ref<ultralight::Bitmap> bitmap = ultralight::Bitmap::Create(width, height, ultralight::kBitmapFormat_BGRA8_UNORM_SRGB,
    width * bpp, (const void*)m_pixelsPtr, width * height * bpp, true, true);

  //
  // We convert sRGB post-gamma premultiplication to sRGB pre-gamma premultiplication
  //
  // PNGs with alpha are decoded in sRGB color-space (actually 2.2 gamma, which
  // is a very close approximation of sRGB gamma) with alpha multiplied with
  // RGB channels AFTER the gamma function.
  //
  // To ensure our rendering pipeline is gamma-correct, we do all blending in
  // linear color-space with all alpha values pre-multiplied linearly. This
  // means we need to convert from sRGB POST-gamma premultiplied-alpha encoding
  // to sRGB PRE-gamma premultiplied-alpha encoding to use the sRGB conversion
  // hardware in GPU.
  //
  // See: <http://www.realtimerendering.com/blog/a-png-puzzle/>
  //

  m_nativeImage->first->SetFrame(0, 1, bitmap, is_complete);

  return m_nativeImage;
}

} // namespace WebCore
