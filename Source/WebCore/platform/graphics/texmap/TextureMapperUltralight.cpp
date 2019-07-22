#include "config.h"
#include "TextureMapperUltralight.h"
#include "BitmapTexturePool.h"

#if USE(TEXTURE_MAPPER_ULTRALIGHT)

#include "BitmapTextureUltralight.h"

namespace WebCore {

TextureMapperUltralight::TextureMapperUltralight() {
  m_texturePool = std::make_unique<BitmapTexturePool>();
}

TextureMapperUltralight::~TextureMapperUltralight() {}

void TextureMapperUltralight::set_default_surface(
  ultralight::RefPtr<ultralight::Canvas> canvas) {
  default_surface_ = canvas;
}

void TextureMapperUltralight::drawBorder(const Color&, float borderWidth,
    const FloatRect&, const TransformationMatrix&) {}

void TextureMapperUltralight::drawNumber(int number, const Color&,
    const FloatPoint&, const TransformationMatrix&) {}

void TextureMapperUltralight::drawTexture(const BitmapTexture& texture,
    const FloatRect& target, const TransformationMatrix& modelViewMatrix,
    float opacity, unsigned exposedEdges) {
    if (!texture.isValid())
        return;

    if (!current_surface_)
      return;

    auto srcCanvas = static_cast<const BitmapTextureUltralight&>(texture).canvas();
    ultralight::Rect src_uv = srcCanvas->render_target().uv_coords;
    ultralight::Rect dest = { target.x(), target.y(), target.maxX(), target.maxY() };
    ultralight::Paint paint;
    paint.color = UltralightRGBA(255, 255, 255, (unsigned char)(opacity * 255.0f));
 
    current_surface_->Save();
    current_surface_->Transform(modelViewMatrix);
    current_surface_->DrawCanvas(*srcCanvas, src_uv, dest, paint);
    current_surface_->Restore();

    //paint.color = UltralightColorGREEN;
    //current_surface_->DrawRect({ 20, 20, 30, 30 }, paint);
}

void TextureMapperUltralight::drawSolidColor(const FloatRect&,
    const TransformationMatrix&, const Color&) {}

// makes a surface the target for the following drawTexture calls.
void TextureMapperUltralight::bindSurface(BitmapTexture* surface) {
  if (!surface) {
    current_surface_ = default_surface_;
    return;
  }

  current_surface_ = static_cast<BitmapTextureUltralight*>(surface)->canvas();
}

void TextureMapperUltralight::beginClip(const TransformationMatrix&,
    const FloatRect&) {}

void TextureMapperUltralight::endClip() {}

IntRect TextureMapperUltralight::clipBounds() {
  if (!default_surface_)
    IntRect(0, 0, 2048, 2048);
    
  return IntRect(0, 0, default_surface_->width(), default_surface_->height());
}

PassRefPtr<BitmapTexture> TextureMapperUltralight::createTexture() {
    BitmapTextureUltralight* texture = new BitmapTextureUltralight();
    return adoptRef(texture);
}

void TextureMapperUltralight::beginPainting(PaintFlags) {
    bindSurface(0);
}

void TextureMapperUltralight::endPainting() {}

IntSize TextureMapperUltralight::maxTextureSize() const {
    return IntSize(2048, 2048);
}

}  // namespace WebCore

#endif
