#pragma once
#include "GraphicsContext.h"
#include <Ultralight/private/Paint.h>

#if USE(ULTRALIGHT)

namespace WebCore {

class WEBCORE_EXPORT GraphicsContextUltralight : public GraphicsContext {
public:
    GraphicsContextUltralight(ultralight::RefPtr<ultralight::Canvas> canvas);
    ~GraphicsContextUltralight();

    bool hasPlatformContext() const final;
    ultralight::Canvas* platformContext() const final;

    void save() final;
    void restore() final;

    void drawRect(const FloatRect&, float borderThickness = 1) final;
    void drawLine(const FloatPoint&, const FloatPoint&) final;
    void drawEllipse(const FloatRect&) final;

    void drawPath(const Path&) final;
    void fillPath(const Path&) final;
    void strokePath(const Path&) final;

    void beginTransparencyLayer(float opacity) final;
    void endTransparencyLayer() final;

    void applyDeviceScaleFactor(float factor) final;

    using GraphicsContext::fillRect;
    void fillRect(const FloatRect&) final;
    void fillRect(const FloatRect&, const Color&) final;
    void fillRoundedRectImpl(const FloatRoundedRect&, const Color&) final;
    void fillRectWithRoundedHole(const FloatRect&, const FloatRoundedRect& roundedHoleRect, const Color&) final;
    void clearRect(const FloatRect&) final;
    void strokeRect(const FloatRect&, float lineWidth) final;

    void fillEllipse(const FloatRect& ellipse) final;
    void strokeEllipse(const FloatRect& ellipse) final;

    RenderingMode renderingMode() const final;

    void clip(const FloatRect&) final;
    void clipOut(const FloatRect&) final;

    void clipOut(const Path&) final;

    void clipPath(const Path&, WindRule = WindRule::EvenOdd) final;

    IntRect clipBounds() const final;

    void setLineCap(LineCap) final;
    void setLineDash(const DashArray&, float dashOffset) final;
    void setLineJoin(LineJoin) final;
    void setMiterLimit(float) final;

    void drawNativeImage(NativeImage&, const FloatSize& selfSize, const FloatRect& destRect, const FloatRect& srcRect, const ImagePaintingOptions& = {}) final;
    void drawPattern(NativeImage&, const FloatRect& destRect, const FloatRect& tileRect, const AffineTransform& patternTransform, const FloatPoint& phase, const FloatSize& spacing, const ImagePaintingOptions& = {}) final;
    bool needsCachedNativeImageInvalidationWorkaround(RenderingMode) override;

    using GraphicsContext::scale;
    void scale(const FloatSize&) final;
    void rotate(float angleInRadians) final;
    void translate(float x, float y) final;

    void concatCTM(const AffineTransform&) final;
    void setCTM(const AffineTransform&) override;

    AffineTransform getCTM(IncludeDeviceScale = PossiblyIncludeDeviceScale) const override;

    void drawFocusRing(const Path&, float outlineWidth, const Color&) final;
    void drawFocusRing(const Vector<FloatRect>&, float outlineOffset, float outlineWidth, const Color&) final;

    void drawLinesForText(const FloatPoint&, float thickness, const DashArray& widths, bool printing, bool doubleLines, StrokeStyle) final;

    void drawDotsForDocumentMarker(const FloatRect&, DocumentMarkerLineStyle) final;

    void setURLForRect(const URL&, const FloatRect&) final;

    void setDestinationForRect(const String& name, const FloatRect&) final;
    void addDestinationAtPoint(const String& name, const FloatPoint&) final;

    bool supportsInternalLinks() const final;

    void didUpdateState(GraphicsContextState&) final;

    virtual bool canUseShadowBlur() const;

    virtual FloatRect roundToDevicePixels(const FloatRect&, RoundingMode = RoundAllSides) const;

private:
    void applyStrokePattern();
    void applyFillPattern();

    ultralight::RefPtr<ultralight::Canvas> m_canvas;

    struct ExtraState {
        ExtraState();
        ExtraState(const ExtraState& state);

        LineCap m_lineCap;
        LineJoin m_lineJoin;
        DashArray m_lineDash;
        float m_lineDashOffset;
        float m_miterLimit;
    };
    WTF::Vector<ExtraState> m_extraStateStack;
    ExtraState& extraState() { return m_extraStateStack.last(); }
};

} // namespace WebCore

#endif // USE(ULTRALIGHT)