#pragma once

#if USE(TEXTURE_MAPPER_ULTRALIGHT)

#include "BitmapTexture.h"
#include <memory>
#include <Ultralight/private/Canvas.h>
#include <Ultralight/platform/Platform.h>
#include <Ultralight/platform/Surface.h>
#include <Ultralight/Bitmap.h>

namespace WebCore {

class TextureMapper;
class FilterOperation;

class WEBCORE_EXPORT BitmapTextureUltralight : public BitmapTexture {
public:
    BitmapTextureUltralight(bool use_gpu, const Flags = NoFlag);

    virtual ~BitmapTextureUltralight();

    ultralight::RefPtr<ultralight::Canvas> canvas() const { return canvas_; }

    // Inherited from BitmapTexture:

    virtual void didReset() override;

    virtual IntSize size() const override { return canvas_size_; }

    virtual void updateContents(Image*, const IntRect&, const IntPoint& offset) override;

    virtual void updateContents(GraphicsLayer* sourceLayer, const IntRect& targetRect,
      const IntPoint& offset, float scale) override;

    virtual void updateContents(const void*, const IntRect& target,
        const IntPoint& offset, int bytesPerLine) override;

    virtual bool isValid() const override { return !!canvas_; }

    virtual RefPtr<BitmapTexture> applyFilters(TextureMapper&,
        const FilterOperations&, bool) override;

protected:
    bool use_gpu_;
    std::unique_ptr<ultralight::Surface> surface_; // CPU backing store, only used when use_gpu_ is false.
    ultralight::RefPtr<ultralight::Canvas> canvas_;
    IntSize canvas_size_;
};

}  // namespace WebCore

#endif // USE(TEXTURE_MAPPER_ULTRALIGHT)
