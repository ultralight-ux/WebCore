#pragma once
#include "TextureMapper.h"

#if USE(TEXTURE_MAPPER_ULTRALIGHT)

#include "ClipStackUltralight.h"
#include <Ultralight/private/Canvas.h>

namespace WebCore {

class FilterOperation;
class ClipStackUltralight;

class WEBCORE_EXPORT TextureMapperUltralight : public TextureMapper {
public:
    TextureMapperUltralight(bool use_gpu, double scale);
    virtual ~TextureMapperUltralight();

    double scale() const { return scale_; }
    void set_scale(double scale) { scale_ = scale; }

    void set_default_surface(ultralight::RefPtr<ultralight::Canvas> canvas);

    virtual void drawBorder(const Color&, float borderWidth, const FloatRect&,
        const TransformationMatrix&) override;

    virtual void drawNumber(int number, const Color&, const FloatPoint&,
        const TransformationMatrix&) override;

    virtual void drawTexture(const BitmapTexture&, const FloatRect& target,
        const TransformationMatrix& modelViewMatrix = TransformationMatrix(),
        float opacity = 1.0f, unsigned exposedEdges = AllEdges) override;

    virtual void drawSolidColor(const FloatRect&, const TransformationMatrix&, const Color&, bool) override;

    virtual void clearColor(const Color&) override;

    // makes a surface the target for the following drawTexture calls.
    virtual void bindSurface(BitmapTexture* surface) override;

    BitmapTexture* currentSurface() { return current_surface_.get(); }

    virtual void beginClip(const TransformationMatrix&, const FloatRoundedRect&) override;

    virtual void endClip() override;

    virtual IntRect clipBounds() override;

    virtual Ref<BitmapTexture> createTexture() override;

    virtual Ref<BitmapTexture> createTexture(int internalFormat) override;

    virtual void setDepthRange(double zNear, double zFar) override;

    virtual void beginPainting(PaintFlags = 0) override;

    virtual void endPainting() override;

    virtual IntSize maxTextureSize() const override;

    void drawFiltered(const BitmapTexture& sourceTexture, const BitmapTexture* contentTexture, RefPtr<FilterOperation>, int pass, float adjustScale);

    void drawTextureWithScale(const BitmapTexture& sourceTexture, const IntSize& srcSize, const IntSize& dstSize);

    // Some filters require multiple passes to render correctly.
    // This function returns the number of passes required for the given filter.
    static unsigned getPassesRequiredForFilter(RefPtr<FilterOperation> filter, bool use_gpu);

    // Get the blur radius (std deviation) in absolute pixels for Blur and DropShadow filters.
    static float getBlurRadiusForFilter(RefPtr<FilterOperation> filter, const IntSize& size, float deviceScale);

    // Some filters (like blurs) will downscale the source texture prior to processing to improve performance.
    // This function returns the scale factor required for the given filter (1.0f = no scaling).
    static float getScaleRequiredForFilter(RefPtr<FilterOperation> filter, const IntSize& size, float deviceScale, bool use_gpu);

protected:
    bool use_gpu_;
    double scale_;
    RefPtr<BitmapTexture> current_surface_;
    RefPtr<BitmapTexture> default_surface_;
    ClipStackUltralight clip_stack_;
};

} // namespace WebCore

#endif
