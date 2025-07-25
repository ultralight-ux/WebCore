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
#include "NotImplemented.h"

namespace WebCore {

BitmapTextureUltralight* toBitmapTextureUL(BitmapTexture* texture)
{
    if (!texture || !texture->isBackedByUltralight())
        return 0;

    return static_cast<BitmapTextureUltralight*>(texture);
}

BitmapTextureUltralight::BitmapTextureUltralight(TextureMapper* textureMapper, bool use_gpu, const BitmapTexture::Flags flags) 
  : textureMapper_(textureMapper), use_gpu_(use_gpu), owns_canvas_(true), clip_stack_(IntRect()) {
}

BitmapTextureUltralight::BitmapTextureUltralight(TextureMapper* textureMapper, ultralight::RefPtr<ultralight::Canvas> canvas) 
  : textureMapper_(textureMapper), owns_canvas_(false), canvas_(canvas), clip_stack_(IntRect(0, 0, canvas->width(), canvas->height())) {
  canvas_size_ = IntSize(canvas->width(), canvas->height());
  use_gpu_ = canvas_->surface() == nullptr;
}

BitmapTextureUltralight::BitmapTextureUltralight(TextureMapper* textureMapper, const IntSize& paddedSize,
    const IntSize& contentSize, bool use_gpu, const BitmapTexture::Flags flags) 
  : textureMapper_(textureMapper), use_gpu_(use_gpu), owns_canvas_(true), clip_stack_(IntRect(IntPoint(), contentSize)) {
  filter_info_ = FilterInfo();
  m_contentSize = contentSize;
  m_flags = flags;
  resetCanvas(paddedSize);
}

BitmapTextureUltralight::~BitmapTextureUltralight() {
  if (canvas_ && owns_canvas_) {
    canvas_->RecycleRenderTexture();
    canvas_ = nullptr;
  }
}

void BitmapTextureUltralight::applyClip() {
  ProfiledZone;
  if (!canvas_)
    return;

  clipStack().applyClip(canvas_);
}

void BitmapTextureUltralight::didReset() {
  ProfiledZone;
  if (!owns_canvas_)
    return;

  IntSize targetSize = contentSize();
  filter_info_ = FilterInfo();
  clipStack().reset(IntRect(IntPoint(), targetSize));

  if (canvas_) {
    if(targetSize.width() > canvas_->width() || targetSize.height() > canvas_->height()) {
      // Resize the canvas if the target size is larger than the current size
      canvas_size_ = targetSize;
      canvas_->Resize(canvas_size_.width(), canvas_size_.height());
    }
      
    canvas_->Clear();
    return;
  }

  // Create a new canvas with the target size
  resetCanvas(targetSize);
}

void BitmapTextureUltralight::updateContents(Image* image,
    const IntRect& targetRect, const IntPoint& offset) {
    ProfiledZone;
    if (!image || !canvas_)
      return;

    if (image->isBitmapImage()) {
      RefPtr<NativeImage> frameImage = image->nativeImageForCurrentFrame();
      if (!frameImage)
        return;

      IntSize imageSize = frameImage->size();

      // Calculate source rectangle from the image, taking offset into account
      // offset represents how far into the image we should start
      ultralight::Rect srcRect = { 
          (float)offset.x(), 
          (float)offset.y(), 
          (float)offset.x() + targetRect.width(), 
          (float)offset.y() + targetRect.height() 
      };

      // Make sure we don't exceed the image bounds
      if (srcRect.right > imageSize.width())
          srcRect.right = imageSize.width();
      if (srcRect.bottom > imageSize.height())
          srcRect.bottom = imageSize.height();

      // Target rectangle in the texture's coordinates
      ultralight::Rect destRect = { 
          (float)targetRect.x(), 
          (float)targetRect.y(),
          (float)targetRect.maxX(), 
          (float)targetRect.maxY() 
      };

      // Draw the image to the canvas
      canvas_->set_blending_enabled(false);
      canvas_->DrawImage(frameImage->platformImage(), srcRect, destRect, UltralightColorWHITE);
      canvas_->set_blending_enabled(true);
    }
}

void BitmapTextureUltralight::updateContents(GraphicsLayer* sourceLayer, const IntRect& targetRect,
  const IntPoint& offset, float scale) {
  ProfiledZone;
  if (!canvas_ || !sourceLayer)
    return;
  
  // Amount of padding to add around the target rect (used to prevent artifacts when doing partial draws)
  int pad = 2;

  // Set scissor rect to the target area
  ultralight::IntRect scissorRect = { 
      targetRect.x(), 
      targetRect.y(), 
      targetRect.maxX(), 
      targetRect.maxY() 
  };

  canvas_->SetScissorRect(scissorRect);
  canvas_->set_scissor_enabled(true);

  IntRect paddedTargetRect = targetRect;
  paddedTargetRect.inflate(pad);

  // Following BitmapTexture.cpp's reference implementation:
  // Calculate the source rectangle in the layer's coordinate space

  FloatRect sourceRect(targetRect);
  sourceRect.setLocation(offset);
  sourceRect.inflate(pad);
  sourceRect.scale(1.0f / scale);
  
  // Clear the target area
  canvas_->set_blending_enabled(false);
  canvas_->DrawRect(FloatRect(paddedTargetRect), UltralightColorTRANSPARENT);
  canvas_->set_blending_enabled(true);
  
  canvas_->Save();
  {
    GraphicsContextUltralight ctx(canvas_);
    
    // Translate to target rect origin
    ctx.translate(paddedTargetRect.x(), paddedTargetRect.y());

    // Apply scale factor
    ctx.applyDeviceScaleFactor(scale);
    
    // Translate to offset the source rectangle's position
    ctx.translate(-sourceRect.x(), -sourceRect.y());
    
    // Paint the layer contents
    sourceLayer->paintGraphicsLayerContents(ctx, sourceRect);
  }
  canvas_->Restore();
  
  canvas_->set_scissor_enabled(false);
}

void BitmapTextureUltralight::updateContents(const void* data, const IntRect& target,
    const IntPoint& offset, int bytesPerLine) {
  ProfiledZone;
  if (!data || !canvas_)
    return;
    
  // Create a temporary bitmap with the source data
  // We need to calculate proper dimensions for the bitmap
  int bpp = 4; // Assuming BGRA8 format (32-bits per pixel)
  int width = target.width();
  int height = target.height();
  
  // Create a bitmap that wraps the existing pixel data (no copy)
  auto bitmap = ultralight::Bitmap::Create(width, height, 
      ultralight::BitmapFormat::BGRA8_UNORM_SRGB, 
      bytesPerLine, data, bytesPerLine * height, 
      nullptr, nullptr); // No destruction callback needed, we don't own the data
      
  if (!bitmap)
    return;
    
  // Create an image from the bitmap
  auto image = ultralight::Image::Create(bitmap, true);
  if (!image)
    return;
  
  // Calculate source and destination rectangles
  ultralight::Rect srcRect = { 
      (float)offset.x(), 
      (float)offset.y(), 
      (float)offset.x() + target.width(),
      (float)offset.y() + target.height() 
  };
  
  ultralight::Rect destRect = { 
      (float)target.x(), 
      (float)target.y(),
      (float)target.maxX(), 
      (float)target.maxY() 
  };
  
  // Draw the image to our canvas
  canvas_->set_blending_enabled(false);
  canvas_->DrawImage(image, srcRect, destRect, UltralightColorWHITE);
  canvas_->set_blending_enabled(true);
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
  ProfiledZone;
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
    ProfiledZone;
    if (filters.isEmpty())
      return this;

    TextureMapperUltralight& texmapUL = static_cast<TextureMapperUltralight&>(textureMapper);
    RefPtr<BitmapTexture> previousSurface = texmapUL.currentSurface();
    RefPtr<BitmapTexture> resultSurface = this;
    RefPtr<BitmapTexture> intermediateSurface;
    RefPtr<BitmapTexture> contentSurface;

    filter_info_ = FilterInfo();

    //defersLastFilter = false;

#if 0
    for (size_t i = 0; i < filters.size(); ++i) {

        RefPtr<FilterOperation> filter = filters.operations()[i];
        ASSERT(filter);

        bool last = (i == filters.size() - 1);
        if (defersLastFilter && last) {
            toBitmapTextureUL(resultSurface.get())->filter_info_ = BitmapTextureUltralight::FilterInfo(filter.copyRef());
            break;
        }

        intermediateSurface = texmapUL.drawFiltered(resultSurface, contentSize(), filter);
        std::swap(resultSurface, intermediateSurface);
    }
#else

    intermediateSurface = texmapUL.acquireTextureFromPool(contentSize(), BitmapTexture::SupportsAlpha, false);

    for (size_t i = 0; i < filters.size(); ++i) {
        RefPtr<FilterOperation> filter = filters.operations()[i];
        ASSERT(filter);

        int numPasses = TextureMapperUltralight::getPassesRequiredForFilter(filter, use_gpu());

        float scale = TextureMapperUltralight::getScaleRequiredForFilter(filter, contentSize(), texmapUL.scale(), use_gpu());
            
        bool isDropShadow = filter->type() == FilterOperation::Type::DropShadow;
        bool isBlur = filter->type() == FilterOperation::Type::Blur;
        bool needsExactSize = isDropShadow || isBlur;
        bool needsCustomScale = scale != 1.0f;

        IntSize scaledSize = contentSize();

        if (isDropShadow) {
            auto& shadow = static_cast<const DropShadowFilterOperation&>(*filter);

            // We need to save the original content texture and draw it over the result surface
            contentSurface = resultSurface;

            resultSurface = texmapUL.acquireTextureFromPool(contentSize(), BitmapTexture::SupportsAlpha, false);

            scaledSize = IntSize((int)std::ceil(contentSize().width() * scale), (int)std::ceil(contentSize().height() * scale));

            // When we use a custom scale, we first draw the content to a temporary surface at the custom scale.
            //intermediateSurface = texmapUL.acquireTextureFromPool(scaledSize, BitmapTexture::SupportsAlpha, needsExactSize);
            texmapUL.bindSurface(intermediateSurface.get());
            texmapUL.drawTextureWithScale(*contentSurface.get(), contentSize(), scaledSize, FloatPoint(), false);
            std::swap(resultSurface, intermediateSurface);

            for (int j = 0; j < numPasses; ++j) {
                //intermediateSurface = texmapUL.acquireTextureFromPool(contentSize(), BitmapTexture::SupportsAlpha, false);
                texmapUL.bindSurface(intermediateSurface.get());
                texmapUL.drawFiltered(*resultSurface.get(), scaledSize, nullptr, filter, j, scale, false);
                std::swap(resultSurface, intermediateSurface);
            }

            //FloatPoint dstOffset = shadow.location();
            //dstOffset.scale((float)texmapUL.scale());
            FloatPoint dstOffset = FloatPoint();

            // We then draw the result to a full-sized surface at the original scale.
            //intermediateSurface = texmapUL.acquireTextureFromPool(contentSize(), BitmapTexture::SupportsAlpha, needsExactSize);
            texmapUL.bindSurface(intermediateSurface.get());
            texmapUL.drawTextureWithScale(*resultSurface.get(), scaledSize, contentSize(), dstOffset, false);

            // Finally, we draw the original content texture over the result surface at the original scale.
            texmapUL.drawTextureWithScale(*contentSurface.get(), contentSize(), contentSize(), FloatPoint(), true);

            std::swap(resultSurface, intermediateSurface);
        } else if (needsCustomScale) {
            scaledSize = IntSize((int)std::ceil(contentSize().width() * scale), (int)std::ceil(contentSize().height() * scale));
            // When we use a custom scale, we first draw the content to a temporary surface at the custom scale.
            texmapUL.bindSurface(intermediateSurface.get());
            texmapUL.drawTextureWithScale(*resultSurface.get(), contentSize(), scaledSize, FloatPoint(), false);

            //resultSurface = texmapUL.acquireTextureFromPool(contentSize(), BitmapTexture::SupportsAlpha, false);

            std::swap(resultSurface, intermediateSurface);
                
            // We then apply the filter to the temporary surface.
            for (int j = 0; j < numPasses; ++j) {
                texmapUL.bindSurface(intermediateSurface.get());
                texmapUL.drawFiltered(*resultSurface.get(), scaledSize, nullptr, filter, j, scale, false);
                std::swap(resultSurface, intermediateSurface);
            }

            // We then draw the result to a full-sized surface at the original scale.
            texmapUL.bindSurface(intermediateSurface.get());
            texmapUL.drawTextureWithScale(*resultSurface.get(), scaledSize, contentSize(), FloatPoint(), false);

            std::swap(resultSurface, intermediateSurface);

        } else {
            for (int j = 0; j < numPasses; ++j) {
                bool last = (i == filters.size() - 1) && (j == numPasses - 1);
                if (defersLastFilter && last && !contentSurface) {
                    toBitmapTextureUL(resultSurface.get())->filter_info_ = BitmapTextureUltralight::FilterInfo(filter.copyRef(), j, nullptr);
                    break;
                }

                texmapUL.bindSurface(intermediateSurface.get());
                texmapUL.drawFiltered(*resultSurface.get(), contentSize(), nullptr, filter, j, 1.0f, false);
                std::swap(resultSurface, intermediateSurface);
            }
        }
    }

    texmapUL.bindSurface(previousSurface.get());
#endif

    return resultSurface;
}

void BitmapTextureUltralight::resetCanvas(const IntSize& size) {
  ProfiledZone;
  canvas_size_ = size;

  if (!use_gpu_) {
    // The underlying Surface is guaranteed to be a BitmapSurface.
    auto bitmapSurfaceFactory = ultralight::GetBitmapSurfaceFactory();
    surface_.reset(bitmapSurfaceFactory->CreateSurface(canvas_size_.width(), canvas_size_.height()));
  }
    
  canvas_ = ultralight::Canvas::Create(canvas_size_.width(),
      canvas_size_.height(), ultralight::BitmapFormat::BGRA8_UNORM_SRGB, surface_.get());
}

} // namespace WebCore

#endif // USE(TEXTURE_MAPPER_ULTRALIGHT) 
