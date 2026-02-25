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

// Static instance registry
Lock ImageBufferUltralightGPUBackend::s_instanceLock;
HashSet<ImageBufferUltralightGPUBackend*> ImageBufferUltralightGPUBackend::s_instances;

size_t ImageBufferUltralightGPUBackend::totalCount()
{
    Locker locker { s_instanceLock };
    return s_instances.size();
}

size_t ImageBufferUltralightGPUBackend::totalMemoryUsage()
{
    Locker locker { s_instanceLock };
    size_t total = 0;
    for (auto* instance : s_instances)
        total += static_cast<size_t>(instance->backendSize().area()) * 4;
    return total;
}

std::unique_ptr<ImageBufferUltralightGPUBackend> ImageBufferUltralightGPUBackend::create(const Parameters& parameters, const ImageBufferCreationContext&)
{
    ProfiledZone;
    ASSERT(parameters.pixelFormat == PixelFormat::BGRA8);

    IntSize backendSize = calculateBackendSize(parameters);
    if (backendSize.isEmpty())
        return nullptr;

    uint32_t width = static_cast<uint32_t>(backendSize.width());
    uint32_t height = static_cast<uint32_t>(backendSize.height());

    // Use CreateGPUDeferred for deferred GPU rendering
    // CanvasGPUDeferred records commands to a CanvasRecorder and syncs to a CanvasGPU at paint time
    auto canvas = ultralight::Canvas::CreateGPUDeferred(width, height, ultralight::BitmapFormat::BGRA8_UNORM_SRGB);
    if (!canvas)
        return nullptr;

    // Verify we got a GPUDeferred canvas
    if (canvas->type() != ultralight::CanvasType::GPUDeferred) {
        return nullptr;
    }

#if defined(ENABLE_CANVAS_TRACING)
    CANVAS_TRACE_WITH_STREAM("ImageBufferUltralightGPUBackend::create",
        stream << "size=" << IntSize(width, height) << " canvas_type=GPUDeferred");
#endif

    auto context = makeUnique<GraphicsContextUltralight>(canvas);
    if (!context)
        return nullptr;

    return std::unique_ptr<ImageBufferUltralightGPUBackend>(
        new ImageBufferUltralightGPUBackend(parameters, WTFMove(context)));
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
    Locker locker { s_instanceLock };
    s_instances.add(this);
}

ImageBufferUltralightGPUBackend::~ImageBufferUltralightGPUBackend()
{
    {
        Locker locker { s_instanceLock };
        s_instances.remove(this);
    }
    m_context = nullptr;
}

GraphicsContext& ImageBufferUltralightGPUBackend::context() const
{
    return *m_context;
}

void ImageBufferUltralightGPUBackend::flushContext()
{
    // CanvasGPUDeferred handles sync internally at paint time
    // Just invalidate our cached native image
    m_cachedNativeImage = nullptr;
}

IntSize ImageBufferUltralightGPUBackend::backendSize() const
{
    return m_backendSize;
}

RefPtr<NativeImage> ImageBufferUltralightGPUBackend::copyNativeImage(BackingStoreCopy copyBehavior) const
{
    ProfiledZone;

    auto* canvas = static_cast<GraphicsContextUltralight&>(context()).platformContext();
    if (!canvas)
        return nullptr;

#if defined(ENABLE_CANVAS_TRACING)
    CANVAS_TRACE_WITH_STREAM("ImageBufferUltralightGPUBackend::copyNativeImage",
        stream << "copyBehavior=" << (copyBehavior == CopyBackingStore ? "CopyBackingStore" : "DontCopyBackingStore")
               << " canvas_id=" << canvas->id()
               << " canvas_type=" << static_cast<int>(canvas->type()));
#endif

    switch (copyBehavior) {
    case CopyBackingStore: {
        // Use Canvas::Clone() to create a deep copy
        // Clone captures both GPU content and pending recorder commands
        auto clone = canvas->Clone();
        if (!clone) {
#if defined(ENABLE_CANVAS_TRACING)
            CANVAS_TRACE("ImageBufferUltralightGPUBackend::copyNativeImage: Clone() returned null");
#endif
            return nullptr;
        }

#if defined(ENABLE_CANVAS_TRACING)
        CANVAS_TRACE_WITH_STREAM("ImageBufferUltralightGPUBackend::copyNativeImage",
            stream << "cloned canvas_id=" << clone->id() << " canvas_type=" << static_cast<int>(clone->type()));
#endif
        return NativeImage::create(ultralight::Image::Create(clone));
    }

    case DontCopyBackingStore: {
        // Return cached image if available
        if (m_cachedNativeImage)
            return m_cachedNativeImage;

        // Create Image wrapping our existing canvas (no copy)
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
    static_cast<GraphicsContextUltralight&>(context).platformContext()->SetClip(copyNativeImage(DontCopyBackingStore)->platformImage(), dest);
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
    // CanvasGPUDeferred handles Clear() by forwarding to recorder and marking as unsynced
    static_cast<GraphicsContextUltralight&>(context()).platformContext()->Clear();

    // Invalidate cached native image
    m_cachedNativeImage = nullptr;
}

} // namespace WebCore

#endif // USE(ULTRALIGHT)
