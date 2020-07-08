#include "config.h"
#include "CanvasImage.h"
#include "GraphicsContext.h"
#include "IntSize.h"
#include <Ultralight/platform/Platform.h>
#include <Ultralight/platform/Config.h>

namespace WebCore {

CanvasImage::CanvasImage(const IntSize& size, bool isDeferred) {
  m_surface = ultralight::GetBitmapSurfaceFactory()->CreateSurface(size.width(), size.height());
  m_canvas = ultralight::Canvas::Create(size.width(), size.height(), ultralight::kBitmapFormat_BGRA8_UNORM_SRGB, m_surface);

  m_context.reset(new GraphicsContext(m_canvas));
  m_canvas->Clear();
}

CanvasImage::~CanvasImage() {
  m_context.reset();
  m_canvas->FlushSurface();
  m_canvas = nullptr;
  ultralight::GetBitmapSurfaceFactory()->DestroySurface(m_surface);
}

void CanvasImage::computeIntrinsicDimensions(Length& intrinsicWidth, Length& intrinsicHeight, FloatSize& intrinsicRatio) {
  Image::computeIntrinsicDimensions(intrinsicWidth, intrinsicHeight, intrinsicRatio);
  intrinsicRatio = FloatSize();
}

ImageDrawResult CanvasImage::draw(GraphicsContext& context, const FloatRect& dstRect, const FloatRect& srcRect, CompositeOperator op, BlendMode mode, DecodingMode decodingMode, ImageOrientationDescription orientation) {
  bool snap_to_pixels = false;
  IntPoint pixel_coords;

  ultralight::Matrix mat = context.platformContext()->canvas()->GetMatrix();
  if (mat.IsSimple()) {
    // Transformation is simple (translation and/or uniform scale).
    // Let's check if that uniform scale matches our device scale.
    // If so, we will snap to pixel coordinates
    float scale = mat.a();
    float device_scale = ultralight::Platform::instance().config().device_scale;
    if (fabsf(scale - device_scale) < 0.01f) {
      ultralight::Rect aabb = mat.Apply(dstRect);
      if (fabsf(aabb.width() - m_surface->width()) < 2.0f &&
        fabsf(aabb.height() - m_surface->height()) < 2.0f) {
        snap_to_pixels = true;
        pixel_coords = { (int)(aabb.x() + 0.5), (int)(aabb.y() + 0.5) };
      }
    }
  }

  context.save();
  context.setCompositeOperation(op, mode);
  ultralight::Paint paint;
  paint.color = UltralightColorWHITE;
  ultralight::Rect src_uv = m_canvas->render_target().uv_coords;

  if (snap_to_pixels) {
    ultralight::Matrix identity_mat;
    identity_mat.SetIdentity();
    context.platformContext()->canvas()->SetMatrix(identity_mat);
    ultralight::Rect dest = { (float)pixel_coords.x(), (float)pixel_coords.y(), (float)pixel_coords.x() + m_surface->width(), (float)pixel_coords.y() + m_surface->height() };
    context.platformContext()->canvas()->DrawCanvas(*m_canvas, src_uv, dest, paint);
  }
  else {
    context.platformContext()->canvas()->DrawCanvas(*m_canvas, src_uv, dstRect, paint);
  }
  context.restore();

  return ImageDrawResult::DidDraw;
}

void CanvasImage::drawPattern(GraphicsContext& context, const FloatRect& destRect, const FloatRect& srcRect, const AffineTransform& patternTransform,
  const FloatPoint& phase, const FloatSize& spacing, CompositeOperator op, BlendMode blend) {
  context.drawPattern(*this, destRect, srcRect, patternTransform, phase, spacing, op, blend);
}

} // namespace WebCore
