// Copyright (C) 2023 Ultralight, Inc. All rights reserved.

#include "config.h"
#include "ImageBufferUltralightBackend.h"

#if USE(ULTRALIGHT)

#include "BitmapImage.h"
#include "GraphicsContext.h"
#include "ImageData.h"
#include "MIMETypeRegistry.h"
#include "NotImplemented.h"
#include <Ultralight/private/Canvas.h>
#include <math.h>
#include <wtf/Assertions.h>
#include <wtf/IsoMallocInlines.h>

namespace WebCore {

WTF_MAKE_ISO_ALLOCATED_IMPL(ImageBufferUltralightBackend);

std::unique_ptr<ImageBufferUltralightBackend> ImageBufferUltralightBackend::create(const FloatSize& size, float resolutionScale, ColorSpace colorSpace, const HostWindow* hostWindow)
{
    IntSize backendSize = calculateBackendSize(size, resolutionScale);
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

    auto context = makeUnique<GraphicsContext>(canvas);
    if (!context)
        return nullptr;

    return std::unique_ptr<ImageBufferUltralightBackend>(new ImageBufferUltralightBackend(size, backendSize, resolutionScale, colorSpace, WTFMove(context), WTFMove(surface), WTFMove(bitmap)));
}

std::unique_ptr<ImageBufferUltralightBackend> ImageBufferUltralightBackend::create(const FloatSize& size, const GraphicsContext& context)
{
    return ImageBufferUltralightBackend::create(size, 1, ColorSpace::SRGB, nullptr);
}

ImageBufferUltralightBackend::ImageBufferUltralightBackend(const FloatSize& logicalSize, const IntSize& backendSize, float resolutionScale, ColorSpace colorSpace, std::unique_ptr<GraphicsContext>&& context, std::unique_ptr<ultralight::Surface>&& surface, ultralight::RefPtr<ultralight::Bitmap>&& bitmap)
    : ImageBufferBackend(logicalSize, backendSize, resolutionScale, colorSpace)
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

NativeImagePtr ImageBufferUltralightBackend::copyNativeImage(BackingStoreCopy copyBehavior) const
{
    switch (copyBehavior) {
    case CopyBackingStore:
        return ultralight::Image::Create(ultralight::Bitmap::Create(*m_bitmap.get()), true);

    case DontCopyBackingStore:
        return ultralight::Image::Create(m_bitmap, true);
    }

    ASSERT_NOT_REACHED();
    return nullptr;
}

static NativeImagePtr createCroppedImageIfNecessary(NativeImagePtr image, const IntSize& backendSize)
{
    if (image && (image->bitmap()->width() != static_cast<uint32_t>(backendSize.width()) || image->bitmap()->height() != static_cast<uint32_t>(backendSize.height()))) {
        auto croppedBitmap = ultralight::Bitmap::Create(backendSize.width(), backendSize.height(), ultralight::BitmapFormat::BGRA8_UNORM_SRGB);
        ultralight::IntRect srcRect = { 0, 0, (int)image->bitmap()->width(), (int)image->bitmap()->height() };
        ultralight::IntRect dstRect = { 0, 0, backendSize.width(), backendSize.height() };
        if (!croppedBitmap->DrawBitmap(srcRect, dstRect, image->bitmap(), false))
            return nullptr;

        return ultralight::Image::Create(croppedBitmap, true);
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

RefPtr<Image> ImageBufferUltralightBackend::copyImage(BackingStoreCopy copyBehavior, PreserveResolution preserveResolution) const
{
    NativeImagePtr image;
    if (m_resolutionScale == 1 || preserveResolution == PreserveResolution::Yes)
        image = copyNativeImage(copyBehavior);
    else
        image = copyNativeImage(DontCopyBackingStore);
    return createBitmapImageAfterScalingIfNeeded(WTFMove(image), m_logicalSize, m_backendSize, m_resolutionScale, preserveResolution);
}

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

RefPtr<Image> ImageBufferUltralightBackend::sinkIntoImage(PreserveResolution preserveResolution)
{
    return createBitmapImageAfterScalingIfNeeded(sinkIntoNativeImage(), m_logicalSize, m_backendSize, m_resolutionScale, preserveResolution);
}

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

unsigned ImageBufferUltralightBackend::bytesPerRow() const
{
    return m_bitmap->row_bytes();
}

} // namespace WebCore

#endif // USE(ULTRALIGHT)
