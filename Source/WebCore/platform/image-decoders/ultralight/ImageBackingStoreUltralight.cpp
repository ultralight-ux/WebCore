#include "config.h"
#include "ImageBackingStore.h"
#include <Ultralight/Bitmap.h>
#include <Ultralight/private/Image.h>
#include <Ultralight/private/Paint.h>
#include <Ultralight/platform/Platform.h>
#include <Ultralight/platform/Config.h>

namespace WebCore {

ImageBackingStore::ImageBackingStore(const IntSize& size, bool premultiplyAlpha)
  : m_premultiplyAlpha(premultiplyAlpha)
{
  ASSERT(!size.isEmpty() && !isOverSize(size));
  setSize(size);
}

ImageBackingStore::ImageBackingStore(const ImageBackingStore& other)
  : m_bitmap(other.m_bitmap)
  , m_size(other.m_size)
  , m_premultiplyAlpha(other.m_premultiplyAlpha)
{
  ASSERT(!m_size.isEmpty() && !isOverSize(m_size));
}

NativeImagePtr ImageBackingStore::image(bool is_complete) const
{
  if (!m_nativeImage) {
    const_cast<ImageBackingStore*>(this)->m_nativeImage = std::make_shared<FramePair>(ultralight::Image::Create(), 0);
  }

  m_nativeImage->first->SetFrame(0, 1, m_bitmap, is_complete);

  return m_nativeImage;
}

RGBA32 ImageBackingStore::pixelValue(unsigned r, unsigned g, unsigned b, unsigned a) const
{
  if (m_premultiplyAlpha && !a)
    return 0;

  if (m_premultiplyAlpha && a < 255) {
      m_buf = { (float)r, (float)g, (float)b, (float)a };
      m_buf /= 255.0f;
      m_buf.x *= m_buf.w;
      m_buf.y *= m_buf.w;
      m_buf.z *= m_buf.w;
      m_buf *= 255.0f;
      return makeRGBA((int)m_buf.x, (int)m_buf.y, (int)m_buf.z, (int)m_buf.w);
  }

  return makeRGBA(r, g, b, a);
}

bool ImageBackingStore::setSize(const IntSize& size)
{
    if (size.isEmpty())
        return false;

    m_bitmap = ultralight::Bitmap::Create(size.width(), size.height(), ultralight::BitmapFormat::BGRA8_UNORM_SRGB);
    m_size = size;
    m_frameRect = IntRect(IntPoint(), m_size);
    clear();
    return true;
}

void ImageBackingStore::setFrameRect(const IntRect& frameRect)
{
    ASSERT(!m_size.isEmpty());
    ASSERT(inBounds(frameRect));
    m_frameRect = frameRect;
}

void ImageBackingStore::clear()
{
    m_bitmap->Erase();
}

void ImageBackingStore::clearRect(const IntRect& rect)
{
    if (rect.isEmpty() || !inBounds(rect))
        return;

    size_t rowBytes = rect.width() * sizeof(RGBA32);
    size_t rowLen = m_bitmap->row_bytes() / m_bitmap->bpp();
    RGBA32* start = pixelAt(rect.x(), rect.y());
    for (int i = 0; i < rect.height(); ++i) {
        memset(start, 0, rowBytes);
        start += rowLen;
    }
}

void ImageBackingStore::fillRect(const IntRect &rect, unsigned r, unsigned g, unsigned b, unsigned a)
{
    if (rect.isEmpty() || !inBounds(rect))
        return;

    RGBA32* start = pixelAt(rect.x(), rect.y());
    size_t rowLen = m_bitmap->row_bytes() / m_bitmap->bpp();
    RGBA32 pixelValue = this->pixelValue(r, g, b, a);
    for (int i = 0; i < rect.height(); ++i) {
        for (int j = 0; j < rect.width(); ++j)
            start[j] = pixelValue;
        start += rowLen;
    }
}

void ImageBackingStore::repeatFirstRow(const IntRect& rect)
{
    if (rect.isEmpty() || !inBounds(rect))
        return;

    size_t rowBytes = m_bitmap->row_bytes();
    uint8_t* src = (uint8_t*)pixelAt(rect.x(), rect.y());
    uint8_t* dest = src + rowBytes;
    for (int i = 1; i < rect.height(); ++i) {
        memcpy(dest, src, rowBytes);
        dest += rowBytes;
    }
}

RGBA32* ImageBackingStore::pixelAt(int x, int y) const
{
    ASSERT(inBounds(IntPoint(x, y)));
    return reinterpret_cast<RGBA32*>((uint8_t*)m_bitmap->raw_pixels() + y * m_bitmap->row_bytes() + x * m_bitmap->bpp());
}

void ImageBackingStore::setPixel(RGBA32* dest, unsigned r, unsigned g, unsigned b, unsigned a)
{
    ASSERT(dest);
    *dest = pixelValue(r, g, b, a);
}

void ImageBackingStore::setPixel(int x, int y, unsigned r, unsigned g, unsigned b, unsigned a)
{
    setPixel(pixelAt(x, y), r, g, b, a);
}

#if ENABLE(APNG)
void ImageBackingStore::blendPixel(RGBA32* dest, unsigned r, unsigned g, unsigned b, unsigned a)
{
    if (!a)
        return;

    if (a >= 255 || !alphaChannel(*dest)) {
        setPixel(dest, r, g, b, a);
        return;
    }

    if (!m_premultiplyAlpha)
        *dest = makePremultipliedRGBA(redChannel(*dest), greenChannel(*dest), blueChannel(*dest), alphaChannel(*dest));

    unsigned d = 255 - a;

    r = fastDivideBy255(r * a + redChannel(*dest) * d);
    g = fastDivideBy255(g * a + greenChannel(*dest) * d);
    b = fastDivideBy255(b * a + blueChannel(*dest) * d);
    a += fastDivideBy255(d * alphaChannel(*dest));

    if (m_premultiplyAlpha)
        *dest = makeRGBA(r, g, b, a);
    else
        *dest = makeUnPremultipliedRGBA(r, g, b, a);
}
#endif

} // namespace WebCore
