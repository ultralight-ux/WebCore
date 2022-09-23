#include "config.h"
#include "PlatformContextUltralight.h"
#include "NotImplemented.h"
#include <iostream>
#include "rendering/BorderEdge.h"
#include "rendering/style/BorderData.h"

namespace WebCore {

  // Encapsulates the additional painting state information we store for each
  // pushed graphics state.
  class PlatformContextUltralight::State {
  public:
    State()
      : m_shouldAntialias(true)
      , m_imageInterpolationQuality(InterpolationDefault)
      , m_fillColor(Color::black)
      , m_strokeColor(Color::black)
      , m_strokeThickness(1.0f)
      , m_lineCap(LineCap::ButtCap)
      , m_lineJoin(LineJoin::MiterJoin)
      , m_lineDashOffset(0.0f)
      , m_miterLimit(10.0f)
    {
    }

    State(const State& state)
      : m_shouldAntialias(state.m_shouldAntialias)
      , m_imageInterpolationQuality(state.m_imageInterpolationQuality)
      , m_fillColor(state.m_fillColor)
      , m_strokeColor(state.m_strokeColor)
      , m_strokeThickness(state.m_strokeThickness)
      , m_lineCap(state.m_lineCap)
      , m_lineJoin(state.m_lineJoin)
      , m_lineDash(state.m_lineDash)
      , m_lineDashOffset(state.m_lineDashOffset)
      , m_miterLimit(state.m_miterLimit)
    {
    }

    bool m_shouldAntialias;
    InterpolationQuality m_imageInterpolationQuality;
    Color m_fillColor;
    Color m_strokeColor;
    float m_strokeThickness;
    LineCap m_lineCap;
    LineJoin m_lineJoin;
    DashArray m_lineDash;
    float m_lineDashOffset;
    float m_miterLimit;
  };

PlatformContextUltralight::PlatformContextUltralight(PlatformCanvas canvas) : m_canvas(canvas)
{
  m_stateStack.append(State());
  m_state = &m_stateStack.last();
}

PlatformContextUltralight::~PlatformContextUltralight()
{
}

PlatformCanvas PlatformContextUltralight::canvas() {
  return m_canvas;
}

void PlatformContextUltralight::save()
{
  m_stateStack.append(State(*m_state));
  m_state = &m_stateStack.last();

  m_canvas->Save();
}

void PlatformContextUltralight::restore()
{
  m_canvas->Restore();

  m_stateStack.removeLast();
  ASSERT(!m_stateStack.isEmpty());
  m_state = &m_stateStack.last();  
}

void PlatformContextUltralight::setGlobalAlpha(float globalAlpha)
{
  m_canvas->SetAlpha(globalAlpha);
}

float PlatformContextUltralight::globalAlpha() const
{
  return m_canvas->alpha();
}

void PlatformContextUltralight::setShouldAntialias(bool enable) {
  m_state->m_shouldAntialias = enable;
}

bool PlatformContextUltralight::shouldAntialias() const {
  return m_state->m_shouldAntialias;
}

void PlatformContextUltralight::setCompositeOperator(CompositeOperator op) {
  m_canvas->SetCompositeOp((ultralight::CompositeOp)op);
}

CompositeOperator PlatformContextUltralight::compositeOperator() const {
  return (CompositeOperator)m_canvas->composite_op();
}

void PlatformContextUltralight::setBlendMode(BlendMode mode) {
  m_canvas->SetBlendMode((ultralight::BlendMode)mode);
}

BlendMode PlatformContextUltralight::blendMode() const {
  return (BlendMode)m_canvas->blend_mode();
}

void PlatformContextUltralight::setFillColor(const Color& color)
{
  m_state->m_fillColor = color;
}

Color PlatformContextUltralight::fillColor() const
{
  return m_state->m_fillColor;
}

void PlatformContextUltralight::setStrokeColor(const Color& color)
{
  m_state->m_strokeColor = color;
}

Color PlatformContextUltralight::strokeColor() const
{
  return m_state->m_strokeColor;
}

void PlatformContextUltralight::setStrokeThickness(float thickness)
{
  m_state->m_strokeThickness = thickness;
}

float PlatformContextUltralight::strokeThickness() const {
  return m_state->m_strokeThickness;
}

void PlatformContextUltralight::setLineCap(LineCap lineCap) {
  m_state->m_lineCap = lineCap;
}

LineCap PlatformContextUltralight::lineCap() const {
  return m_state->m_lineCap;
}

void PlatformContextUltralight::setLineJoin(LineJoin lineJoin) {
  m_state->m_lineJoin = lineJoin;
}

LineJoin PlatformContextUltralight::lineJoin() const {
  return m_state->m_lineJoin;
}

void PlatformContextUltralight::setLineDash(const DashArray& dashes, float dashOffset) {
  m_state->m_lineDash = dashes;
  m_state->m_lineDashOffset = dashOffset;
}

float* PlatformContextUltralight::lineDashData() const {
  return m_state->m_lineDash.data();
}

size_t PlatformContextUltralight::lineDashSize() const {
  return m_state->m_lineDash.size();
}

float PlatformContextUltralight::lineDashOffset() const {
  return m_state->m_lineDashOffset;
}

void PlatformContextUltralight::setMiterLimit(float miter) {
  m_state->m_miterLimit = miter;
}

float PlatformContextUltralight::miterLimit() const {
  return m_state->m_miterLimit;
}

void PlatformContextUltralight::setImageInterpolationQuality(InterpolationQuality)
{
  // TODO
  notImplemented();
}

InterpolationQuality PlatformContextUltralight::imageInterpolationQuality() const
{
  // TODO
  notImplemented();
  return InterpolationDefault;
}

void PlatformContextUltralight::prepareForFilling(const GraphicsContextState&, PatternAdjustment)
{
  // TODO
  notImplemented();
}

void PlatformContextUltralight::prepareForStroking(const GraphicsContextState&, AlphaPreservation)
{
  // TODO
  notImplemented();
}

void PlatformContextUltralight::setMask(const Path& path, WindRule clipRule) {
  ultralight::RefPtr<ultralight::Path> p = path.ultralightPath();
  ultralight::FillRule fill_rule = clipRule == WindRule::NonZero ? ultralight::kFillRule_NonZero : ultralight::kFillRule_EvenOdd;
  canvas()->SetClip(p, fill_rule, false);
}

ultralight::BorderPaint GetBorderPaint(const BorderEdge& edge) {
  ultralight::BorderPaint result;
  Color c = edge.color();
  result.color = UltralightRGBA(c.red(), c.green(), c.blue(), c.alpha());

  switch (edge.style()) {
  case BorderStyle::Inset:
    result.style = ultralight::kBorderStyle_Inset;
    break;
  case BorderStyle::Groove:
    result.style = ultralight::kBorderStyle_Groove;
    break;
  case BorderStyle::Outset:
    result.style = ultralight::kBorderStyle_Outset;
    break;
  case BorderStyle::Ridge:
    result.style = ultralight::kBorderStyle_Ridge;
    break;
  case BorderStyle::Dotted:
    result.style = ultralight::kBorderStyle_Dotted;
    break;
  case BorderStyle::Dashed:
    result.style = ultralight::kBorderStyle_Dashed;
    break;
  case BorderStyle::Solid:
    result.style = ultralight::kBorderStyle_Solid;
    break;
  case BorderStyle::Double:
    result.style = ultralight::kBorderStyle_Double;
    break;
  default:
    result.style = ultralight::kBorderStyle_None;
    break;
  }

  return result;
}

void PlatformContextUltralight::DrawBoxDecorations(const FloatRect& layout_rect, const FloatRoundedRect& outer_rrect,
  const FloatRoundedRect& inner_rrect, const BorderEdge* edges, Color fill_color) {
  ultralight::RoundedRect rrect1;
  FloatRect r = outer_rrect.rect();
  rrect1.rect = { r.x(), r.y(), r.x() + r.width(), r.y() + r.height() };
  rrect1.radii_x[0] = outer_rrect.radii().topLeft().width();
  rrect1.radii_x[1] = outer_rrect.radii().topRight().width();
  rrect1.radii_x[2] = outer_rrect.radii().bottomRight().width();
  rrect1.radii_x[3] = outer_rrect.radii().bottomLeft().width();
  rrect1.radii_y[0] = outer_rrect.radii().topLeft().height();
  rrect1.radii_y[1] = outer_rrect.radii().topRight().height();
  rrect1.radii_y[2] = outer_rrect.radii().bottomRight().height();
  rrect1.radii_y[3] = outer_rrect.radii().bottomLeft().height();

  ultralight::RoundedRect rrect2;
  r = inner_rrect.rect();
  rrect2.rect = { r.x(), r.y(), r.x() + r.width(), r.y() + r.height() };
  rrect2.radii_x[0] = inner_rrect.radii().topLeft().width();
  rrect2.radii_x[1] = inner_rrect.radii().topRight().width();
  rrect2.radii_x[2] = inner_rrect.radii().bottomRight().width();
  rrect2.radii_x[3] = inner_rrect.radii().bottomLeft().width();
  rrect2.radii_y[0] = inner_rrect.radii().topLeft().height();
  rrect2.radii_y[1] = inner_rrect.radii().topRight().height();
  rrect2.radii_y[2] = inner_rrect.radii().bottomRight().height();
  rrect2.radii_y[3] = inner_rrect.radii().bottomLeft().height();

  ultralight::BorderPaint border_top = GetBorderPaint(edges[0]);
  ultralight::BorderPaint border_right = GetBorderPaint(edges[1]);
  ultralight::BorderPaint border_bottom = GetBorderPaint(edges[2]);
  ultralight::BorderPaint border_left = GetBorderPaint(edges[3]);

  if (border_top.style == ultralight::kBorderStyle_None)
    border_top.color = UltralightColorTRANSPARENT;
  if (border_right.style == ultralight::kBorderStyle_None)
    border_right.color = UltralightColorTRANSPARENT;
  if (border_bottom.style == ultralight::kBorderStyle_None)
    border_bottom.color = UltralightColorTRANSPARENT;
  if (border_left.style == ultralight::kBorderStyle_None)
    border_left.color = UltralightColorTRANSPARENT;

  ultralight::Color bgColor = UltralightRGBA(fill_color.red(), fill_color.green(), fill_color.blue(), fill_color.alpha());

  m_canvas->DrawBoxDecorations(layout_rect, rrect1, rrect2, border_top, border_right, border_bottom, border_left, bgColor);
}

inline ultralight::RoundedRect ToRoundedRect(const FloatRoundedRect& rrect) {
  ultralight::RoundedRect result;
  FloatRect r = rrect.rect();
  result.rect = { r.x(), r.y(), r.x() + r.width(), r.y() + r.height() };
  result.radii_x[0] = rrect.radii().topLeft().width();
  result.radii_x[1] = rrect.radii().topRight().width();
  result.radii_x[2] = rrect.radii().bottomRight().width();
  result.radii_x[3] = rrect.radii().bottomLeft().width();
  result.radii_y[0] = rrect.radii().topLeft().height();
  result.radii_y[1] = rrect.radii().topRight().height();
  result.radii_y[2] = rrect.radii().bottomRight().height();
  result.radii_y[3] = rrect.radii().bottomLeft().height();
  return result;
}

inline ultralight::Color ToColor(const Color& color) {
  return UltralightRGBA(color.red(), color.green(), color.blue(), color.alpha());
}

void PlatformContextUltralight::DrawRoundedRect(const FloatRect& layout_rect, const FloatRoundedRect& rrect,
  Color fill_color, float stroke_width, Color stroke_color) {
  ultralight::Paint paint;
  paint.color = ToColor(fill_color);
  m_canvas->DrawRoundedRect(ToRoundedRect(rrect), paint, stroke_width, ToColor(stroke_color));
}

void PlatformContextUltralight::DrawBoxShadow(const FloatRoundedRect& rect, const FloatSize& shadowOffset, float shadowBlur,
  const Color& shadowColor, bool insetShadow, const FloatRoundedRect& clip_rect) {
  // From WebKit's ShadowData::paintingExtent():
  //   Blurring uses a Gaussian function whose std. deviation is radius/2, and which in theory
  //   extends to infinity. In 8-bit contexts, however, rounding causes the effect to become
  //   undetectable at around 1.4x the radius.
  const float radiusExtentMultiplier = 1.4;
  int paintExtent = ceilf(shadowBlur * radiusExtentMultiplier);

  ultralight::Rect paintRect = rect.rect();
  float padding = paintExtent;
  paintRect.Outset(padding, padding);
  paintRect.Move(shadowOffset.width(), shadowOffset.height());

  ultralight::Paint paint;
  paint.color = ToColor(shadowColor);
  m_canvas->DrawBoxShadow(paintRect, ToRoundedRect(rect), ToRoundedRect(clip_rect), insetShadow,
    { shadowOffset.width(), shadowOffset.height() }, shadowBlur, paint);
}

void PlatformContextUltralight::clipForPatternFilling(const GraphicsContextState&)
{
  // TODO
  notImplemented();
}

} // namespace WebCore
