#include "config.h"
#include "BitmapTextureUltralight.h"

#if USE(TEXTURE_MAPPER_ULTRALIGHT) 

#include "FilterOperations.h"
#include "GraphicsContextUltralight.h"
#include "TextureMapperUltralight.h"
#include "Image.h"
#include "GraphicsLayer.h"
#include "TextureMapper.h"
#include <Ultralight/platform/Platform.h>
#include <Ultralight/platform/Config.h>
#include "FilterUltralight.h"
#include "FilterImage.h"
#include "FilterResults.h"
#include "ImageBufferUltralightBackend.h"

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

inline ultralight::RefPtr<ultralight::Bitmap> ImageBufferToBitmap(ImageBuffer& imageBuffer) {
  auto imageBufferBackend = static_cast<ImageBufferUltralightBackend*>(imageBuffer.backend());
  auto bitmap = imageBufferBackend->bitmap();
  return bitmap;
}

inline ultralight::RefPtr<ultralight::Bitmap> BitmapTextureToBitmap(BitmapTexture& bitmapTexture) {
  auto bitmapTextureUltralight = static_cast<BitmapTextureUltralight*>(&bitmapTexture);
  auto canvas = bitmapTextureUltralight->canvas();
  canvas->FlushSurface();
  auto bitmap = static_cast<ultralight::BitmapSurface*>(canvas->surface())->bitmap();
  return bitmap;
}

inline GraphicsContextUltralight BitmapTextureToGraphicsContext(BitmapTexture& bitmapTexture) {
  auto bitmapTextureUltralight = static_cast<BitmapTextureUltralight*>(&bitmapTexture);
  auto canvas = bitmapTextureUltralight->canvas();
  return GraphicsContextUltralight(canvas);
}

inline void CopyBitmaps(ultralight::RefPtr<ultralight::Bitmap> src, ultralight::RefPtr<ultralight::Bitmap> dst) {
  ultralight::IntRect srcRect = { 0, 0, (int)src->width(), (int)src->height() };
  ultralight::IntRect dstRect = { 0, 0, (int)dst->width(), (int)dst->height() };
  if (srcRect.Intersects(dstRect)) {
    auto rect = srcRect.Intersect(dstRect);
    dst->DrawBitmap(rect, rect, src, false);
  }
}

RefPtr<BitmapTexture> BitmapTextureUltralight::applyFilters(TextureMapper& textureMapper,
  const FilterOperations& filters, bool defersLastFilter)
{
    // TODO: Implement GPU pipeline for filters
    if (use_gpu_)
      return this;

    ProfiledZone;
    if (filters.isEmpty())
      return this;

    TextureMapperUltralight& texmapUL = static_cast<TextureMapperUltralight&>(textureMapper);
    RefPtr<BitmapTexture> dstSurface = texmapUL.acquireTextureFromPool(contentSize(), BitmapTexture::SupportsAlpha);

    GraphicsContextUltralight ctx = BitmapTextureToGraphicsContext(*dstSurface);

    double scale = texmapUL.scale();

    FloatRect boundingBox = FloatRect(FloatPoint(), contentSize());

    auto filter = FilterUltralight::create(filters, OptionSet<FilterRenderingMode> { FilterRenderingMode::Software },
        FloatSize(scale, scale), Filter::ClipOperation::Intersect, boundingBox, ctx);

    auto imageBuffer = ImageBuffer::create(boundingBox.size(), RenderingPurpose::LayerBacking, 1.0, DestinationColorSpace::SRGB(), PixelFormat::BGRA8);

    CopyBitmaps(BitmapTextureToBitmap(*this), ImageBufferToBitmap(*imageBuffer));

    FilterResults results;
    IntRect absoluteSourceImageRect = enclosingIntRect(boundingBox);
    auto filterImage = FilterImage::create(boundingBox, boundingBox, absoluteSourceImageRect, *imageBuffer, results.allocator());

    auto resultImage = filter->apply(filterImage.get(), results);

    if (!resultImage)
      return this;

    auto resultImageBuffer = resultImage->imageBuffer();
    CopyBitmaps(ImageBufferToBitmap(*resultImageBuffer), BitmapTextureToBitmap(*dstSurface));

    return dstSurface;
}

} // namespace WebCore

#endif // USE(TEXTURE_MAPPER_ULTRALIGHT) 
