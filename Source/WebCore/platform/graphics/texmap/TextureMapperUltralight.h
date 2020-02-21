#pragma once
#include "TextureMapper.h"

#if USE(TEXTURE_MAPPER_ULTRALIGHT)

#include <Ultralight/private/Canvas.h>

namespace WebCore {

class WEBCORE_EXPORT TextureMapperUltralight : public TextureMapper {
public:
    TextureMapperUltralight();
    virtual ~TextureMapperUltralight();

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

    virtual void beginClip(const TransformationMatrix&, const FloatRect&) override;

    virtual void endClip() override;

    virtual IntRect clipBounds() override;

    virtual Ref<BitmapTexture> createTexture() override;

	virtual Ref<BitmapTexture> createTexture(int internalFormat) override;

    virtual void beginPainting(PaintFlags = 0) override;

    virtual void endPainting() override;

    virtual IntSize maxTextureSize() const override;

protected:
    ultralight::RefPtr<ultralight::Canvas> default_surface_;
    ultralight::RefPtr<ultralight::Canvas> current_surface_;
};

}  // namespace WebCore

#endif
