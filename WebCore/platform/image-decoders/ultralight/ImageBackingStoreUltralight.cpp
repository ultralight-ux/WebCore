#include "config.h"
#include "ImageBackingStore.h"
#include <Ultralight/Bitmap.h>
#include <Ultralight/private/Image.h>

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
  ultralight::Ref<ultralight::Bitmap> bitmap = ultralight::Bitmap::Create(width, height, ultralight::kBitmapFormat_RGBA8,
    width * bpp, (const void*)m_pixelsPtr, width * height * bpp, true);

  //bitmap->WritePNG("test.png");

  m_nativeImage->first->SetFrame(0, 1, bitmap, is_complete);

  /*
  if (is_complete) {
    auto& pixels = const_cast<Vector<RGBA32>&>(m_pixels);
    pixels.clear();
  }
  */

  return m_nativeImage;
}

} // namespace WebCore
