// Copyright (C) 2025 Ultralight, Inc. All rights reserved.

#include "config.h"
#include "ImageBufferUltralightGPUBackend.h"

#if USE(ULTRALIGHT)

#include "GraphicsContextUltralight.h"
#include "NotImplemented.h"
#include <Ultralight/private/Canvas.h>
#include <Ultralight/private/CanvasRecorder.h>
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

    ultralight::RefPtr<ultralight::Canvas> canvas;
    bool usesDeferredRendering = false;

    // HTML5 Canvas elements require deferred rendering to handle thread-safe operations
    if (parameters.purpose == RenderingPurpose::Canvas) {
        // Create CanvasRecorder for deferred rendering
        // The recorder will capture all drawing operations thread-safely
        canvas = ultralight::Canvas::CreateRecorder(width, height, ultralight::BitmapFormat::BGRA8_UNORM_SRGB);
        if (!canvas)
            return nullptr;

        // Verify we got a recorder canvas
        if (canvas->type() != ultralight::CanvasType::Recorder) {
            return nullptr;
        }

        usesDeferredRendering = true;

#if defined(ENABLE_CANVAS_TRACING)
        CANVAS_TRACE_WITH_STREAM("ImageBufferUltralightGPUBackend::create",
            stream << "size=" << IntSize(width, height) << " canvas_type=Recorder mode=deferred");
#endif
    } else {
        // Create CanvasGPU by passing nullptr for surface parameter
        // This forces the Canvas factory to create a GPU backend instead of CPU/Skia
        canvas = ultralight::Canvas::Create(width, height, ultralight::BitmapFormat::BGRA8_UNORM_SRGB, nullptr);
        if (!canvas)
            return nullptr;

        // Verify we got a GPU canvas
        if (canvas->type() != ultralight::CanvasType::GPU) {
            // If we didn't get a GPU canvas, creation failed
            return nullptr;
        }

#if defined(ENABLE_CANVAS_TRACING)
        CANVAS_TRACE_WITH_STREAM("ImageBufferUltralightGPUBackend::create",
            stream << "size=" << IntSize(width, height) << " canvas_type=GPU mode=direct");
#endif
    }

    auto context = makeUnique<GraphicsContextUltralight>(canvas);
    if (!context)
        return nullptr;

    auto backend = std::unique_ptr<ImageBufferUltralightGPUBackend>(new ImageBufferUltralightGPUBackend(parameters, WTFMove(context)));

    // Store deferred rendering state
    if (usesDeferredRendering) {
        backend->m_usesDeferredRendering = true;
        backend->m_deferredRecordingCanvas = canvas;
    }

    return backend;
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
    if (m_usesDeferredRendering) {
        // Ensure GPU canvas exists (lazy-create if needed)
        if (!m_deferredGPUCanvas) {
            m_deferredGPUCanvas = ultralight::Canvas::Create(
                m_backendSize.width(),
                m_backendSize.height(),
                ultralight::BitmapFormat::BGRA8_UNORM_SRGB,
                nullptr  // nullptr = GPU canvas
            );

            // Verify we got a GPU canvas
            if (!m_deferredGPUCanvas || m_deferredGPUCanvas->type() != ultralight::CanvasType::GPU) {
                return;
            }

#if defined(ENABLE_CANVAS_TRACING)
            CANVAS_TRACE_WITH_STREAM("ImageBufferUltralightGPUBackend::flushContext",
                stream << "lazy-created GPU canvas for flush");
#endif
        }

        // Replay recorded commands to GPU canvas
        auto* recorder = m_deferredRecordingCanvas->as<ultralight::CanvasRecorder>();
        if (recorder) {
            recorder->ReplayTo(m_deferredGPUCanvas);

#if defined(ENABLE_CANVAS_TRACING)
            CANVAS_TRACE_WITH_STREAM("ImageBufferUltralightGPUBackend::flushContext",
                stream << "replayed commands during flush");
#endif
        }

        // Flush the GPU canvas
        m_deferredGPUCanvas->FlushSurface();
        m_deferredGPUCanvas->ShrinkAllocators();

        // Clear recorder commands to prevent memory accumulation
        // State (transform, alpha, etc.) is preserved for continued recording
        if (recorder) {
            recorder->ClearCommands();
        }

        // Invalidate cached native image since we've updated the canvas
        m_cachedNativeImage = nullptr;
    } else {
        // Direct GPU rendering path (non-Canvas ImageBuffers)
        // Flush GPU command queue
        // Note: GPU canvas doesn't have a surface, but we still need to flush commands
        auto* canvas = static_cast<GraphicsContextUltralight&>(context()).platformContext();
        canvas->FlushSurface();  // This will be a no-op for GPU canvas but ensures command queue is flushed
        canvas->ShrinkAllocators();
    }
}

IntSize ImageBufferUltralightGPUBackend::backendSize() const
{
    return m_backendSize;
}

RefPtr<NativeImage> ImageBufferUltralightGPUBackend::copyNativeImage(BackingStoreCopy copyBehavior) const
{
    ProfiledZone;

    // Preprocessing: Handle deferred rendering for HTML5 Canvas elements
    if (m_usesDeferredRendering) {
        // Lazy-create GPU canvas on first paint
        if (!m_deferredGPUCanvas) {
            m_deferredGPUCanvas = ultralight::Canvas::Create(
                m_backendSize.width(),
                m_backendSize.height(),
                ultralight::BitmapFormat::BGRA8_UNORM_SRGB,
                nullptr  // nullptr = GPU canvas
            );

            // Verify we got a GPU canvas
            if (!m_deferredGPUCanvas || m_deferredGPUCanvas->type() != ultralight::CanvasType::GPU) {
                return nullptr;
            }

#if defined(ENABLE_CANVAS_TRACING)
            CANVAS_TRACE_WITH_STREAM("ImageBufferUltralightGPUBackend::copyNativeImage",
                stream << "lazy-created GPU canvas size=" << m_backendSize);
#endif
        }

        // Replay recorded commands to GPU canvas
        // ReplayTo() drains commands but preserves recorder state for continued recording
        auto* recorder = m_deferredRecordingCanvas->as<ultralight::CanvasRecorder>();
        if (recorder) {
            recorder->ReplayTo(m_deferredGPUCanvas);

#if defined(ENABLE_CANVAS_TRACING)
            CANVAS_TRACE_WITH_STREAM("ImageBufferUltralightGPUBackend::copyNativeImage",
                stream << "replayed commands to GPU canvas");
#endif
        }
    }

    // Get the canvas to use (either deferred GPU canvas or direct GPU canvas)
    auto* canvas = m_usesDeferredRendering
        ? m_deferredGPUCanvas.get()
        : static_cast<GraphicsContextUltralight&>(context()).platformContext();

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
    if (m_usesDeferredRendering) {
        // Clear the recorder (clears commands and resets state)
        m_deferredRecordingCanvas->Clear();

        // Clear the GPU canvas if it exists
        if (m_deferredGPUCanvas) {
            m_deferredGPUCanvas->Clear();
        }

        // Invalidate cached native image
        m_cachedNativeImage = nullptr;

#if defined(ENABLE_CANVAS_TRACING)
        CANVAS_TRACE_WITH_STREAM("ImageBufferUltralightGPUBackend::clearContents",
            stream << "cleared deferred rendering canvases");
#endif
    } else {
        // Direct GPU rendering path (non-Canvas ImageBuffers)
        static_cast<GraphicsContextUltralight&>(context()).platformContext()->Clear();
    }
}

} // namespace WebCore

#endif // USE(ULTRALIGHT)
