#include "config.h"
#include "CanvasImage.h"
#include "GraphicsContext.h"
#include "IntSize.h"

namespace WebCore {

CanvasImage::CanvasImage(const IntSize& size, bool isDeferred) {
  if (isDeferred)
    m_canvas = ultralight::Canvas::CreateRecorder(size.width(), size.height(), ultralight::kBitmapFormat_BGRA8_UNORM_SRGB);
  else
    m_canvas = ultralight::Canvas::Create(size.width(), size.height(), ultralight::kBitmapFormat_BGRA8_UNORM_SRGB);

  m_context.reset(new GraphicsContext(m_canvas));
  m_canvas->Clear();
}

CanvasImage::~CanvasImage() {
}

void CanvasImage::computeIntrinsicDimensions(Length& intrinsicWidth, Length& intrinsicHeight, FloatSize& intrinsicRatio) {
  Image::computeIntrinsicDimensions(intrinsicWidth, intrinsicHeight, intrinsicRatio);
  intrinsicRatio = FloatSize();
}

ImageDrawResult CanvasImage::draw(GraphicsContext& context, const FloatRect& dstRect, const FloatRect& srcRect, CompositeOperator op, BlendMode mode, DecodingMode decodingMode, ImageOrientationDescription orientation) {
  ultralight::RefPtr<ultralight::Canvas> srcCanvas;

  if (m_canvas->IsRecorder()) {
    if (!m_backingStore)
      m_backingStore = ultralight::Canvas::Create(m_canvas->width(), m_canvas->height(), m_canvas->format());

    // Apply pending draw calls to our backing store
    m_canvas->Replay(m_backingStore.get());
    srcCanvas = m_backingStore;
  } else {
    srcCanvas = m_canvas;
  }
  
  context.save();
  context.setCompositeOperation(op, mode);
  ultralight::Paint paint;
  paint.color = UltralightColorWHITE;
  ultralight::Rect src_uv = srcCanvas->render_target().uv_coords;
  context.platformContext()->canvas()->DrawCanvas(*srcCanvas, src_uv, dstRect, paint);
  context.restore();

  return ImageDrawResult::DidDraw;
}

void CanvasImage::drawPattern(GraphicsContext& context, const FloatRect& destRect, const FloatRect& srcRect, const AffineTransform& patternTransform,
  const FloatPoint& phase, const FloatSize& spacing, CompositeOperator op, BlendMode blend) {
  context.drawPattern(*this, destRect, srcRect, patternTransform, phase, spacing, op, blend);
}

} // namespace WebCore
