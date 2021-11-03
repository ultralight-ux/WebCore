#include "config.h"
#include "CanvasImage.h"
#include "GraphicsContext.h"
#include "IntSize.h"
#include <Ultralight/platform/Config.h>
#include <Ultralight/platform/Platform.h>

namespace WebCore {

CanvasImage::CanvasImage(const IntSize& size, bool isDeferred)
{
    m_surface = ultralight::GetBitmapSurfaceFactory()->CreateSurface(size.width(), size.height());
    m_canvas = ultralight::Canvas::Create(size.width(), size.height(), ultralight::BitmapFormat::BGRA8_UNORM_SRGB, m_surface);

    m_context.reset(new GraphicsContext(m_canvas));
    m_canvas->Clear();
}

CanvasImage::~CanvasImage()
{
    m_context.reset();
    m_canvas->FlushSurface();
    m_canvas = nullptr;
    ultralight::GetBitmapSurfaceFactory()->DestroySurface(m_surface);
}

void CanvasImage::computeIntrinsicDimensions(Length& intrinsicWidth, Length& intrinsicHeight, FloatSize& intrinsicRatio)
{
    Image::computeIntrinsicDimensions(intrinsicWidth, intrinsicHeight, intrinsicRatio);
    intrinsicRatio = FloatSize();
}

ImageDrawResult CanvasImage::draw(GraphicsContext& context, const FloatRect& dstRect, const FloatRect& srcRect, CompositeOperator op, BlendMode mode, DecodingMode decodingMode, ImageOrientationDescription orientation)
{
    context.save();
    context.setCompositeOperation(op, mode);
    ultralight::Paint paint;
    paint.color = UltralightColorWHITE;
    context.platformContext()->canvas()->DrawCanvas(m_canvas, srcRect, dstRect, paint);
    context.restore();

    return ImageDrawResult::DidDraw;
}

void CanvasImage::drawPattern(GraphicsContext& context, const FloatRect& destRect, const FloatRect& srcRect, const AffineTransform& patternTransform,
    const FloatPoint& phase, const FloatSize& spacing, CompositeOperator op, BlendMode blend)
{
    context.drawPattern(*this, destRect, srcRect, patternTransform, phase, spacing, op, blend);
}

} // namespace WebCore
