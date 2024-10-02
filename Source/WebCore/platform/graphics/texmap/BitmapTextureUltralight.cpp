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

BitmapTextureUltralight* toBitmapTextureUL(BitmapTexture* texture)
{
    if (!texture || !texture->isBackedByUltralight())
        return 0;

    return static_cast<BitmapTextureUltralight*>(texture);
}

BitmapTextureUltralight::BitmapTextureUltralight(bool use_gpu, const BitmapTexture::Flags flags) : use_gpu_(use_gpu), owns_canvas_(true) {
}

BitmapTextureUltralight::BitmapTextureUltralight(ultralight::RefPtr<ultralight::Canvas> canvas) : canvas_(canvas), owns_canvas_(false) {
  canvas_size_ = IntSize(canvas->width(), canvas->height());
  use_gpu_ = canvas_->surface() == nullptr;
}

BitmapTextureUltralight::~BitmapTextureUltralight() {
  if (canvas_ && owns_canvas_) {
    canvas_->RecycleRenderTexture();
    canvas_ = nullptr;
  }
}

void BitmapTextureUltralight::applyClipIfNeeded(const ClipStackUltralight& clip) {
  if (!canvas_)
    return;

  if (clip_hash_ == clip.clipHash())
    return;

  if (clip_applied_)
    canvas_->Restore();

  canvas_->Save();

  clip.applyClip(canvas_);

  clip_hash_ = clip.clipHash();
  clip_applied_ = true;
}

void BitmapTextureUltralight::didReset() {
  ProfiledZone;
  if (!owns_canvas_)
    return;

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

static unsigned getPassesRequiredForFilter(FilterOperation::Type type)
{
    switch (type) {
    case FilterOperation::Type::Grayscale:
    case FilterOperation::Type::Sepia:
    case FilterOperation::Type::Saturate:
    case FilterOperation::Type::HueRotate:
    case FilterOperation::Type::Invert:
    case FilterOperation::Type::Brightness:
    case FilterOperation::Type::Contrast:
    case FilterOperation::Type::Opacity:
        return 1;
    case FilterOperation::Type::Blur:
    case FilterOperation::Type::DropShadow:
        // We use two-passes (vertical+horizontal) for blur and drop-shadow.
        return 2;
    default:
        return 0;
    }
}

RefPtr<BitmapTexture> BitmapTextureUltralight::applyFilters(TextureMapper& textureMapper,
  const FilterOperations& filters, bool defersLastFilter)
{
    ProfiledZone;
    if (filters.isEmpty())
      return this;

    if (use_gpu_) {
      TextureMapperUltralight& texmapUL = static_cast<TextureMapperUltralight&>(textureMapper);
      RefPtr<BitmapTexture> previousSurface = texmapUL.currentSurface();
      RefPtr<BitmapTexture> resultSurface = this;
      RefPtr<BitmapTexture> intermediateSurface;
      RefPtr<BitmapTexture> spareSurface;

      filter_info_ = FilterInfo();

      for (size_t i = 0; i < filters.size(); ++i) {
          RefPtr<FilterOperation> filter = filters.operations()[i];
          ASSERT(filter);

          int numPasses = getPassesRequiredForFilter(filter->type());
          for (int j = 0; j < numPasses; ++j) {
              bool last = (i == filters.size() - 1) && (j == numPasses - 1);
              if (defersLastFilter && last) {
                  toBitmapTextureUL(resultSurface.get())->filter_info_ = BitmapTextureUltralight::FilterInfo(filter.copyRef(), j, spareSurface.copyRef());
                  break;
              }

              if (!intermediateSurface)
                  intermediateSurface = texmapUL.acquireTextureFromPool(contentSize(), BitmapTexture::SupportsAlpha);
              texmapUL.bindSurface(intermediateSurface.get());
              texmapUL.drawFiltered(*resultSurface.get(), spareSurface.get(), *filter, j);
              if (!j && filter->type() == FilterOperation::Type::DropShadow) {
                  spareSurface = resultSurface;
                  resultSurface = nullptr;
              }
              std::swap(resultSurface, intermediateSurface);
          }
      }

      texmapUL.bindSurface(previousSurface.get());
      return resultSurface;
    }

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
