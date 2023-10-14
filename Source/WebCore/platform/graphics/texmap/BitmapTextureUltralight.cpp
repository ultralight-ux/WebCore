#include "config.h"
#include "BitmapTextureUltralight.h"

#if USE(TEXTURE_MAPPER_ULTRALIGHT) 

#include "FilterOperations.h"
#include "GraphicsContextUltralight.h"
#include "Image.h"
#include "GraphicsLayer.h"
#include "TextureMapper.h"
#include <Ultralight/platform/Platform.h>
#include <Ultralight/platform/Config.h>

namespace WebCore {

BitmapTextureUltralight::BitmapTextureUltralight(bool use_gpu, const BitmapTexture::Flags flags) : use_gpu_(use_gpu) {
}

BitmapTextureUltralight::~BitmapTextureUltralight() {
  if (canvas_) {
    canvas_->RecycleRenderTexture();
    canvas_ = nullptr;
  }
}

void BitmapTextureUltralight::didReset() {
  ProfiledZone;
  canvas_size_ = contentSize();

  if (canvas_) {
    if(canvas_size_.width() != canvas_->width() || canvas_size_.height() != canvas_->height())
      canvas_->Resize(canvas_size_.width(), canvas_size_.height());
      
    canvas_->Clear();
    return;
  }

  if (!use_gpu_ && !surface_) {
    // The underlying Surface is guaranteed to be a BitmapSurface.
    auto bitmapSurfaceFactory = ultralight::GetBitmapSurfaceFactory();
    surface_.reset(bitmapSurfaceFactory->CreateSurface(canvas_size_.width(), canvas_size_.height()));
  }
    
  canvas_ = ultralight::Canvas::Create(canvas_size_.width(),
      canvas_size_.height(), ultralight::BitmapFormat::BGRA8_UNORM_SRGB, surface_.get());
}

void BitmapTextureUltralight::updateContents(Image* image,
    const IntRect& targetRect, const IntPoint& offset) {
  ProfiledZone;
  if (!image)
        return;

    if (image->isBitmapImage()) {
      RefPtr<NativeImage> frameImage = image->nativeImageForCurrentFrame();

      IntSize imageSize = frameImage->size();

      ultralight::Rect srcRect = { 0.0f, 0.0f, (float)imageSize.width(),
          (float)imageSize.height() };
      srcRect.Move(offset.x(), offset.y());

      ultralight::Rect destRect = { (float)targetRect.x(), (float)targetRect.y(),
        (float)targetRect.maxX(), (float)targetRect.maxY() };

      canvas_->set_blending_enabled(false);
      canvas_->DrawImage(frameImage->platformImage(), srcRect, destRect, UltralightColorWHITE);
      canvas_->set_blending_enabled(true);
    }
}

void BitmapTextureUltralight::updateContents(GraphicsLayer* sourceLayer, const IntRect& targetRect,
  const IntPoint& offset, float scale) {
  ProfiledZone;
  IntRect sourceRect(targetRect);
  sourceRect.setLocation(offset);
  ultralight::IntRect scissorRect = { sourceRect.x(), sourceRect.y(), sourceRect.maxX(), sourceRect.maxY() };
  canvas_->SetScissorRect(scissorRect);

  // Add 2 pixel buffer around drawn area to avoid artifacts
  sourceRect.expand(4, 4);
  sourceRect.move(-2, -2);

  // Clear rect by disabling blending and drawing a transparent quad.
  canvas_->set_scissor_enabled(true);
  canvas_->set_blending_enabled(false);
  canvas_->DrawRect(FloatRect(sourceRect), UltralightColorTRANSPARENT);
  canvas_->set_blending_enabled(true);

  sourceRect.scale(1 / scale);
  
  canvas_->Save();
  {
    GraphicsContextUltralight ctx(canvas_);
    ctx.applyDeviceScaleFactor(scale);

    sourceLayer->paintGraphicsLayerContents(ctx, sourceRect);
  }
  canvas_->Restore();

  canvas_->set_scissor_enabled(false);
}

void BitmapTextureUltralight::updateContents(const void*, const IntRect& target,
    const IntPoint& offset, int bytesPerLine) {
  ProfiledZone;
  // not implemented
}

RefPtr<BitmapTexture> BitmapTextureUltralight::applyFilters(TextureMapper&,
    const FilterOperations&, bool) {
  ProfiledZone;
  // not implemented
    return this;
}

} // namespace WebCore

#endif // USE(TEXTURE_MAPPER_ULTRALIGHT) 
