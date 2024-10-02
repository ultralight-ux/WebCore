#pragma once

#if USE(TEXTURE_MAPPER_ULTRALIGHT)

#include "BitmapTexture.h"
#include "FilterOperation.h"
#include "ClipStackUltralight.h"
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

    static Ref<BitmapTexture> create(bool use_gpu, const Flags flags = NoFlag)
    {
        return adoptRef(*new BitmapTextureUltralight(use_gpu, flags));
    }

    static Ref<BitmapTexture> create(ultralight::RefPtr<ultralight::Canvas> canvas)
    {
        return adoptRef(*new BitmapTextureUltralight(canvas));
    }

    // Create a new BitmapTextureUltralight (canvas will be lazily created):
    BitmapTextureUltralight(bool use_gpu, const Flags = NoFlag);

    // Create from an existing Ultralight Canvas:
    BitmapTextureUltralight(ultralight::RefPtr<ultralight::Canvas> canvas);

    virtual ~BitmapTextureUltralight();

    ultralight::RefPtr<ultralight::Canvas> canvas() const { return canvas_; }

    bool use_gpu() const { return use_gpu_; }

    void applyClipIfNeeded(const ClipStackUltralight& clip);

    // Inherited from BitmapTexture:
    
    virtual bool isBackedByUltralight() const override { return true; }

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
    struct FilterInfo {
        RefPtr<FilterOperation> filter;
        unsigned pass;
        RefPtr<BitmapTexture> contentTexture;

        FilterInfo(RefPtr<FilterOperation>&& f = nullptr, unsigned p = 0, RefPtr<BitmapTexture>&& t = nullptr)
            : filter(WTFMove(f))
            , pass(p)
            , contentTexture(WTFMove(t))
            { }
    };
    const FilterInfo* filterInfo() const { return &filter_info_; }

protected:
    bool use_gpu_;
    bool owns_canvas_;
    std::unique_ptr<ultralight::Surface> surface_; // CPU backing store, only used when use_gpu_ is false.
    ultralight::RefPtr<ultralight::Canvas> canvas_;
    IntSize canvas_size_;
    FilterInfo filter_info_;
    size_t clip_hash_ = 0;
    bool clip_applied_ = false;
};

}  // namespace WebCore

#endif // USE(TEXTURE_MAPPER_ULTRALIGHT)
