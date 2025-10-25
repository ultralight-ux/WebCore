// Copyright (C) 2025 Ultralight, Inc. All rights reserved.

#pragma once

#if USE(ULTRALIGHT)

#include "ImageBufferBackend.h"
#include <wtf/IsoMalloc.h>

namespace WebCore {

class ImageBufferUltralightGPUBackend : public ImageBufferBackend {
    WTF_MAKE_ISO_ALLOCATED(ImageBufferUltralightGPUBackend);
    WTF_MAKE_NONCOPYABLE(ImageBufferUltralightGPUBackend);
public:
    // Static compile-time constants for GPU acceleration
    static constexpr RenderingMode renderingMode = RenderingMode::Accelerated;
    static constexpr bool canMapBackingStore = false;  // No direct CPU pixel access
    static constexpr bool isOriginAtBottomLeftCorner = false;

    virtual ~ImageBufferUltralightGPUBackend();

    static size_t calculateMemoryCost(const Parameters&);
    static size_t calculateExternalMemoryCost(const Parameters&);

    static std::unique_ptr<ImageBufferUltralightGPUBackend> create(const Parameters&, const ImageBufferCreationContext&);
    static std::unique_ptr<ImageBufferUltralightGPUBackend> create(const Parameters&, const GraphicsContext&);

    GraphicsContext& context() const override;
    void flushContext() override;

    IntSize backendSize() const override;

    RefPtr<NativeImage> copyNativeImage(BackingStoreCopy = CopyBackingStore) const override;

    void clipToMask(GraphicsContext&, const FloatRect&) override;

    RefPtr<PixelBuffer> getPixelBuffer(const PixelBufferFormat& outputFormat, const IntRect&, const ImageBufferAllocator& = ImageBufferAllocator()) const override;
    void putPixelBuffer(const PixelBuffer&, const IntRect& srcRect, const IntPoint& destPoint, AlphaPremultiplication destFormat) override;

    void clearContents() override;

protected:
    ImageBufferUltralightGPUBackend(const Parameters& parameters, std::unique_ptr<GraphicsContext>&&);

    unsigned bytesPerRow() const override;

    std::unique_ptr<GraphicsContext> m_context;
    mutable RefPtr<NativeImage> m_cachedNativeImage;
    IntSize m_backendSize;

    // Deferred rendering support for HTML5 Canvas elements
    // When RenderingPurpose::Canvas is detected, we use CanvasRecorder to record
    // drawing operations thread-safely, then replay to GPU canvas during paint phase
    mutable ultralight::RefPtr<ultralight::Canvas> m_deferredGPUCanvas;       // Lazy-created GPU canvas for replay
    ultralight::RefPtr<ultralight::Canvas> m_deferredRecordingCanvas;         // CanvasRecorder for recording
    bool m_usesDeferredRendering = false;                                     // Flag indicating deferred mode
};

}  // namespace WebCore

#endif  // USE(ULTRALIGHT)
