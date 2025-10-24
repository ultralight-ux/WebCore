// Copyright (C) 2025 Ultralight, Inc. All rights reserved.

#include "config.h"
#include "ImageBufferUltralightGPUBackend.h"

#if USE(ULTRALIGHT)

#include "GraphicsContextUltralight.h"
#include "NotImplemented.h"
#include <Ultralight/private/Canvas.h>
#include <Ultralight/private/Image.h>
#include <Ultralight/private/Paint.h>
#include <wtf/IsoMallocInlines.h>
#include <Ultralight/private/CanvasProfiler.h>
#include <wtf/text/TextStream.h>

namespace WebCore {

WTF_MAKE_ISO_ALLOCATED_IMPL(ImageBufferUltralightGPUBackend);

std::unique_ptr<ImageBufferUltralightGPUBackend> ImageBufferUltralightGPUBackend::create(const Parameters& parameters, const ImageBufferCreationContext&)
{
    ProfiledZone;
    ASSERT(parameters.pixelFormat == PixelFormat::BGRA8);

    IntSize backendSize = calculateBackendSize(parameters);
    if (backendSize.isEmpty())
        return nullptr;

    uint32_t width = static_cast<uint32_t>(backendSize.width());
    uint32_t height = static_cast<uint32_t>(backendSize.height());

    // Create CanvasGPU by passing nullptr for surface parameter
    // This forces the Canvas factory to create a GPU backend instead of CPU/Skia
    auto canvas = ultralight::Canvas::Create(width, height, ultralight::BitmapFormat::BGRA8_UNORM_SRGB, nullptr);
    if (!canvas)
        return nullptr;

    // Verify we got a GPU canvas
    if (canvas->type() != ultralight::CanvasType::GPU) {
        // If we didn't get a GPU canvas, creation failed
        return nullptr;
    }

#if defined(ENABLE_CANVAS_TRACING)
    CANVAS_TRACE_WITH_STREAM("ImageBufferUltralightGPUBackend::create",
        stream << "size=" << IntSize(width, height) << " canvas_type=GPU");
#endif

    auto context = makeUnique<GraphicsContextUltralight>(canvas);
    if (!context)
        return nullptr;

    return std::unique_ptr<ImageBufferUltralightGPUBackend>(new ImageBufferUltralightGPUBackend(parameters, WTFMove(context)));
}

size_t ImageBufferUltralightGPUBackend::calculateMemoryCost(const ImageBufferBackend::Parameters& parameters)
{
    // GPU backend has minimal CPU memory cost (just pointers/handles)
    return 0;
}

size_t ImageBufferUltralightGPUBackend::calculateExternalMemoryCost(const ImageBufferBackend::Parameters& parameters)
{
    // GPU texture memory lives in GPU VRAM (external to process)
    IntSize backendSize = calculateBackendSize(parameters);
    return ImageBufferBackend::calculateMemoryCost(backendSize, backendSize.width() * 4);  // BGRA8 = 4 bytes/pixel
}

std::unique_ptr<ImageBufferUltralightGPUBackend> ImageBufferUltralightGPUBackend::create(const Parameters& parameters, const GraphicsContext&)
{
    return ImageBufferUltralightGPUBackend::create(parameters, nullptr);
}

ImageBufferUltralightGPUBackend::ImageBufferUltralightGPUBackend(const Parameters& parameters, std::unique_ptr<GraphicsContext>&& context)
    : ImageBufferBackend(parameters)
    , m_context(WTFMove(context))
    , m_backendSize(calculateBackendSize(parameters))
{
}

ImageBufferUltralightGPUBackend::~ImageBufferUltralightGPUBackend()
{
    m_context = nullptr;
}

GraphicsContext& ImageBufferUltralightGPUBackend::context() const
{
    return *m_context;
}

void ImageBufferUltralightGPUBackend::flushContext()
{
    // Flush GPU command queue
    // Note: GPU canvas doesn't have a surface, but we still need to flush commands
    auto* canvas = static_cast<GraphicsContextUltralight&>(context()).platformContext();
    canvas->FlushSurface();  // This will be a no-op for GPU canvas but ensures command queue is flushed
    canvas->ShrinkAllocators();
}

IntSize ImageBufferUltralightGPUBackend::backendSize() const
{
    return m_backendSize;
}

RefPtr<NativeImage> ImageBufferUltralightGPUBackend::copyNativeImage(BackingStoreCopy copyBehavior) const
{
    ProfiledZone;

    // Get the GPU canvas from our graphics context
    auto* canvas = static_cast<GraphicsContextUltralight&>(context()).platformContext();
    if (!canvas || canvas->type() != ultralight::CanvasType::GPU)
        return nullptr;

    switch (copyBehavior) {
    case CopyBackingStore: {
        // Create a deep copy by creating a new GPU Canvas and drawing to it

        // Create new GPU Canvas with same dimensions and format
        // IMPORTANT: Pass nullptr for surface parameter to get GPU canvas
        auto newCanvas = ultralight::Canvas::Create(
            m_backendSize.width(),
            m_backendSize.height(),
            ultralight::BitmapFormat::BGRA8_UNORM_SRGB,
            nullptr  // nullptr = GPU canvas
        );

        // Verify we got a GPU canvas
        if (!newCanvas || newCanvas->type() != ultralight::CanvasType::GPU)
            return nullptr;

        // Define source and destination rectangles
        ultralight::Rect src = {
            0.0f, 0.0f,
            static_cast<float>(m_backendSize.width()),
            static_cast<float>(m_backendSize.height())
        };
        ultralight::Rect dest = src;  // Same dimensions

        // Draw the source canvas to the new canvas (GPU→GPU copy)
        // UltralightColorWHITE = no tinting
        newCanvas->DrawCanvas(canvas, src, dest, UltralightColorWHITE);

        // NOTE: No Flush() needed - DrawCanvas sets up draw dependency

        // Create an Image wrapping the new canvas
        auto image = ultralight::Image::Create(newCanvas);

        // Return NativeImage wrapping the new canvas
        return NativeImage::create(WTFMove(image));
    }

    case DontCopyBackingStore: {
        // Return cached image if available (following CPU ImageBuffer pattern)
        if (m_cachedNativeImage)
            return m_cachedNativeImage;

        // Create Image wrapping our existing canvas (no copy)
        // This creates a reference, not a copy
        auto image = ultralight::Image::Create(canvas);

        // Cache and return the NativeImage
        m_cachedNativeImage = NativeImage::create(WTFMove(image));
        return m_cachedNativeImage;
    }
    }

    ASSERT_NOT_REACHED();
    return nullptr;
}

void ImageBufferUltralightGPUBackend::clipToMask(GraphicsContext& context, const FloatRect& dest)
{
    ProfiledZone;

    // TODO: Clipping to GPU canvas requires reading back GPU texture
    // This is currently not implemented

    notImplemented();
}

RefPtr<PixelBuffer> ImageBufferUltralightGPUBackend::getPixelBuffer(const PixelBufferFormat& outputFormat, const IntRect& srcRect, const ImageBufferAllocator& allocator) const
{
    ProfiledZone;

    // TODO: Implement GPU→CPU readback
    // This requires:
    // 1. Flush GPU command queue to ensure all rendering is complete
    // 2. Synchronize GPU fence/barrier
    // 3. Download texture data from GPU memory to CPU memory
    // 4. Convert format if needed (handle color space, alpha premultiplication)
    // 5. Return PixelBuffer with the downloaded data
    //
    // This is a complex operation that requires:
    // - Access to the GPU rendering context
    // - Proper synchronization to avoid reading incomplete/dirty data
    // - Efficient memory transfer from GPU to CPU
    // - Format conversion utilities
    //
    // For now, return nullptr (no pixel access for GPU buffers)

    notImplemented();
    return nullptr;
}

void ImageBufferUltralightGPUBackend::putPixelBuffer(const PixelBuffer& pixelBuffer, const IntRect& srcRect, const IntPoint& destPoint, AlphaPremultiplication destFormat)
{
    ProfiledZone;

    // TODO: Implement CPU→GPU upload
    // This requires:
    // 1. Convert PixelBuffer format if needed
    // 2. Upload pixel data to GPU texture
    // 3. Mark texture region as dirty/updated
    // 4. Handle partial updates (srcRect, destPoint)
    //
    // This is complex because:
    // - Need to update specific region of GPU texture
    // - Must handle format conversions
    // - Need proper synchronization
    //
    // For now, this is a no-op (pixel data won't be uploaded to GPU)

    notImplemented();
}

unsigned ImageBufferUltralightGPUBackend::bytesPerRow() const
{
    // GPU textures don't have a traditional "row stride" in CPU memory
    // Return the logical bytes per row based on width
    return m_backendSize.width() * 4;  // BGRA8 = 4 bytes per pixel
}

void ImageBufferUltralightGPUBackend::clearContents()
{
    static_cast<GraphicsContextUltralight&>(context()).platformContext()->Clear();
}

} // namespace WebCore

#endif // USE(ULTRALIGHT)
