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


  ASSERT(hasPlatformContext());
  ultralight::RefPtr<ultralight::Canvas> canvas = platformContext()->canvas();
  ultralight::Matrix m = canvas->GetMatrix();
  return AffineTransform(m.a(), m.b(), m.c(), m.d(), m.e(), m.f());
}

PlatformContextUltralight* GraphicsContext::platformContext() const
{
  return m_data->platformContext;
}

void GraphicsContext::savePlatformState()
{
  ASSERT(hasPlatformContext());
  platformContext()->save();
}

void GraphicsContext::restorePlatformState()
{
  ASSERT(hasPlatformContext());
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

  ASSERT(!rect.isEmpty());
  ASSERT(hasPlatformContext());
  fillRect(rect, platformContext()->fillColor());
  // TODO, handle stroked border
}

void GraphicsContext::drawNativeImage(const NativeImagePtr& image, const FloatSize& imageSize, const FloatRect& destRect, const FloatRect& srcRect, CompositeOperator op, BlendMode blendMode, ImageOrientation orientation)
{
  if (paintingDisabled())
    return;

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

  // TODO
  notImplemented();
}

// This method is only used to draw the little circles used in lists.
void GraphicsContext::drawEllipse(const FloatRect& rect)
{
  if (paintingDisabled())
    return;

  ASSERT(hasPlatformContext());
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

  ASSERT(hasPlatformContext());

  if (m_state.fillGradient) {
    platformContext()->save();
    platformContext()->setMask(path, fillRule());
    m_state.fillGradient->fill(*this, path.fastBoundingRect());
    platformContext()->restore();
    return;
  }

  ultralight::Paint paint;
  WebCore::Color color = fillColor();
  paint.color = UltralightRGBA(color.red(), color.green(), color.blue(), color.alpha());
  platformContext()->canvas()->FillPath(path.ultralightPath(), paint, 
    fillRule() == WindRule::NonZero ? ultralight::kFillRule_NonZero : ultralight::kFillRule_EvenOdd);
  // TODO, handle shadow state
}

void GraphicsContext::strokePath(const Path& path)
{
  if (paintingDisabled() || path.isEmpty())
    return;

  ASSERT(hasPlatformContext());

  ultralight::Paint paint;
  WebCore::Color color = strokeColor();
  paint.color = UltralightRGBA(color.red(), color.green(), color.blue(), color.alpha());

  ultralight::LineCap lineCap;
  switch (platformContext()->lineCap()) {
  case WebCore::LineCap::RoundCap:
    lineCap = ultralight::kLineCap_Round;
    break;
  case WebCore::LineCap::SquareCap:
    lineCap = ultralight::kLineCap_Square;
    break;
  case WebCore::LineCap::ButtCap:
  default:
    lineCap = ultralight::kLineCap_Butt;
  }

  ultralight::LineJoin lineJoin;
  switch (platformContext()->lineJoin()) {
  case WebCore::LineJoin::BevelJoin:
    lineJoin = ultralight::kLineJoin_Bevel;
    break;
  case WebCore::LineJoin::RoundJoin:
    lineJoin = ultralight::kLineJoin_Round;
    break;
  case WebCore::LineJoin::MiterJoin:
  default:
    lineJoin = ultralight::kLineJoin_Miter;
  }

  platformContext()->canvas()->StrokePath(path.ultralightPath(), paint, strokeThickness(),
    lineCap, lineJoin, platformContext()->miterLimit());

  // TODO, handle shadow state
}

void GraphicsContext::fillRect(const FloatRect& rect)
{
  if (m_state.fillGradient) {
    platformContext()->save();
    m_state.fillGradient->fill(*this, rect);
    platformContext()->restore();
    return;
  }

  fillRect(rect, fillColor());
}

void GraphicsContext::fillRect(const FloatRect& rect, const Color& color)
{
  if (paintingDisabled())
    return;

  ASSERT(hasPlatformContext());

  if (hasShadow()) {
    ShadowBlur contextShadow(m_state);
    contextShadow.drawRectShadow(*this, FloatRoundedRect(rect));
    /*
    FloatSize shadowOffset;
    float shadowBlur;
    Color shadowColor;
    getShadow(shadowOffset, shadowBlur, shadowColor);

    platformContext()->DrawBoxShadow(FloatRoundedRect(rect), shadowOffset, shadowBlur, shadowColor, false, FloatRoundedRect(rect));
    */
  }

  if (color.isVisible()) {
    ultralight::Paint paint;
    paint.color = UltralightRGBA(color.red(), color.green(), color.blue(), color.alpha());
    platformContext()->canvas()->DrawRect(rect, paint);
  }
}

void GraphicsContext::clip(const FloatRect& rect)
{
  if (paintingDisabled())
    return;

  ASSERT(hasPlatformContext());
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

  ASSERT(hasPlatformContext());
  platformContext()->setMask(path, clipRule);
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
  color = Color(makeRGBA(0, 150, 255, 200));
}

static inline void adjustFocusRingLineWidth(float& width)
{
  width = 4;
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
  
  if (width <= 0.1 || path.isEmpty())
    return;

  ASSERT(hasPlatformContext());
  ultralight::Paint paint;
  paint.color = UltralightRGBA(ringColor.red(), ringColor.green(), ringColor.blue(), ringColor.alpha());
  platformContext()->canvas()->StrokePath(path.ultralightPath(), paint, width * 0.5 / scaleFactor().width());
}

void GraphicsContext::drawFocusRing(const Vector<FloatRect>& rects, float width, float /* offset */, const Color& color)
{
  if (paintingDisabled())
    return;

  unsigned rectCount = rects.size();
  int radius = (width - 1) / 2;

  if (rectCount == 0)
    return;

  // Determine the bounding rectangle of all the rects and use that
  // to draw the rounded focus ring path.
  // NOTE: Other ports are drawing every rect here which seems incorrect?
  FloatRect boundingRect = rects[0];
  for (unsigned i = 1; i < rectCount; ++i) 
    boundingRect.unite(rects[i]);

  Path path;
  path.addRoundedRect(boundingRect, FloatSize(radius, radius));
  drawFocusRing(path, width, 0, color);
}

void GraphicsContext::drawLineForText(const FloatRect& rect, bool printing, bool doubleUnderlines, StrokeStyle)
{
  drawLinesForText(rect.location(), rect.height(), DashArray{ 0, rect.width() }, printing, doubleUnderlines);
}

void GraphicsContext::drawLinesForText(const FloatPoint& point, float thickness, const DashArray& widths, bool printing, bool doubleUnderlines, StrokeStyle strokeStyle)
{
  if (paintingDisabled())
    return;

  if (widths.isEmpty())
    return;

  Color localStrokeColor(strokeColor());

  FloatRect bounds = computeLineBoundsAndAntialiasingModeForText(FloatRect(point, FloatSize(widths.last(), thickness)), printing, localStrokeColor);

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
  WebCore::Color color = localStrokeColor;
  paint.color = UltralightRGBA(color.red(), color.green(), color.blue(), color.alpha());

  for (auto& dash : dashBounds)
    platformContext()->canvas()->DrawRect(dash, paint);
}

void GraphicsContext::drawDotsForDocumentMarker(const FloatRect&, DocumentMarkerLineStyle)
{
	// TODO
	notImplemented();
}

FloatRect GraphicsContext::roundToDevicePixels(const FloatRect& frect, RoundingMode)
{
  if (paintingDisabled())
    return frect;

  // TODO
  notImplemented();
  return frect;
}

void GraphicsContext::translate(float x, float y)
{
  if (paintingDisabled())
    return;

  ASSERT(hasPlatformContext());
  AffineTransform transform;
  transform.translate(x, y);

  platformContext()->canvas()->Transform(transform);
}

void GraphicsContext::platformApplyDeviceScaleFactor(float scale)
{
  if (paintingDisabled())
    return;

  ASSERT(hasPlatformContext());
  platformContext()->canvas()->SetDeviceScaleHint(scale);
}

void GraphicsContext::setPlatformFillColor(const Color& color)
{
  if (paintingDisabled())
    return;

  ASSERT(hasPlatformContext());

  platformContext()->setFillColor(color);
}

void GraphicsContext::setPlatformStrokeColor(const Color& color)
{
  if (paintingDisabled())
    return;

  ASSERT(hasPlatformContext());

  platformContext()->setStrokeColor(color);
}

void GraphicsContext::setPlatformStrokeThickness(float strokeThickness)
{
  if (paintingDisabled())
    return;

  ASSERT(hasPlatformContext());

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

  ASSERT(hasPlatformContext());
  platformContext()->canvas()->Transform(transform);
}

void GraphicsContext::setCTM(const AffineTransform& transform)
{
  if (paintingDisabled())
    return;

  ASSERT(hasPlatformContext());
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

  ASSERT(hasPlatformContext());
  platformContext()->shadowBlur().setShadowValues(FloatSize(m_state.shadowBlur, m_state.shadowBlur),
    m_state.shadowOffset,
    m_state.shadowColor,
    m_state.shadowsIgnoreTransforms);
}

void GraphicsContext::clearPlatformShadow()
{
  if (paintingDisabled())
    return;

  ASSERT(hasPlatformContext());
  platformContext()->shadowBlur().clear();
}

void GraphicsContext::beginPlatformTransparencyLayer(float opacity)
{
  if (paintingDisabled())
    return;

  ASSERT(hasPlatformContext());
  platformContext()->canvas()->BeginTransparencyLayer(opacity);
}

void GraphicsContext::endPlatformTransparencyLayer()
{
  if (paintingDisabled())
    return;

  ASSERT(hasPlatformContext());
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

  ASSERT(hasPlatformContext());

  auto canvas = platformContext()->canvas();
  canvas->Save();

  ultralight::Matrix mat = canvas->GetMatrix();
  ultralight::Rect aabb = rect;
  aabb = mat.Apply(aabb);

  canvas->set_scissor_enabled(true);
  ultralight::IntRect scissorRect = { (int)aabb.left, (int)aabb.top, (int)ceilf(aabb.right), (int)ceilf(aabb.bottom) };
  canvas->SetScissorRect(scissorRect);

  // Add 2 pixel buffer around drawn area to avoid artifacts
  aabb.Outset(2.0f, 2.0f);

  // aabb is in pixel coordinates, reset matrix
  ultralight::Matrix identity_mat;
  identity_mat.SetIdentity();
  canvas->SetMatrix(identity_mat);

  // Clear rect by disabling blending and drawing a transparent quad.
  canvas->set_blending_enabled(false);
  ultralight::Paint paint;
  paint.color = UltralightColorTRANSPARENT;
  canvas->DrawRect(aabb, paint);
  canvas->set_blending_enabled(true);
  canvas->set_scissor_enabled(false);

  canvas->Restore();
}

void GraphicsContext::strokeRect(const FloatRect& rect, float width)
{
  if (paintingDisabled())
    return;

  ASSERT(hasPlatformContext());

  if (width < 0.001)
    return;

  if (rect.isEmpty() || rect.isZero())
    return;

  Path path;
  path.addRect(rect);

  ultralight::Paint paint;
  WebCore::Color color = strokeColor();
  paint.color = UltralightRGBA(color.red(), color.green(), color.blue(), color.alpha());
  platformContext()->canvas()->StrokePath(path.ultralightPath(), paint, width);
  // TODO, handle shadow state
}

void GraphicsContext::setLineCap(LineCap lineCap)
{
  if (paintingDisabled())
    return;

  platformContext()->setLineCap(lineCap);
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

  // TODO
  notImplemented();
}

void GraphicsContext::setLineJoin(LineJoin lineJoin)
{
  if (paintingDisabled())
    return;

  platformContext()->setLineJoin(lineJoin);
}

void GraphicsContext::setMiterLimit(float miter)
{
  if (paintingDisabled())
    return;

  platformContext()->setMiterLimit(miter);
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

  ultralight::RefPtr<ultralight::Path> p = path.ultralightPath();
  // CoreGraphics seems to use EvenOdd rule here so we do the same.
  ultralight::FillRule fill_rule = ultralight::kFillRule_EvenOdd;
  platformContext()->canvas()->SetClip(p, fill_rule, true);
}

void GraphicsContext::rotate(float radians)
{
  if (paintingDisabled())
    return;

  double cosAngle = cos(radians);
  double sinAngle = sin(radians);
  AffineTransform transform(cosAngle, sinAngle, -sinAngle, cosAngle, 0, 0);

  platformContext()->canvas()->Transform(transform);
}

void GraphicsContext::scale(const FloatSize& size)
{
  if (paintingDisabled())
    return;

  AffineTransform transform;
  transform.scale(size);

  platformContext()->canvas()->Transform(transform);
}

void GraphicsContext::clipOut(const FloatRect& r)
{
  if (paintingDisabled())
    return;

  platformContext()->canvas()->SetClip(r, true);
}

void GraphicsContext::platformFillRoundedRect(const FloatRoundedRect& rect, const Color& color)
{
  if (paintingDisabled())
    return;

  ASSERT(hasPlatformContext());

  if (hasShadow()) {
      ShadowBlur contextShadow(m_state);
      contextShadow.drawRectShadow(*this, rect);
      /*
    FloatSize shadowOffset;
    float shadowBlur;
    Color shadowColor;
    getShadow(shadowOffset, shadowBlur, shadowColor);

    platformContext()->DrawBoxShadow(rect, shadowOffset, shadowBlur, shadowColor, false, rect);
    */
  }

  if (color.isVisible()) {
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
}

void GraphicsContext::fillRectWithRoundedHole(const FloatRect& rect, const FloatRoundedRect& roundedHoleRect, const Color& color)
{
    if (paintingDisabled())
        return;

    if (m_impl) {
        m_impl->fillRectWithRoundedHole(rect, roundedHoleRect, color);
        return;
    }

    Path path;
    path.addRect(rect);

    if (!roundedHoleRect.radii().isZero())
        path.addRoundedRect(roundedHoleRect);
    else
        path.addRect(roundedHoleRect.rect());

    WindRule oldFillRule = fillRule();
    Color oldFillColor = fillColor();

    setFillRule(WindRule::EvenOdd);
    setFillColor(color);

    if (hasShadow()) {
        ShadowBlur contextShadow(m_state);
        contextShadow.drawInsetShadow(*this, rect, roundedHoleRect);
    }

    fillPath(path);

    setFillRule(oldFillRule);
    setFillColor(oldFillColor);
}

void GraphicsContext::drawPattern(Image& image, const FloatRect& destRect, const FloatRect& tileRect, const AffineTransform& patternTransform, const FloatPoint& phase, const FloatSize& spacing, CompositeOperator op, BlendMode blendMode)
{
  if (paintingDisabled())
    return;

  ASSERT(hasPlatformContext());

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
    ultralight::Rect src_uv = canvas_image.canvas()->render_target().uv_coords;
    platformContext()->canvas()->DrawCanvasPattern(
      canvas_image.canvas(), src_uv, src, dest, combined);

    platformContext()->restore();
  }
}

void GraphicsContext::setPlatformShouldAntialias(bool enable)
{
  if (paintingDisabled())
    return;

  ASSERT(hasPlatformContext());
  platformContext()->setShouldAntialias(enable);
}

void GraphicsContext::setPlatformImageInterpolationQuality(InterpolationQuality quality)
{
  ASSERT(hasPlatformContext());
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

#if ENABLE(3D_TRANSFORMS) && USE(TEXTURE_MAPPER) && 0
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
