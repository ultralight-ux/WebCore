#include "config.h"
#include "GraphicsContext.h"

#if USE(ULTRALIGHT)

#include "PlatformContextUltralight.h"
#include "AffineTransform.h"
#include "DisplayListRecorder.h"
#include "FloatConversion.h"
#include "FloatRect.h"
#include "FloatRoundedRect.h"
#include "Font.h"
#include "GraphicsContextPlatformPrivateUltralight.h"
#include "ImageBuffer.h"
#include "IntRect.h"
#include "NotImplemented.h"
#include "Path.h"
#include "Pattern.h"
#include "PlatformContextUltralight.h"
#include "RefPtrUltralight.h"
#include "ShadowBlur.h"
#include "TransformationMatrix.h"
#include <Ultralight/private/Canvas.h>
#include <math.h>
#include <stdio.h>
#include <wtf/MathExtras.h>
//#include "WebCore/Path.h"
//#include "WebCore/BitmapImage.h"
#include "BitmapImage.h"
#include "platform/graphics/ultralight/CanvasImage.h"

namespace WebCore {

GraphicsContext::GraphicsContext(ultralight::RefPtr<ultralight::Canvas> ctx)
{
  if (!ctx)
    return;

  m_data = new GraphicsContextPlatformPrivateTopLevel(new PlatformContextUltralight(ctx));
}

void GraphicsContext::platformInit(PlatformGraphicsContext* platformContext)
{
  if (!platformContext)
    return;

  m_data = new GraphicsContextPlatformPrivate(platformContext);
}

void GraphicsContext::platformDestroy()
{
  delete m_data;
}

AffineTransform GraphicsContext::getCTM(IncludeDeviceScale) const
{
  if (paintingDisabled())
    return AffineTransform();

  if (isRecording()) {
    WTFLogAlways("GraphicsContext::getCTM() is not yet compatible with recording contexts.");
    return AffineTransform();
  }

  ultralight::RefPtr<ultralight::Canvas> canvas = platformContext()->canvas();
  ultralight::Matrix m = canvas->GetMatrix();
  return AffineTransform(m.data[0], m.data[1], m.data[2], m.data[3], m.data[4], m.data[5]);
}

PlatformContextUltralight* GraphicsContext::platformContext() const
{
  return m_data->platformContext;
}

void GraphicsContext::savePlatformState()
{
  ASSERT(!isRecording());
  platformContext()->save();
}

void GraphicsContext::restorePlatformState()
{
  ASSERT(!isRecording());
  platformContext()->restore();

  platformContext()->shadowBlur().setShadowValues(FloatSize(m_state.shadowBlur, m_state.shadowBlur),
    m_state.shadowOffset,
    m_state.shadowColor,
    m_state.shadowsIgnoreTransforms);
}

// Draws a filled rectangle with a stroked border.
void GraphicsContext::drawRect(const FloatRect& rect, float borderThickness)
{
  if (paintingDisabled())
    return;

  if (isRecording()) {
    m_displayListRecorder->drawRect(rect, borderThickness);
    return;
  }

  ASSERT(!rect.isEmpty());
  fillRect(rect, platformContext()->fillColor());
}

void GraphicsContext::drawNativeImage(const NativeImagePtr& image, const FloatSize& imageSize, const FloatRect& destRect, const FloatRect& srcRect, CompositeOperator op, BlendMode blendMode, ImageOrientation orientation)
{
  if (paintingDisabled())
    return;

  if (isRecording()) {
    m_displayListRecorder->drawNativeImage(image, imageSize, destRect, srcRect, op, blendMode, orientation);
    return;
  }

  // TODO
  notImplemented();
}

// This is only used to draw borders, so we should not draw shadows.
void GraphicsContext::drawLine(const FloatPoint& point1, const FloatPoint& point2)
{
  if (paintingDisabled())
    return;

  if (strokeStyle() == NoStroke)
    return;

  if (isRecording()) {
    m_displayListRecorder->drawLine(point1, point2);
    return;
  }

  // TODO
  notImplemented();
}

// This method is only used to draw the little circles used in lists.
void GraphicsContext::drawEllipse(const FloatRect& rect)
{
  if (paintingDisabled())
    return;

  if (isRecording()) {
    m_displayListRecorder->drawEllipse(rect);
    return;
  }

  ultralight::Paint paint;
  WebCore::Color color = fillColor();
  paint.color = UltralightRGBA(color.red(), color.green(), color.blue(), color.alpha());
  ultralight::RoundedRect rrect;
  rrect.rect = rect;
  float radius = rect.width() * 0.5;
  rrect.radii_x[0] = radius;
  rrect.radii_x[1] = radius;
  rrect.radii_x[2] = radius;
  rrect.radii_x[3] = radius;
  rrect.radii_y[0] = radius;
  rrect.radii_y[1] = radius;
  rrect.radii_y[2] = radius;
  rrect.radii_y[3] = radius;
  platformContext()->canvas()->DrawRoundedRect(rrect, paint, 0.0, UltralightColorTRANSPARENT);
}

void GraphicsContext::fillPath(const Path& path)
{
  if (paintingDisabled() || path.isEmpty())
    return;

  if (isRecording()) {
    m_displayListRecorder->fillPath(path);
    return;
  }

  // TODO
  //notImplemented();

  ultralight::Paint paint;
  WebCore::Color color = fillColor();
  paint.color = UltralightRGBA(color.red(), color.green(), color.blue(), color.alpha());
  platformContext()->canvas()->DrawPath(path.ultralightPath(), paint, true, false, 1.0);
}

void GraphicsContext::strokePath(const Path& path)
{
  if (paintingDisabled() || path.isEmpty())
    return;

  if (isRecording()) {
    m_displayListRecorder->strokePath(path);
    return;
  }

  ultralight::Paint paint;
  WebCore::Color color = strokeColor();
  paint.color = UltralightRGBA(color.red(), color.green(), color.blue(), color.alpha());
  platformContext()->canvas()->DrawPath(path.ultralightPath(), paint, false, true, strokeThickness());
}

void GraphicsContext::fillRect(const FloatRect& rect)
{
  if (paintingDisabled())
    return;

  if (isRecording()) {
    m_displayListRecorder->fillRect(rect);
    return;
  }

  // TODO
  notImplemented();
}

void GraphicsContext::fillRect(const FloatRect& rect, const Color& color)
{
  if (paintingDisabled())
    return;

  if (isRecording()) {
    m_displayListRecorder->fillRect(rect, color);
    return;
  }

  if (hasShadow()) {

    FloatSize shadowOffset;
    float shadowBlur;
    Color shadowColor;
    getShadow(shadowOffset, shadowBlur, shadowColor);

    ultralight::Paint paint;
    paint.color = UltralightRGBA(shadowColor.red(), shadowColor.green(), shadowColor.blue(), shadowColor.alpha());
    ultralight::RoundedRect rrect;
    rrect.SetEmpty();
    rrect.rect = rect;  

    // From WebKit's ShadowData::paintingExtent():
    //   Blurring uses a Gaussian function whose std. deviation is radius/2, and which in theory
    //   extends to infinity. In 8-bit contexts, however, rounding causes the effect to become
    //   undetectable at around 1.4x the radius.
    const float radiusExtentMultiplier = 1.4;
    int paintExtent = ceilf(shadowBlur * radiusExtentMultiplier);

    ultralight::Rect paintRect = rrect.rect;
    float padding = paintExtent;
    paintRect.Outset(padding, padding);
    paintRect.Move(shadowOffset.width(), shadowOffset.height());

    platformContext()->canvas()->DrawRoundedRectShadow(paintRect, rrect, false, { shadowOffset.width(), shadowOffset.height() }, shadowBlur, paint);
  }

  // TODO
  ultralight::Paint paint;
  paint.color = UltralightRGBA(color.red(), color.green(), color.blue(), color.alpha());
  platformContext()->canvas()->DrawRect(rect, paint);
}

void GraphicsContext::clip(const FloatRect& rect)
{
  if (paintingDisabled())
    return;

  if (isRecording()) {
    m_displayListRecorder->clip(rect);
    return;
  }

  platformContext()->canvas()->SetClip(rect, false);

  //fillRect(rect, makeRGBA(200, 10, 50, 128));
  //return;
  /*
  ultralight::Ref<ultralight::Matrix> cur_matrix = platformContext()->canvas()->GetTransform();
  ultralight::Rect r = rect;
  r = cur_matrix->Apply(r);

  float canvas_width = platformContext()->canvas()->width();
  float canvas_height = platformContext()->canvas()->height();

  // rect is now in orthographic coords ([-1, -1, 1, 1]), we need to convert:
  r.left = (r.left + 1.0) * 0.5 * canvas_width;
  r.top = (r.top + 1.0) * 0.5 * canvas_height;
  r.right = (r.right + 1.0) * 0.5 * canvas_width;
  r.bottom = (r.bottom + 1.0) * 0.5 * canvas_height;

  platformContext()->canvas()->SetClip(r);
  */
 // printf("SetClip(%f, %f, %f, %f)\n", r.left, r.top, r.right, r.bottom);
}

void GraphicsContext::clipRoundedRect(const FloatRoundedRect& rect)
{
  if (paintingDisabled())
    return;

  ultralight::RoundedRect rrect;
  rrect.rect = rect.rect();
  rrect.radii_x[0] = rect.radii().topLeft().width();
  rrect.radii_x[1] = rect.radii().topRight().width();
  rrect.radii_x[2] = rect.radii().bottomRight().width();
  rrect.radii_x[3] = rect.radii().bottomLeft().width();
  rrect.radii_y[0] = rect.radii().topLeft().height();
  rrect.radii_y[1] = rect.radii().topRight().height();
  rrect.radii_y[2] = rect.radii().bottomRight().height();
  rrect.radii_y[3] = rect.radii().bottomLeft().height();

  platformContext()->canvas()->SetClip(rrect, false);
}

void GraphicsContext::clipOutRoundedRect(const FloatRoundedRect& rect)
{
  if (paintingDisabled())
    return;

  if (!rect.isRounded()) {
    clipOut(rect.rect());
    return;
  }

  ultralight::RoundedRect rrect;
  rrect.rect = rect.rect();
  rrect.radii_x[0] = rect.radii().topLeft().width();
  rrect.radii_x[1] = rect.radii().topRight().width();
  rrect.radii_x[2] = rect.radii().bottomRight().width();
  rrect.radii_x[3] = rect.radii().bottomLeft().width();
  rrect.radii_y[0] = rect.radii().topLeft().height();
  rrect.radii_y[1] = rect.radii().topRight().height();
  rrect.radii_y[2] = rect.radii().bottomRight().height();
  rrect.radii_y[3] = rect.radii().bottomLeft().height();

  platformContext()->canvas()->SetClip(rrect, true);
}

IntRect GraphicsContext::clipBounds() const
{
  return IntRect(platformContext()->canvas()->GetClipBounds());
}

void GraphicsContext::clipPath(const Path& path, WindRule clipRule)
{
  if (paintingDisabled())
    return;

  if (isRecording()) {
    m_displayListRecorder->clipPath(path, clipRule);
    return;
  }

  ultralight::RefPtr<ultralight::Path> p = path.ultralightPath();
  ultralight::FillRule fill_rule = clipRule == RULE_NONZERO ? ultralight::kFillRule_NonZero : ultralight::kFillRule_EvenOdd;
  platformContext()->canvas()->SetClip(p, fill_rule, false);
}

void GraphicsContext::clipToImageBuffer(ImageBuffer& buffer, const FloatRect& destRect)
{
  if (paintingDisabled())
    return;

  // TODO
  notImplemented();
}

/*
IntRect GraphicsContext::clipBounds() const
{
  if (paintingDisabled())
    return IntRect();

  if (isRecording()) {
    WTFLogAlways("Getting the clip bounds not yet supported with display lists");
    return IntRect(-2048, -2048, 4096, 4096); // FIXME: display lists.
  }

  // TODO
  notImplemented();
}
*/

static inline void adjustFocusRingColor(Color& color)
{
#if !PLATFORM(GTK)
  // Force the alpha to 50%.  This matches what the Mac does with outline rings.
  color = Color(makeRGBA(color.red(), color.green(), color.blue(), 127));
#else
  UNUSED_PARAM(color);
#endif
}

static inline void adjustFocusRingLineWidth(float& width)
{
#if PLATFORM(GTK)
  width = 2;
#else
  UNUSED_PARAM(width);
#endif
}

static inline StrokeStyle focusRingStrokeStyle()
{
#if PLATFORM(GTK)
  return DottedStroke;
#else
  return SolidStroke;
#endif
}

void GraphicsContext::drawFocusRing(const Path& path, float width, float /* offset */, const Color& color)
{
  if (paintingDisabled())
    return;

  // FIXME: We should draw paths that describe a rectangle with rounded corners
  // so as to be consistent with how we draw rectangular focus rings.
  Color ringColor = color;
  adjustFocusRingColor(ringColor);
  adjustFocusRingLineWidth(width);

  // TODO
  notImplemented();
}

void GraphicsContext::drawFocusRing(const Vector<FloatRect>& rects, float width, float /* offset */, const Color& color)
{
  if (paintingDisabled())
    return;

  Path path;
#if PLATFORM(GTK)
  for (const auto& rect : rects)
    path.addRect(rect);
#else
  unsigned rectCount = rects.size();
  int radius = (width - 1) / 2;
  Path subPath;
  for (unsigned i = 0; i < rectCount; ++i) {
    if (i > 0)
      subPath.clear();
    subPath.addRoundedRect(rects[i], FloatSize(radius, radius));
    path.addPath(subPath, AffineTransform());
  }
#endif
  drawFocusRing(path, width, 0, color);
}

void GraphicsContext::drawLineForText(const FloatPoint& origin, float width, bool printing, bool doubleUnderlines, StrokeStyle)
{
  DashArray widths;
  widths.append(width);
  widths.append(0);
  drawLinesForText(origin, widths, printing, doubleUnderlines);
}

void GraphicsContext::drawLinesForText(const FloatPoint& point, const DashArray& widths, bool printing, bool doubleUnderlines, StrokeStyle strokeStyle)
{
  if (paintingDisabled())
    return;

  if (widths.size() <= 0)
    return;

  if (isRecording()) {
    m_displayListRecorder->drawLinesForText(point, widths, printing, doubleUnderlines, strokeThickness());
    return;
  }

  Color localStrokeColor(strokeColor());

  float thickness = std::max(strokeThickness(), 0.5f);

  FloatRect rect = FloatRect(point.x(), point.y() + thickness + 1.0f, widths[0], thickness);
  ultralight::Paint paint;
  WebCore::Color color = fillColor();
  paint.color = UltralightRGBA(color.red(), color.green(), color.blue(), color.alpha());
  platformContext()->canvas()->DrawRect(rect, paint);

  /*

  FloatRect bounds = computeLineBoundsAndAntialiasingModeForText(point, widths.last(), printing, localStrokeColor);

  Vector<FloatRect, 4> dashBounds;
  ASSERT(!(widths.size() % 2));
  dashBounds.reserveInitialCapacity(dashBounds.size() / 2);

  float dashWidth = 0;
  switch (strokeStyle) {
  case DottedStroke:
    dashWidth = bounds.height();
    break;
  case DashedStroke:
    dashWidth = 2 * bounds.height();
    break;
  case SolidStroke:
  default:
    break;
  }

  for (size_t i = 0; i < widths.size(); i += 2) {
    auto left = widths[i];
    auto width = widths[i + 1] - widths[i];
    if (!dashWidth)
      dashBounds.append(FloatRect(FloatPoint(bounds.x() + left, bounds.y()), FloatSize(width, bounds.height())));
    else {
      auto startParticle = static_cast<unsigned>(std::ceil(left / (2 * dashWidth)));
      auto endParticle = static_cast<unsigned>((left + width) / (2 * dashWidth));
      for (unsigned j = startParticle; j < endParticle; ++j)
        dashBounds.append(FloatRect(FloatPoint(bounds.x() + j * 2 * dashWidth, bounds.y()), FloatSize(dashWidth, bounds.height())));
    }
  }

  if (doubleUnderlines) {
    // The space between double underlines is equal to the height of the underline
    for (size_t i = 0; i < widths.size(); i += 2)
      dashBounds.append(FloatRect(FloatPoint(bounds.x() + widths[i], bounds.y() + 2 * bounds.height()), FloatSize(widths[i + 1] - widths[i], bounds.height())));
  }

  ultralight::Paint paint;
  WebCore::Color color = fillColor();
  paint.color = UltralightRGBA(color.red(), color.green(), color.blue(), color.alpha());

  for (auto& dash : dashBounds)
    platformContext()->canvas()->DrawRect(dash, paint);
    */
}

void GraphicsContext::updateDocumentMarkerResources()
{
  // Unnecessary, since our document markers don't use resources.
}

void GraphicsContext::drawLineForDocumentMarker(const FloatPoint& origin, float width, DocumentMarkerLineStyle style)
{
  if (paintingDisabled())
    return;

  // TODO
  notImplemented();
}

FloatRect GraphicsContext::roundToDevicePixels(const FloatRect& frect, RoundingMode)
{
  if (paintingDisabled())
    return frect;

  if (isRecording()) {
    WTFLogAlways("GraphicsContext::roundToDevicePixels() is not yet compatible with recording contexts.");
    return frect;
  }

  // TODO
  notImplemented();
  return frect;
}

void GraphicsContext::translate(float x, float y)
{
  if (paintingDisabled())
    return;

  if (isRecording()) {
    m_displayListRecorder->translate(x, y);
    return;
  }

  AffineTransform transform;
  transform.translate(x, y);

  platformContext()->canvas()->Transform(transform);
}

void GraphicsContext::setPlatformFillColor(const Color& color)
{
  if (paintingDisabled())
    return;

  ASSERT(!isRecording());

  platformContext()->setFillColor(color);
}

void GraphicsContext::setPlatformStrokeColor(const Color& color)
{
  if (paintingDisabled())
    return;

  ASSERT(!isRecording());

  platformContext()->setStrokeColor(color);
}

void GraphicsContext::setPlatformStrokeThickness(float strokeThickness)
{
  if (paintingDisabled())
    return;

  ASSERT(!isRecording());

  platformContext()->setStrokeThickness(strokeThickness);
}

/*
void GraphicsContext::setPlatformStrokeStyle(StrokeStyle strokeStyle)
{
  static const double dashPattern[] = { 5.0, 5.0 };
  static const double dotPattern[] = { 1.0, 1.0 };

  if (paintingDisabled())
    return;

  ASSERT(!isRecording());

  // TODO
  notImplemented();
}
*/

void GraphicsContext::setURLForRect(const URL&, const FloatRect&)
{
  notImplemented();
}

void GraphicsContext::concatCTM(const AffineTransform& transform)
{
  if (paintingDisabled())
    return;

  if (isRecording()) {
    m_displayListRecorder->concatCTM(transform);
    return;
  }

  platformContext()->canvas()->Transform(transform);
}

void GraphicsContext::setCTM(const AffineTransform& transform)
{
  if (paintingDisabled())
    return;

  if (isRecording()) {
    WTFLogAlways("GraphicsContext::setCTM() is not compatible with recording contexts.");
    return;
  }

  platformContext()->canvas()->SetMatrix(transform);
}

void GraphicsContext::setPlatformShadow(FloatSize const& size, float, Color const&)
{
  if (paintingDisabled())
    return;

  if (m_state.shadowsIgnoreTransforms) {
    // Meaning that this graphics context is associated with a CanvasRenderingContext
    // We flip the height since CG and HTML5 Canvas have opposite Y axis
    m_state.shadowOffset = FloatSize(size.width(), -size.height());
  }

  // Cairo doesn't support shadows natively, they are drawn manually in the draw* functions using ShadowBlur.
  platformContext()->shadowBlur().setShadowValues(FloatSize(m_state.shadowBlur, m_state.shadowBlur),
    m_state.shadowOffset,
    m_state.shadowColor,
    m_state.shadowsIgnoreTransforms);
}

void GraphicsContext::clearPlatformShadow()
{
  if (paintingDisabled())
    return;

  platformContext()->shadowBlur().clear();
}

void GraphicsContext::beginPlatformTransparencyLayer(float opacity)
{
  if (paintingDisabled())
    return;

  ASSERT(!isRecording());

  platformContext()->canvas()->BeginTransparencyLayer(opacity);
}

void GraphicsContext::endPlatformTransparencyLayer()
{
  if (paintingDisabled())
    return;

  ASSERT(!isRecording());

  platformContext()->canvas()->EndTransparencyLayer();

}

bool GraphicsContext::supportsTransparencyLayers()
{
  return true;
}

void GraphicsContext::clearRect(const FloatRect& rect)
{
  if (paintingDisabled())
    return;

  if (isRecording()) {
    m_displayListRecorder->clearRect(rect);
    return;
  }

  // TODO
  notImplemented();
}

void GraphicsContext::strokeRect(const FloatRect& rect, float width)
{
  if (paintingDisabled())
    return;

  if (isRecording()) {
    m_displayListRecorder->strokeRect(rect, width);
    return;
  }

  // TODO
  notImplemented();
}

void GraphicsContext::setLineCap(LineCap lineCap)
{
  if (paintingDisabled())
    return;

  if (isRecording()) {
    m_displayListRecorder->setLineCap(lineCap);
    return;
  }

  // TODO
  notImplemented();
}

static inline bool isDashArrayAllZero(const DashArray& dashes)
{
  for (auto& dash : dashes) {
    if (dash)
      return false;
  }
  return true;
}

void GraphicsContext::setLineDash(const DashArray& dashes, float dashOffset)
{
  if (paintingDisabled())
    return;

  if (isRecording()) {
    m_displayListRecorder->setLineDash(dashes, dashOffset);
    return;
  }

  // TODO
  notImplemented();
}

void GraphicsContext::setLineJoin(LineJoin lineJoin)
{
  if (paintingDisabled())
    return;

  if (isRecording()) {
    m_displayListRecorder->setLineJoin(lineJoin);
    return;
  }

  // TODO
  notImplemented();
}

void GraphicsContext::setMiterLimit(float miter)
{
  if (paintingDisabled())
    return;

  // TODO
  notImplemented();
}

void GraphicsContext::setPlatformAlpha(float alpha)
{
  platformContext()->setGlobalAlpha(alpha);
}

void GraphicsContext::setPlatformCompositeOperation(CompositeOperator op, BlendMode blendOp)
{
  if (paintingDisabled())
    return;

  platformContext()->setCompositeOperator(op);
  platformContext()->setBlendMode(blendOp);
}

void GraphicsContext::canvasClip(const Path& path, WindRule windRule)
{
  clipPath(path, windRule);
}

void GraphicsContext::clipOut(const Path& path)
{
  if (paintingDisabled())
    return;

  if (isRecording()) {
    m_displayListRecorder->clipOut(path);
    return;
  }

  ultralight::RefPtr<ultralight::Path> p = path.ultralightPath();
  // TODO: What is the fill rule? No winding specified.
  ultralight::FillRule fill_rule = ultralight::kFillRule_EvenOdd;
  platformContext()->canvas()->SetClip(p, fill_rule, true);
}

void GraphicsContext::rotate(float radians)
{
  if (paintingDisabled())
    return;

  if (isRecording()) {
    m_displayListRecorder->rotate(radians);
    return;
  }

  AffineTransform transform;
  transform.rotate(radians);

  platformContext()->canvas()->Transform(transform);
}

void GraphicsContext::scale(const FloatSize& size)
{
  if (paintingDisabled())
    return;

  if (isRecording()) {
    m_displayListRecorder->scale(size);
    return;
  }

  AffineTransform transform;
  transform.scale(size);

  platformContext()->canvas()->Transform(transform);
}

void GraphicsContext::clipOut(const FloatRect& r)
{
  if (paintingDisabled())
    return;

  if (isRecording()) {
    m_displayListRecorder->clipOut(r);
    return;
  }

  platformContext()->canvas()->SetClip(r, true);
}

void GraphicsContext::platformFillRoundedRect(const FloatRoundedRect& rect, const Color& color)
{
  if (paintingDisabled())
    return;

  ASSERT(!isRecording());

  // TODO: Handle shadows.
  // 
  if (hasShadow()) {

    FloatSize shadowOffset;
    float shadowBlur;
    Color shadowColor;
    getShadow(shadowOffset, shadowBlur, shadowColor);

    ultralight::Paint paint;
    paint.color = UltralightRGBA(shadowColor.red(), shadowColor.green(), shadowColor.blue(), shadowColor.alpha());
    ultralight::RoundedRect rrect;
    rrect.rect = rect.rect();
    rrect.radii_x[0] = rect.radii().topLeft().width();
    rrect.radii_x[1] = rect.radii().topRight().width();
    rrect.radii_x[2] = rect.radii().bottomRight().width();
    rrect.radii_x[3] = rect.radii().bottomLeft().width();
    rrect.radii_y[0] = rect.radii().topLeft().height();
    rrect.radii_y[1] = rect.radii().topRight().height();
    rrect.radii_y[2] = rect.radii().bottomRight().height();
    rrect.radii_y[3] = rect.radii().bottomLeft().height();

    // From WebKit's ShadowData::paintingExtent():
    //   Blurring uses a Gaussian function whose std. deviation is radius/2, and which in theory
    //   extends to infinity. In 8-bit contexts, however, rounding causes the effect to become
    //   undetectable at around 1.4x the radius.
    const float radiusExtentMultiplier = 1.4;
    int paintExtent = ceilf(shadowBlur * radiusExtentMultiplier);

    ultralight::Rect paintRect = rrect.rect;
    float padding = paintExtent;
    paintRect.Outset(padding, padding);
    paintRect.Move(shadowOffset.width(), shadowOffset.height());

    platformContext()->canvas()->DrawRoundedRectShadow(paintRect, rrect, false, { shadowOffset.width(), shadowOffset.height() }, shadowBlur, paint);
  }

  ultralight::Paint paint;
  paint.color = UltralightRGBA(color.red(), color.green(), color.blue(), color.alpha());
  ultralight::RoundedRect rrect;
  rrect.rect = rect.rect();
  rrect.radii_x[0] = rect.radii().topLeft().width();
  rrect.radii_x[1] = rect.radii().topRight().width();
  rrect.radii_x[2] = rect.radii().bottomRight().width();
  rrect.radii_x[3] = rect.radii().bottomLeft().width();
  rrect.radii_y[0] = rect.radii().topLeft().height();
  rrect.radii_y[1] = rect.radii().topRight().height();
  rrect.radii_y[2] = rect.radii().bottomRight().height();
  rrect.radii_y[3] = rect.radii().bottomLeft().height();
  platformContext()->canvas()->DrawRoundedRect(rrect, paint, 0.0, UltralightColorTRANSPARENT);
}

void GraphicsContext::fillRectWithRoundedHole(const FloatRect& rect, const FloatRoundedRect& roundedHoleRect, const Color& color)
{
  if (paintingDisabled() || !color.isValid())
    return;

  if (isRecording()) {
    m_displayListRecorder->fillRectWithRoundedHole(rect, roundedHoleRect, color);
    return;
  }

  // AFAIK, this is only called from RenderBoxModelObject::paintBoxShadow when
  // it wants to draw an inset shadow.

  if (hasShadow()) {

    FloatSize shadowOffset;
    float shadowBlur;
    Color shadowColor;
    getShadow(shadowOffset, shadowBlur, shadowColor);

    ultralight::Paint paint;
    paint.color = UltralightRGBA(shadowColor.red(), shadowColor.green(), shadowColor.blue(), shadowColor.alpha());
    ultralight::RoundedRect rrect;
    rrect.SetEmpty();
    rrect.rect = roundedHoleRect.rect();

    ultralight::Rect paint_rect = rect;
    paint_rect.Move(shadowOffset.width(), shadowOffset.height());

    platformContext()->canvas()->DrawRoundedRectShadow(paint_rect, rrect, true, { shadowOffset.width(), shadowOffset.height() }, shadowBlur, paint);
  }

  // TODO
  //notImplemented();

  return;

  /*
  if (this->mustUseShadowBlur())
    platformContext()->shadowBlur().drawInsetShadow(*this, rect, roundedHoleRect);

  Path path;
  path.addRect(rect);
  if (!roundedHoleRect.radii().isZero())
    path.addRoundedRect(roundedHoleRect);
  else
    path.addRect(roundedHoleRect.rect());
	*/
}

void GraphicsContext::drawPattern(Image& image, const FloatRect& destRect, const FloatRect& tileRect, const AffineTransform& patternTransform, const FloatPoint& phase, const FloatSize& spacing, CompositeOperator op, BlendMode blendMode)
{
  if (paintingDisabled())
    return;

  if (isRecording()) {
    m_displayListRecorder->drawPattern(image, destRect, tileRect, patternTransform, phase, spacing, op, blendMode);
    return;
  }

  // Avoid NaN
  if (!std::isfinite(phase.x()) || !std::isfinite(phase.y()))
    return;

  // if (tileRect.size() != image.size()) {
  // we need to crop image to tileRect and use that instead of image
  //}

  // Compute combined transform (patternTransform and phase)
  AffineTransform combined = patternTransform;
  combined.multiply(AffineTransform(1, 0, 0, 1,
    phase.x() + tileRect.x() * patternTransform.a(),
    phase.y() + tileRect.y() * patternTransform.d()));

  if (image.isBitmapImage())
  {
    BitmapImage& bitmap_image = (BitmapImage&)image;

    // We call this to ensure the frame is cached
    NativeImagePtr nativeImage = bitmap_image.nativeImageForCurrentFrame();
    if (!nativeImage)
      return;

    platformContext()->save();
    //platformContext()->setBlendMode(blendMode);
    //platformContext()->setCompositeOperator(op);

    ultralight::Rect dest = destRect;
    ultralight::Rect src = tileRect;
    platformContext()->canvas()->DrawPattern(
      nativeImage->first, nativeImage->second, src, dest, combined);

    platformContext()->restore();
  }
  else if (image.isCanvasImage()) {
    CanvasImage& canvas_image = (CanvasImage&)image;
    
    platformContext()->save();
    ultralight::Rect dest = destRect;
    ultralight::Rect src = tileRect;
    ultralight::Rect src_uv = { 0.0f, 0.0f, 1.0f, 1.0f };
    platformContext()->canvas()->DrawCanvasPattern(
      *canvas_image.canvas(), src_uv, src, dest, combined);

    platformContext()->restore();
  }
}

void GraphicsContext::setPlatformShouldAntialias(bool enable)
{
  if (paintingDisabled())
    return;

  ASSERT(!isRecording());

  platformContext()->setShouldAntialias(enable);
}

void GraphicsContext::setPlatformImageInterpolationQuality(InterpolationQuality quality)
{
  ASSERT(!isRecording());

  platformContext()->setImageInterpolationQuality(quality);
}

/*
bool GraphicsContext::isAcceleratedContext() const
{
  if (isRecording())
    return false;

  return true;
}
*/

#if ENABLE(3D_TRANSFORMS) && USE(TEXTURE_MAPPER)
TransformationMatrix GraphicsContext::get3DTransform() const
{
  // FIXME: Can we approximate the transformation better than this?
  return getCTM().toTransformationMatrix();
}

void GraphicsContext::concat3DTransform(const TransformationMatrix& transform)
{
  concatCTM(transform.toAffineTransform());
}

void GraphicsContext::set3DTransform(const TransformationMatrix& transform)
{
  setCTM(transform.toAffineTransform());
}
#endif // ENABLE(3D_TRANSFORMS) && USE(TEXTURE_MAPPER)

} // namespace WebCore

#endif // USE(ULTRALIGHT)
