#include "config.h"
#include "TextureMapperUltralight.h"
#include "BitmapTexturePool.h"
#include "FloatQuad.h"
#include "NotImplemented.h"

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
    ultralight::Rect src = { 0.0f, 0.0f, (float)srcCanvas->width(), (float)srcCanvas->height() };
    ultralight::Rect dest = { target.x(), target.y(), target.maxX(), target.maxY() };
    ultralight::Paint paint;
    paint.color = UltralightRGBA(255, 255, 255, (unsigned char)(opacity * 255.0f));
 
    current_surface_->Save();
    current_surface_->Transform(modelViewMatrix);
    current_surface_->DrawCanvas(srcCanvas, src, dest, paint);
    current_surface_->Restore();

    //paint.color = UltralightColorGREEN;
    //current_surface_->DrawRect({ 20, 20, 30, 30 }, paint);
}

void TextureMapperUltralight::drawSolidColor(const FloatRect&, const TransformationMatrix&, const Color&, bool) {}

void TextureMapperUltralight::clearColor(const Color&) {
  // TODO
  notImplemented();
}

// makes a surface the target for the following drawTexture calls.
void TextureMapperUltralight::bindSurface(BitmapTexture* surface) {
  if (!surface) {
    current_surface_ = default_surface_;
    return;
  }

  current_surface_ = static_cast<BitmapTextureUltralight*>(surface)->canvas();
}

void TextureMapperUltralight::beginClip(const TransformationMatrix& mat,
    const FloatRect& rect) {
  auto surface = current_surface_ ? current_surface_ : default_surface_;
  surface->Save();

  // TODO: support 3d transforms in clip
  if (!mat.isAffine())
    return;

  FloatQuad quad = mat.projectQuad(rect);
  IntRect bbox = quad.enclosingBoundingBox();

  // Only use scissors on rectilinear clips.
  if (!quad.isRectilinear() || bbox.isEmpty())
    return;

  ultralight::IntRect scissor_box = { bbox.x(), bbox.y(), bbox.maxX(), bbox.maxY() };
  ultralight::IntRect new_scissor = surface->GetScissorRect().Intersect(scissor_box);

  surface->set_scissor_enabled(true);
  surface->SetScissorRect(new_scissor);
}

void TextureMapperUltralight::endClip() {
  auto surface = current_surface_ ? current_surface_ : default_surface_;
  surface->Restore();
}

IntRect TextureMapperUltralight::clipBounds() {
  auto surface = current_surface_ ? current_surface_ : default_surface_;
  ultralight::IntRect bounds = surface->GetScissorRect();
  return IntRect(bounds.x(), bounds.y(), bounds.width(), bounds.height());
}

Ref<BitmapTexture> TextureMapperUltralight::createTexture() {
    BitmapTextureUltralight* texture = new BitmapTextureUltralight();
    return *adoptRef(texture);
}

Ref<BitmapTexture> TextureMapperUltralight::createTexture(int internalFormat) {
  return createTexture();
}

void TextureMapperUltralight::beginPainting(PaintFlags) {
    bindSurface(0);
}

void TextureMapperUltralight::endPainting() {}

IntSize TextureMapperUltralight::maxTextureSize() const {
    return IntSize(4096, 4096);
}

}  // namespace WebCore

#endif
