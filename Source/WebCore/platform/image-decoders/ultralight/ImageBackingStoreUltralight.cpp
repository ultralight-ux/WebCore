#pragma once

#include "config.h"
#include "ImageBackingStore.h"

#if USE(ULTRALIGHT)

namespace WebCore {

struct UserImageData {
    RefPtr<SharedBuffer::DataSegment> pixels;
};

static void OnDestroyImage(void* user_data, void* data)
{
    UserImageData* userData = reinterpret_cast<UserImageData*>(user_data);
    delete userData;
}

NativeImagePtr ImageBackingStore::image() const
{
    if (m_image) {
        if (m_pixelsDirty) {
            m_image->set_is_bitmap_dirty(true);
            m_pixelsDirty = false;
        }

        return m_image;
    }


    UserImageData* userData = new UserImageData();
    userData->pixels = m_pixels;

    uint32_t rowBytes = size().width() * sizeof(uint32_t);
    size_t pixelDataSize = rowBytes * size().height();

    ultralight::RefPtr<ultralight::Bitmap> bitmap = ultralight::Bitmap::Create(size().width(),
      size().height(), ultralight::BitmapFormat::BGRA8_UNORM_SRGB, rowBytes,
      const_cast<uint32_t*>(m_pixelsPtr), pixelDataSize, userData, OnDestroyImage);

    m_image = ultralight::Image::Create(bitmap, true);

    return m_image;
}

} // namespace WebCore

#endif
