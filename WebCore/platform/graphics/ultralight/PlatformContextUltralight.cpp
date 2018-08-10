#include "config.h"
#include "PlatformContextUltralight.h"
#include "NotImplemented.h"
#include <iostream>

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
      , m_strokeThickness(1.0)
    {
    }

    State(const State& state)
      : m_shouldAntialias(state.m_shouldAntialias)
      , m_imageInterpolationQuality(state.m_imageInterpolationQuality)
      , m_fillColor(state.m_fillColor)
      , m_strokeColor(state.m_strokeColor)
      , m_strokeThickness(state.m_strokeThickness)
    {
    }

    bool m_shouldAntialias;
    InterpolationQuality m_imageInterpolationQuality;
    Color m_fillColor;
    Color m_strokeColor;
    float m_strokeThickness;
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
  notImplemented();
  //m_canvas->SetOpacity(globalAlpha);
}

float PlatformContextUltralight::globalAlpha() const
{
  notImplemented();
  return 1.0f;
  //return m_canvas->GetOpacity();
}

void PlatformContextUltralight::setShouldAntialias(bool enable) {
  m_state->m_shouldAntialias = enable;
}

bool PlatformContextUltralight::shouldAntialias() const {
  return m_state->m_shouldAntialias;
}

void PlatformContextUltralight::setCompositeOperator(CompositeOperator op) {
  //m_canvas->SetCompositeOp((ultralight::CompositeOp)op);
  notImplemented();
}

CompositeOperator PlatformContextUltralight::compositeOperator() const {
  notImplemented();
  return CompositeSourceOver;
  //return (CompositeOperator)m_canvas->GetCompositeOp();
}

void PlatformContextUltralight::setBlendMode(BlendMode mode) {
  //m_canvas->SetBlendMode((ultralight::BlendMode)mode);
  notImplemented();
}

BlendMode PlatformContextUltralight::blendMode() const {
  notImplemented();
  return BlendModeNormal;
  //return (BlendMode)m_canvas->GetBlendMode();
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

PlatformCanvas PlatformContextUltralight::getMask() {
  return nullptr;
}

ultralight::BorderPaint GetBorderPaint(BorderEdge edge) {
  ultralight::BorderPaint result;
  Color c = edge.color();
  result.color = UltralightRGBA(c.red(), c.green(), c.blue(), c.alpha());

  switch (edge.style()) {
  case INSET:
    result.style = ultralight::kBorderStyle_Inset;
    break;
  case GROOVE:
    result.style = ultralight::kBorderStyle_Groove;
    break;
  case OUTSET:
    result.style = ultralight::kBorderStyle_Outset;
    break;
  case RIDGE:
    result.style = ultralight::kBorderStyle_Ridge;
    break;
  case DOTTED:
    result.style = ultralight::kBorderStyle_Dotted;
    break;
  case DASHED:
    result.style = ultralight::kBorderStyle_Dashed;
    break;
  case SOLID:
    result.style = ultralight::kBorderStyle_Solid;
    break;
  case DOUBLE:
    result.style = ultralight::kBorderStyle_Double;
    break;
  default:
    result.style = ultralight::kBorderStyle_None;
    break;
  }

  return result;
}

void PlatformContextUltralight::DrawBoxDecorations(const FloatRect& layout_rect, const FloatRoundedRect& outer_rrect,
  const FloatRoundedRect& inner_rrect, BorderEdge edges[4], Color fill_color) {
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

void PlatformContextUltralight::DrawRoundedRect(const FloatRect& layout_rect, const FloatRoundedRect& rrect,
  Color fill_color, float stroke_width, Color stroke_color) {
  ultralight::RoundedRect rrect1;
  FloatRect r = rrect.rect();
  rrect1.rect = { r.x(), r.y(), r.x() + r.width(), r.y() + r.height() };
  rrect1.radii_x[0] = rrect.radii().topLeft().width();
  rrect1.radii_x[1] = rrect.radii().topRight().width();
  rrect1.radii_x[2] = rrect.radii().bottomRight().width();
  rrect1.radii_x[3] = rrect.radii().bottomLeft().width();
  rrect1.radii_y[0] = rrect.radii().topLeft().height();
  rrect1.radii_y[1] = rrect.radii().topRight().height();
  rrect1.radii_y[2] = rrect.radii().bottomRight().height();
  rrect1.radii_y[3] = rrect.radii().bottomLeft().height();

  ultralight::Color bgColor = UltralightRGBA(fill_color.red(), fill_color.green(), fill_color.blue(), fill_color.alpha());
  ultralight::Color strokeColor = UltralightRGBA(stroke_color.red(), stroke_color.green(), stroke_color.blue(), stroke_color.alpha());

  ultralight::Paint paint;
  paint.color = bgColor;
  m_canvas->DrawRoundedRect(rrect1, paint, stroke_width, strokeColor);
}

void PlatformContextUltralight::clipForPatternFilling(const GraphicsContextState&)
{
  // TODO
  notImplemented();
}

} // namespace WebCore
