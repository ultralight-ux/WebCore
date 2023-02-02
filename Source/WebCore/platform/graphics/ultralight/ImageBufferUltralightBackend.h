// Copyright (C) 2023 Ultralight, Inc. All rights reserved.

#pragma once

#if USE(ULTRALIGHT)

#include "ImageBufferBackend.h"
#include <wtf/IsoMalloc.h>
#include <Ultralight/platform/Platform.h>
#include <Ultralight/platform/Surface.h>
#include <Ultralight/Bitmap.h>

namespace WebCore {

class ImageBufferUltralightBackend : public ImageBufferBackend {
    WTF_MAKE_ISO_ALLOCATED(ImageBufferUltralightBackend);
    WTF_MAKE_NONCOPYABLE(ImageBufferUltralightBackend);
public:
    static std::unique_ptr<ImageBufferUltralightBackend> create(const FloatSize&, float resolutionScale, ColorSpace, const HostWindow*);
    static std::unique_ptr<ImageBufferUltralightBackend> create(const FloatSize&, const GraphicsContext&);

    GraphicsContext& context() const override;
    void flushContext() override;

    NativeImagePtr copyNativeImage(BackingStoreCopy = CopyBackingStore) const override;
    RefPtr<Image> copyImage(BackingStoreCopy, PreserveResolution) const override;

    void draw(GraphicsContext&, const FloatRect& destRect, const FloatRect& srcRect, const ImagePaintingOptions&) override;
    void drawPattern(GraphicsContext&, const FloatRect& destRect, const FloatRect& srcRect, const AffineTransform& patternTransform, const FloatPoint& phase, const FloatSize& spacing, const ImagePaintingOptions&) override;

    RefPtr<Image> sinkIntoImage(PreserveResolution) override;

    String toDataURL(const String& mimeType, Optional<double> quality, PreserveResolution) const override;
    Vector<uint8_t> toData(const String& mimeType, Optional<double> quality) const override;
    Vector<uint8_t> toBGRAData() const override;

    RefPtr<ImageData> getImageData(AlphaPremultiplication outputFormat, const IntRect&) const override;
    void putImageData(AlphaPremultiplication inputFormat, const ImageData&, const IntRect& srcRect, const IntPoint& destPoint, AlphaPremultiplication destFormat) override;

protected:
    ImageBufferUltralightBackend(const FloatSize& logicalSize, const IntSize& physicalSize, float resolutionScale, ColorSpace, std::unique_ptr<GraphicsContext>&&, std::unique_ptr<ultralight::Surface>&&, ultralight::RefPtr<ultralight::Bitmap>&&);

    unsigned bytesPerRow() const override;

    std::unique_ptr<ultralight::Surface> m_surface;
    std::unique_ptr<GraphicsContext> m_context;
    ultralight::RefPtr<ultralight::Bitmap> m_bitmap;
};

}  // namespace WebCore

#endif  // USE(ULTRALIGHT)