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

GraphicsContext& ImageBufferUltralightBackend::context() const
{
    return *m_context;
}

void ImageBufferUltralightBackend::flushContext()
{
    // not implemented
}

RefPtr<NativeImage> ImageBufferUltralightBackend::copyNativeImage(BackingStoreCopy copyBehavior) const
{
    switch (copyBehavior) {
    case CopyBackingStore:
        return NativeImage::create(ultralight::Image::Create(ultralight::Bitmap::Create(*m_bitmap.get()), true));

    case DontCopyBackingStore:
        return NativeImage::create(ultralight::Image::Create(m_bitmap, true));
    }

    ASSERT_NOT_REACHED();
    return nullptr;
}

/*
static RefPtr<NativeImage> createCroppedImageIfNecessary(RefPtr<NativeImage> image, const IntSize& backendSize)
{
    if (image && (image->platformImage()->bitmap()->width() != static_cast<uint32_t>(backendSize.width()) || image->bitmap()->height() != static_cast<uint32_t>(backendSize.height()))) {
        auto croppedBitmap = ultralight::Bitmap::Create(backendSize.width(), backendSize.height(), ultralight::BitmapFormat::BGRA8_UNORM_SRGB);
        ultralight::IntRect srcRect = { 0, 0, (int)image->bitmap()->width(), (int)image->bitmap()->height() };
        ultralight::IntRect dstRect = { 0, 0, backendSize.width(), backendSize.height() };
        if (!croppedBitmap->DrawBitmap(srcRect, dstRect, image->bitmap(), false))
            return nullptr;

        return NativeImage::create(ultralight::Image::Create(croppedBitmap, true));
    }

    return image;
}

static RefPtr<Image> createBitmapImageAfterScalingIfNeeded(NativeImagePtr&& image, const IntSize& logicalSize, const IntSize& backendSize, float resolutionScale, PreserveResolution preserveResolution)
{
    if (resolutionScale == 1 || preserveResolution == PreserveResolution::Yes)
        image = createCroppedImageIfNecessary(image.get(), backendSize);
    else {
        // FIXME: Need to implement scaled version
        notImplemented();
    }

    if (!image)
        return nullptr;

    return BitmapImage::create(WTFMove(image));
}
*/

/*
RefPtr<Image> ImageBufferUltralightBackend::copyImage(BackingStoreCopy copyBehavior, PreserveResolution preserveResolution) const
{
    NativeImagePtr image;
    if (m_resolutionScale == 1 || preserveResolution == PreserveResolution::Yes)
        image = copyNativeImage(copyBehavior);
    else
        image = copyNativeImage(DontCopyBackingStore);
    return createBitmapImageAfterScalingIfNeeded(WTFMove(image), m_logicalSize, m_backendSize, m_resolutionScale, preserveResolution);
}
*/

/*
void ImageBufferUltralightBackend::draw(GraphicsContext& destContext, const FloatRect& destRect, const FloatRect& srcRect, const ImagePaintingOptions& options)
{
    FloatRect srcRectScaled = srcRect;
    srcRectScaled.scale(m_resolutionScale);

    if (auto image = copyNativeImage(&destContext == &context() ? CopyBackingStore : DontCopyBackingStore))
        destContext.drawNativeImage(image.get(), m_backendSize, destRect, srcRectScaled, options);
}

void ImageBufferUltralightBackend::drawPattern(GraphicsContext& destContext, const FloatRect& destRect, const FloatRect& srcRect, const AffineTransform& patternTransform, const FloatPoint& phase, const FloatSize& spacing, const ImagePaintingOptions& options)
{
    FloatRect adjustedSrcRect = srcRect;
    adjustedSrcRect.scale(m_resolutionScale);

    if (auto image = copyImage(&destContext == &context() ? CopyBackingStore : DontCopyBackingStore, PreserveResolution::No))
        image->drawPattern(destContext, destRect, adjustedSrcRect, patternTransform, phase, spacing, options);
}
*/

/*
RefPtr<Image> ImageBufferUltralightBackend::sinkIntoImage(PreserveResolution preserveResolution)
{
    return createBitmapImageAfterScalingIfNeeded(sinkIntoNativeImage(), m_logicalSize, m_backendSize, m_resolutionScale, preserveResolution);
}
*/

/*
String ImageBufferUltralightBackend::toDataURL(const String& mimeType, Optional<double> quality, PreserveResolution) const
{
    notImplemented();
    return "data:,"_s;
}

Vector<uint8_t> ImageBufferUltralightBackend::toData(const String& mimeType, Optional<double> quality) const
{
    notImplemented();
    return {};
}

Vector<uint8_t> ImageBufferUltralightBackend::toBGRAData() const
{
    notImplemented();
    return {};
}

RefPtr<ImageData> ImageBufferUltralightBackend::getImageData(AlphaPremultiplication outputFormat, const IntRect& srcRect) const
{
    return ImageBufferBackend::getImageData(outputFormat, srcRect, m_bitmap->raw_pixels());
}

void ImageBufferUltralightBackend::putImageData(AlphaPremultiplication inputFormat, const ImageData& imageData, const IntRect& srcRect, const IntPoint& destPoint, AlphaPremultiplication destFormat)
{
    ImageBufferBackend::putImageData(inputFormat, imageData, srcRect, destPoint, destFormat, m_bitmap->raw_pixels());
    m_surface->set_dirty_bounds({ 0, 0, (int)m_surface->width(), (int)m_surface->height() });
}
*/

RefPtr<PixelBuffer> ImageBufferUltralightBackend::getPixelBuffer(const PixelBufferFormat& outputFormat, const IntRect& srcRect, const ImageBufferAllocator & allocator) const
{
    return ImageBufferBackend::getPixelBuffer(outputFormat, srcRect, m_bitmap->raw_pixels(), allocator);
}


void ImageBufferUltralightBackend::putPixelBuffer(const PixelBuffer& pixelBuffer, const IntRect& srcRect, const IntPoint& destPoint, AlphaPremultiplication destFormat)
{
    ImageBufferBackend::putPixelBuffer(pixelBuffer, srcRect, destPoint, destFormat, m_bitmap->raw_pixels());
    m_surface->set_dirty_bounds({ 0, 0, (int)m_surface->width(), (int)m_surface->height() });
}

unsigned ImageBufferUltralightBackend::bytesPerRow() const
{
    return m_bitmap->row_bytes();
}

} // namespace WebCore

#endif // USE(ULTRALIGHT)
