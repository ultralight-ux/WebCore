// Copyright (C) 2023 Ultralight, Inc. All rights reserved.

#include "config.h"
#include "ImageBufferUltralightBackend.h"

#if USE(ULTRALIGHT)

#include "BitmapImage.h"
#include "GraphicsContextUltralight.h"
#include "ImageData.h"
#include "MIMETypeRegistry.h"
#include "NotImplemented.h"
#include <Ultralight/private/Canvas.h>
#include <math.h>
#include <wtf/Assertions.h>
#include <wtf/IsoMallocInlines.h>

namespace WebCore {

WTF_MAKE_ISO_ALLOCATED_IMPL(ImageBufferUltralightBackend);

std::unique_ptr<ImageBufferUltralightBackend> ImageBufferUltralightBackend::create(const Parameters& parameters, const ImageBufferCreationContext&)
{
    ProfiledZone;
    ASSERT(parameters.pixelFormat == PixelFormat::BGRA8);

    IntSize backendSize = calculateBackendSize(parameters);
    if (backendSize.isEmpty())
        return nullptr;

    uint32_t width = static_cast<uint32_t>(backendSize.width());
    uint32_t height = static_cast<uint32_t>(backendSize.height());

    // The underlying Surface is guaranteed to be a BitmapSurface so we can safely
    // obtain an ultralight::Bitmap later.
    auto bitmapSurfaceFactory = ultralight::GetBitmapSurfaceFactory();

    std::unique_ptr<ultralight::Surface> surface;
    surface.reset(bitmapSurfaceFactory->CreateSurface(width, height));

    if (!surface)
        return nullptr;

    ultralight::BitmapSurface* bitmapSurface = static_cast<ultralight::BitmapSurface*>(surface.get());
    ultralight::RefPtr<ultralight::Bitmap> bitmap = bitmapSurface->bitmap();

    auto canvas = ultralight::Canvas::Create(width, height, ultralight::BitmapFormat::BGRA8_UNORM_SRGB, surface.get());
    if (!canvas)
        return nullptr;

    auto context = makeUnique<GraphicsContextUltralight>(canvas);
    if (!context)
        return nullptr;

    return std::unique_ptr<ImageBufferUltralightBackend>(new ImageBufferUltralightBackend(parameters, WTFMove(context), WTFMove(surface), WTFMove(bitmap)));
}

size_t ImageBufferUltralightBackend::calculateMemoryCost(const ImageBufferBackend::Parameters& parameters)
{
    IntSize backendSize = calculateBackendSize(parameters);
    return ImageBufferBackend::calculateMemoryCost(backendSize, backendSize.width() * 4);
}

std::unique_ptr<ImageBufferUltralightBackend> ImageBufferUltralightBackend::create(const Parameters& parameters, const GraphicsContext&)
{
    return ImageBufferUltralightBackend::create(parameters, nullptr);
}

ImageBufferUltralightBackend::ImageBufferUltralightBackend(const Parameters& parameters, std::unique_ptr<GraphicsContext>&& context, std::unique_ptr<ultralight::Surface>&& surface, ultralight::RefPtr<ultralight::Bitmap>&& bitmap)
    : ImageBufferBackend(parameters)
    , m_context(WTFMove(context))
    , m_surface(WTFMove(surface))
    , m_bitmap(WTFMove(bitmap))
{
}

ImageBufferUltralightBackend::~ImageBufferUltralightBackend() {
    m_context = nullptr;
    m_bitmap = nullptr;
    if (m_surface) {
        auto bitmapSurfaceFactory = ultralight::GetBitmapSurfaceFactory();
        bitmapSurfaceFactory->DestroySurface(m_surface.release());
    }
}

GraphicsContext& ImageBufferUltralightBackend::context() const
{
    return *m_context;
}

void ImageBufferUltralightBackend::flushContext()
{
    static_cast<GraphicsContextUltralight&>(context()).platformContext()->FlushSurface();
}

IntSize ImageBufferUltralightBackend::backendSize() const
{
    return IntSize(m_bitmap->width(), m_bitmap->height());
}

RefPtr<NativeImage> ImageBufferUltralightBackend::copyNativeImage(BackingStoreCopy copyBehavior) const
{
    ProfiledZone;
    switch (copyBehavior) {
    case CopyBackingStore:
        return NativeImage::create(ultralight::Image::Create(ultralight::Bitmap::Create(*m_bitmap.get()), true));

    case DontCopyBackingStore:
        return NativeImage::create(ultralight::Image::Create(m_bitmap, true));
    }

    ASSERT_NOT_REACHED();
    return nullptr;
}

void ImageBufferUltralightBackend::clipToMask(GraphicsContext& context, const FloatRect& dest)
{
    ProfiledZone;
    static_cast<GraphicsContextUltralight&>(context).platformContext()->SetClip(copyNativeImage(DontCopyBackingStore)->platformImage(), dest);
}

RefPtr<PixelBuffer> ImageBufferUltralightBackend::getPixelBuffer(const PixelBufferFormat& outputFormat, const IntRect& srcRect, const ImageBufferAllocator & allocator) const
{
    ProfiledZone;
    return ImageBufferBackend::getPixelBuffer(outputFormat, srcRect, m_bitmap->raw_pixels(), allocator);
}


void ImageBufferUltralightBackend::putPixelBuffer(const PixelBuffer& pixelBuffer, const IntRect& srcRect, const IntPoint& destPoint, AlphaPremultiplication destFormat)
{
    ProfiledZone;
    ImageBufferBackend::putPixelBuffer(pixelBuffer, srcRect, destPoint, destFormat, m_bitmap->raw_pixels());
    m_surface->set_dirty_bounds({ 0, 0, (int)m_surface->width(), (int)m_surface->height() });
}

unsigned ImageBufferUltralightBackend::bytesPerRow() const
{
    return m_bitmap->row_bytes();
}

} // namespace WebCore

#endif // USE(ULTRALIGHT)
