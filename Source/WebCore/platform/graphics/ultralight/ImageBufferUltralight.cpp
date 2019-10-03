#pragma once

#include "config.h"
#include "ImageBuffer.h"

#if USE(ULTRALIGHT)

#include "BitmapImage.h"
#include "NotImplemented.h"
#include "GraphicsContext.h"
#include <runtime/JSCInlines.h>
#include <runtime/TypedArrayInlines.h>
#include <Ultralight/private/Canvas.h>
#include "platform/graphics/ultralight/CanvasImage.h"

using namespace std;

namespace WebCore {

ImageBufferData::ImageBufferData(const IntSize& size, RenderingMode renderingMode, bool isDeferred)
  : m_size(size)
  , m_image(CanvasImage::create(size, isDeferred))
{
}

ImageBufferData::~ImageBufferData()
{
}
  
ImageBuffer::ImageBuffer(const FloatSize& size, float /* resolutionScale */, ColorSpace, RenderingMode renderingMode, bool isDeferred, bool& success)
  : m_data(IntSize(size), renderingMode, isDeferred)
  , m_size(size)
  , m_logicalSize(size)
{
  success = false;  // Make early return mean error.
  if (m_size.isEmpty())
    return;

  success = true;
}

ImageBuffer::~ImageBuffer()
{
}

GraphicsContext& ImageBuffer::context() const
{
  return *m_data.m_image->m_context;
}

RefPtr<Image> ImageBuffer::sinkIntoImage(std::unique_ptr<ImageBuffer> imageBuffer, ScaleBehavior scaleBehavior)
{
  return imageBuffer->copyImage(DontCopyBackingStore, scaleBehavior);
}

RefPtr<Image> ImageBuffer::copyImage(BackingStoreCopy copyBehavior, ScaleBehavior) const
{
  // TODO
  //notImplemented();
  //return RefPtr<Image>();
  return m_data.m_image;
}

BackingStoreCopy ImageBuffer::fastCopyImageMode()
{
  return DontCopyBackingStore;
}

void ImageBuffer::drawConsuming(std::unique_ptr<ImageBuffer> imageBuffer, GraphicsContext& destContext, const FloatRect& destRect, const FloatRect& srcRect, CompositeOperator op, BlendMode blendMode)
{
  imageBuffer->draw(destContext, destRect, srcRect, op, blendMode);
}

void ImageBuffer::draw(GraphicsContext& context, const FloatRect& destRect, const FloatRect& srcRect,
  CompositeOperator op, BlendMode blendMode)
{
  m_data.m_image->draw(context, destRect, srcRect, op, blendMode, ImageOrientationDescription());
}

void ImageBuffer::drawPattern(GraphicsContext& context, const FloatRect& destRect, const FloatRect& srcRect, const AffineTransform& patternTransform,
  const FloatPoint& phase, const FloatSize& spacing, CompositeOperator op, BlendMode blend)
{
  m_data.m_image->drawPattern(context, destRect, srcRect, patternTransform, phase, spacing, op, blend);
}

void ImageBuffer::platformTransformColorSpace(const Vector<int>& lookUpTable)
{
  // TODO
  notImplemented();
}

RefPtr<Uint8ClampedArray> ImageBuffer::getUnmultipliedImageData(const IntRect&, CoordinateSystem) const {
  // TODO
  notImplemented();

  return nullptr;
}

RefPtr<Uint8ClampedArray> ImageBuffer::getPremultipliedImageData(const IntRect&, CoordinateSystem) const {
  // TODO
  notImplemented();

  return nullptr;
}

void ImageBuffer::putByteArray(Multiply multiplied, Uint8ClampedArray* source, const IntSize& sourceSize, const IntRect& sourceRect, const IntPoint& destPoint, CoordinateSystem)
{
  // TODO
  notImplemented();
}

String ImageBuffer::toDataURL(const String& mimeType, std::optional<double> quality, CoordinateSystem) const
{
  // TODO
  notImplemented();

  return String();
}

#if ENABLE(ACCELERATED_2D_CANVAS) && !USE(COORDINATED_GRAPHICS_THREADED)
void ImageBufferData::paintToTextureMapper(TextureMapper& textureMapper, const FloatRect& targetRect, const TransformationMatrix& matrix, float opacity)
{
  notImplemented();
}
#endif

} // namespace WebCore

#endif // USE(ULTRALIGHT)
