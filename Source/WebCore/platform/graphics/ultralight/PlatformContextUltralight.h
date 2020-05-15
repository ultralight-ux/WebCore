#pragma once

#if USE(ULTRALIGHT)

#include "GraphicsContext.h"
#include <Ultralight/private/Canvas.h>
#include "ShadowBlur.h"


typedef ultralight::RefPtr<ultralight::Canvas> PlatformCanvas;

namespace WebCore {

  class BorderEdge;

  struct GraphicsContextState;

  // Much like PlatformContextSkia in the Skia port, this class holds information that
  // would normally be private to GraphicsContext, except that we want to allow access
  // to it in Font and Image code. This allows us to separate the concerns of Ultralight-specific
  // code from the platform-independent GraphicsContext.

  class PlatformContextUltralight {
    WTF_MAKE_NONCOPYABLE(PlatformContextUltralight);
  public:
    PlatformContextUltralight(PlatformCanvas);
    ~PlatformContextUltralight();

    PlatformCanvas canvas();

    ShadowBlur& shadowBlur() { return m_shadowBlur; }

    void save();
    void restore();

    void setGlobalAlpha(float);
    float globalAlpha() const;

    void setShouldAntialias(bool enable);
    bool shouldAntialias() const;

    void setCompositeOperator(CompositeOperator op);
    CompositeOperator compositeOperator() const;

    void setBlendMode(BlendMode mode);
    BlendMode blendMode() const;

    void setFillColor(const Color& color);
    Color fillColor() const;

    void setStrokeColor(const Color& color);
    Color strokeColor() const;

    void setStrokeThickness(float thickness);
    float strokeThickness() const;

    void setLineCap(LineCap lineCap);
    LineCap lineCap() const;

    void setLineJoin(LineJoin lineJoin);
    LineJoin lineJoin() const;

    void setMiterLimit(float miter);
    float miterLimit() const;

    void setImageInterpolationQuality(InterpolationQuality);
    InterpolationQuality imageInterpolationQuality() const;

    enum PatternAdjustment { NoAdjustment, AdjustPatternForGlobalAlpha };
    void prepareForFilling(const GraphicsContextState&, PatternAdjustment);

    enum AlphaPreservation { DoNotPreserveAlpha, PreserveAlpha };
    void prepareForStroking(const GraphicsContextState&, AlphaPreservation = PreserveAlpha);

    void setMask(const Path& path, WindRule clipRule);

    void DrawBoxDecorations(const FloatRect& layout_rect, const FloatRoundedRect& outer_rrect,
      const FloatRoundedRect& inner_rrect, const BorderEdge* edges, Color fill_color);

    void DrawRoundedRect(const FloatRect& layout_rect, const FloatRoundedRect& rrect,
      Color fill_color, float stroke_width, Color stroke_color);

    void DrawBoxShadow(const FloatRoundedRect& rect, const FloatSize& shadowOffset, float shadowBlur,
      const Color& shadowColor, bool insetShadow, const FloatRoundedRect& clip_rect);

  private:
    void clipForPatternFilling(const GraphicsContextState&);

    PlatformCanvas m_canvas;

    class State;
    State* m_state;
    WTF::Vector<State> m_stateStack;

    // GraphicsContext is responsible for managing the state of the ShadowBlur,
    // so it does not need to be on the state stack.
    ShadowBlur m_shadowBlur;

  };

} // namespace WebCore

#endif // USE(ULTRALIGHT)
