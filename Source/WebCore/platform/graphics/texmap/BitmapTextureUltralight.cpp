#include "config.h"
#include "BitmapTextureUltralight.h"

#if USE(TEXTURE_MAPPER_ULTRALIGHT) 

#include "FilterOperations.h"
#include "GraphicsContext.h"
#include "Image.h"
#include "CanvasImage.h"

namespace WebCore {

BitmapTextureUltralight::BitmapTextureUltralight(const BitmapTexture::Flags flags) {
}

BitmapTextureUltralight::~BitmapTextureUltralight() {
}

void BitmapTextureUltralight::didReset() {
    if (canvas_ && canvas_size_ == contentSize()) {
        canvas_->Clear();
        return;
    }

    canvas_size_ = contentSize();
    canvas_ = ultralight::Canvas::Create(canvas_size_.width(),
        canvas_size_.height(), ultralight::kBitmapFormat_RGBA8);
}

void BitmapTextureUltralight::updateContents(Image* image,
    const IntRect& targetRect, const IntPoint& offset,
    UpdateContentsFlag updateContentsFlag) {
    if (!image)
        return;

    if (image->isCanvasImage()) {
      ultralight::RefPtr<ultralight::Canvas> imageCanvas = static_cast<CanvasImage*>(image)->canvas();
      ultralight::Rect src_uv = imageCanvas->render_target().uv_coords;
      ultralight::Rect dest = { (float)targetRect.x(), (float)targetRect.y(),
        (float)targetRect.maxX(), (float)targetRect.maxY() };
      ultralight::Paint paint;
      paint.color = UltralightRGBA(255, 255, 255, 255);

      // TODO: handle offset

      canvas_->DrawCanvas(*imageCanvas, src_uv, dest, paint);

      paint.color = UltralightColorRED;

      canvas_->DrawRect({ 10, 10, 20, 20 }, paint);
      //return;
    } else if (image->isBitmapImage()) {
      NativeImagePtr frameImage = image->nativeImageForCurrentFrame();

      IntSize imageSize = nativeImageSize(frameImage);

      ultralight::Rect srcRect = { 0.0f, 0.0f, (float)imageSize.width(),
          (float)imageSize.height() };
      srcRect.Move(offset.x(), offset.y());

      ultralight::Rect destRect = { (float)targetRect.x(), (float)targetRect.y(),
        (float)targetRect.maxX(), (float)targetRect.maxY() };

      ultralight::Paint paint;
      paint.color = UltralightColorWHITE;
      canvas_->DrawImage(frameImage->first, frameImage->second, srcRect,
        destRect, paint);
    }
}

void BitmapTextureUltralight::updateContents(const void*, const IntRect& target,
    const IntPoint& offset, int bytesPerLine, UpdateContentsFlag) {
  // not implemented
}

PassRefPtr<BitmapTexture> BitmapTextureUltralight::applyFilters(TextureMapper&,
    const FilterOperations&) {
    // not implemented
    return this;
}

} // namespace WebCore

#endif // USE(TEXTURE_MAPPER_ULTRALIGHT) 
