#include "config.h"
#include "GraphicsContextUltralight.h"
#include "NotImplemented.h"
#include "ShadowBlur.h"

#if USE(ULTRALIGHT)

namespace WebCore {

GraphicsContextUltralight::ExtraState::ExtraState()
    : m_lineCap(LineCap::Butt)
    , m_lineJoin(LineJoin::Miter)
    , m_lineDashOffset(0.0f)
    , m_miterLimit(10.0f)
{
}

GraphicsContextUltralight::ExtraState::ExtraState(const ExtraState& state)
    : m_lineCap(state.m_lineCap)
    , m_lineJoin(state.m_lineJoin)
    , m_lineDash(state.m_lineDash)
    , m_lineDashOffset(state.m_lineDashOffset)
    , m_miterLimit(state.m_miterLimit)
{
}

GraphicsContextUltralight::GraphicsContextUltralight(ultralight::RefPtr<ultralight::Canvas> canvas)
    : GraphicsContext(GraphicsContextState::basicChangeFlags)
    , m_canvas(canvas)
{
    ASSERT(m_canvas);
    m_extraStateStack.append(ExtraState());
    // Make sure the context starts in sync with our state.
    didUpdateState(m_state);
}

GraphicsContextUltralight::~GraphicsContextUltralight()
{
}

bool GraphicsContextUltralight::hasPlatformContext() const
{
    return true;
}

ultralight::Canvas* GraphicsContextUltralight::platformContext() const
{
    ASSERT(m_canvas);
    return m_canvas.get();
}

void GraphicsContextUltralight::save()
{
    GraphicsContext::save();

    m_canvas->Save();
}

void GraphicsContextUltralight::restore()
{
    if (!stackSize())
        return;

    GraphicsContext::restore();

    m_canvas->Restore();
}

void GraphicsContextUltralight::drawNativeImage(NativeImage& nativeImage, const FloatSize& imageSize, const FloatRect& destRect, const FloatRect& srcRect, const ImagePaintingOptions& options)
{
    ProfiledZone;
    auto image = nativeImage.platformImage();
    auto imageRect = FloatRect { {}, imageSize };
    auto normalizedSrcRect = normalizeRect(srcRect);
    auto normalizedDestRect = normalizeRect(destRect);

    if (!image || !imageRect.intersects(normalizedSrcRect))
        return;

    save();

    // Skip blending if we can do a straight copy of opaque pixels
    if (options.compositeOperator() == CompositeOperator::SourceOver && options.blendMode() == BlendMode::Normal && !nativeImage.hasAlpha())
        setCompositeOperation(CompositeOperator::Copy);
    else
        setCompositeOperation(options.compositeOperator(), options.blendMode());

#if ENABLE(IMAGE_DECODER_DOWN_SAMPLING)
    IntSize scaledSize = nativeImageSize(image);
    FloatRect adjustedSrcRect = adjustSourceRectForDownSampling(srcRect, scaledSize);
#else
    FloatRect adjustedSrcRect(srcRect);
#endif

    FloatRect adjustedDestRect = destRect;

    auto orientation = options.orientation();
    if (orientation != ImageOrientation()) {
        // ImageOrientation expects the origin to be at (0, 0).
        translate(destRect.x(), destRect.y());
        adjustedDestRect.setLocation(FloatPoint());
        concatCTM(orientation.transformFromDefault(adjustedDestRect.size()));
        if (orientation.usesWidthAsHeight()) {
            // The destination rectangle will have it's width and height already reversed for the orientation of
            // the image, as it was needed for page layout, so we need to reverse it back here.
            adjustedDestRect.setSize(adjustedDestRect.size().transposedSize());
        }
    }

    platformContext()->DrawImage(image, adjustedSrcRect, adjustedDestRect, UltralightColorWHITE);
    restore();
}

bool GraphicsContextUltralight::needsCachedNativeImageInvalidationWorkaround(RenderingMode imageRenderingMode)
{
    // Only GPU-backed Canvases need the workaround
    return m_canvas->surface() == nullptr;
}

void GraphicsContextUltralight::drawPattern(NativeImage& nativeImage, const FloatRect& destRect, const FloatRect& tileRect, const AffineTransform& patternTransform, const FloatPoint& phase, const FloatSize& spacing, const ImagePaintingOptions& options)
{
    ProfiledZone;
    if (!patternTransform.isInvertible())
        return;

    auto image = nativeImage.platformImage();
    auto imageSize = nativeImage.size();

    // Avoid NaN
    if (!std::isfinite(phase.x()) || !std::isfinite(phase.y()))
        return;

    // Compute combined transform (patternTransform and phase)
    AffineTransform combined = patternTransform;
    combined.multiply(AffineTransform(1, 0, 0, 1,
        phase.x() + tileRect.x() * patternTransform.a(),
        phase.y() + tileRect.y() * patternTransform.d()));

    save();

    // Skip blending if we can do a straight copy of opaque pixels
    if (options.compositeOperator() == CompositeOperator::SourceOver && options.blendMode() == BlendMode::Normal && !nativeImage.hasAlpha())
        setCompositeOperation(CompositeOperator::Copy);
    else
        setCompositeOperation(options.compositeOperator(), options.blendMode());

    ultralight::Rect dest = destRect;
    ultralight::Rect src = tileRect;
    platformContext()->DrawPattern(image, src, dest, combined);

    restore();
}

// Draws a filled rectangle with a stroked border.
void GraphicsContextUltralight::drawRect(const FloatRect& rect, float borderThickness)
{
    ProfiledZone;
    // FIXME: this function does not handle patterns and gradients like drawPath does, it probably should.
    ASSERT(!rect.isEmpty());

    platformContext()->DrawRect(rect, fillColor());

    if (strokeStyle() != NoStroke) {
        // We do a fill of four rects to simulate the stroke of a border.
        FloatRect rects[4] = {
            FloatRect(rect.x(), rect.y(), rect.width(), borderThickness),
            FloatRect(rect.x(), rect.maxY() - borderThickness, rect.width(), borderThickness),
            FloatRect(rect.x(), rect.y() + borderThickness, borderThickness, rect.height() - 2 * borderThickness),
            FloatRect(rect.maxX() - borderThickness, rect.y() + borderThickness, borderThickness, rect.height() - 2 * borderThickness)
        };

        for (size_t i = 0; i < 4; ++i)
            platformContext()->DrawRect(rects[i], strokeColor());
    }
}

// This is only used to draw borders.
void GraphicsContextUltralight::drawLine(const FloatPoint& point1, const FloatPoint& point2)
{
    ProfiledZone;
    if (strokeStyle() == NoStroke)
        return;

    float thickness = strokeThickness();
    bool isVerticalLine = (point1.x() + thickness == point2.x());
    float strokeWidth = isVerticalLine ? point2.y() - point1.y() : point2.x() - point1.x();
    if (!thickness || !strokeWidth)
        return;

    Path path;
    path.moveTo(point1);
    path.addLineTo(point2);
    strokePath(path);
}

void GraphicsContextUltralight::drawEllipse(const FloatRect& rect)
{
    Path path;
    path.addEllipse(rect);
    drawPath(path);
}

void GraphicsContextUltralight::applyStrokePattern()
{
    if (!strokePattern())
        return;

    // Ultralight TODO
}

void GraphicsContextUltralight::applyFillPattern()
{
    if (!fillPattern())
        return;

    // Ultralight TODO
}

void GraphicsContextUltralight::drawPath(const Path& path)
{
    if (path.isEmpty())
        return;

    fillPath(path);
    strokePath(path);
}

void GraphicsContextUltralight::fillPath(const Path& path)
{
    ProfiledZone;
    if (path.isEmpty())
        return;

    auto platformFillRule = fillRule() == WindRule::NonZero ? ultralight::kFillRule_NonZero : ultralight::kFillRule_EvenOdd;

    if (auto fillGradient = this->fillGradient()) {
        platformContext()->Save();
        platformContext()->SetClip(path.platformPath(), platformFillRule, false);
        fillGradient->fill(*this, path.fastBoundingRect());
        platformContext()->Restore();
        return;
    }

    platformContext()->FillPath(path.platformPath(), fillColor(), platformFillRule);
}

void GraphicsContextUltralight::strokePath(const Path& path)
{
    ProfiledZone;
    if (path.isEmpty())
        return;

    ultralight::LineCap lineCap;
    switch (extraState().m_lineCap) {
    case WebCore::LineCap::Round:
        lineCap = ultralight::kLineCap_Round;
        break;
    case WebCore::LineCap::Square:
        lineCap = ultralight::kLineCap_Square;
        break;
    case WebCore::LineCap::Butt:
    default:
        lineCap = ultralight::kLineCap_Butt;
    }

    ultralight::LineJoin lineJoin;
    switch (extraState().m_lineJoin) {
    case WebCore::LineJoin::Bevel:
        lineJoin = ultralight::kLineJoin_Bevel;
        break;
    case WebCore::LineJoin::Round:
        lineJoin = ultralight::kLineJoin_Round;
        break;
    case WebCore::LineJoin::Miter:
    default:
        lineJoin = ultralight::kLineJoin_Miter;
    }

    float* dashArray = nullptr;
    size_t dashArraySize = extraState().m_lineDash.size();

    if (dashArraySize)
        dashArray = extraState().m_lineDash.data();

    platformContext()->StrokePath(path.platformPath(), strokeColor(), strokeThickness(),
        lineCap, lineJoin, extraState().m_miterLimit, dashArray, dashArraySize,
        extraState().m_lineDashOffset);
}

void GraphicsContextUltralight::fillRect(const FloatRect& rect)
{
    ProfiledZone;
    if (auto fillGradient = this->fillGradient()) {
        save();
        fillGradient->fill(*this, rect);
        restore();
        return;
    }

    fillRect(rect, fillColor());
}

void GraphicsContextUltralight::fillRect(const FloatRect& rect, const Color& color)
{
    ProfiledZone;
    if (hasVisibleShadow()) {
        ShadowBlur contextShadow(dropShadow(), shadowsIgnoreTransforms());
        contextShadow.drawRectShadow(*this, FloatRoundedRect(rect));
    }

    if (color.isVisible()) {
        platformContext()->DrawRect(rect, color);
    }
}

void GraphicsContextUltralight::fillRoundedRectImpl(const FloatRoundedRect& rect, const Color& color)
{
    ProfiledZone;
    if (hasVisibleShadow()) {
        ShadowBlur contextShadow(dropShadow(), shadowsIgnoreTransforms());
        contextShadow.drawRectShadow(*this, rect);
    }

    if (color.isVisible()) {
        platformContext()->DrawRoundedRect(rect, color, 0.0, UltralightColorTRANSPARENT);
    }
}

void GraphicsContextUltralight::fillRectWithRoundedHole(const FloatRect& rect, const FloatRoundedRect& roundedHoleRect, const Color& color)
{
    ProfiledZone;
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

    // fillRectWithRoundedHole() assumes that the edges of rect are clipped out, so we only care about shadows cast around inside the hole.
    if (hasVisibleShadow()) {
        ShadowBlur contextShadow(dropShadow(), shadowsIgnoreTransforms());
        contextShadow.drawInsetShadow(*this, rect, roundedHoleRect);
    }

    fillPath(path);

    setFillRule(oldFillRule);
    setFillColor(oldFillColor);
}

void GraphicsContextUltralight::clip(const FloatRect& rect)
{
    ProfiledZone;
    platformContext()->SetClip(rect, false);
}

void GraphicsContextUltralight::clipOut(const FloatRect& rect)
{
    ProfiledZone;
    platformContext()->SetClip(rect, true);
}

void GraphicsContextUltralight::clipOut(const Path& path)
{
    ProfiledZone;
    platformContext()->SetClip(path.platformPath(), ultralight::kFillRule_EvenOdd, true);
}

void GraphicsContextUltralight::clipPath(const Path& path, WindRule clipRule)
{
    ProfiledZone;
    platformContext()->SetClip(path.platformPath(), 
      clipRule == WindRule::EvenOdd ? ultralight::kFillRule_EvenOdd : ultralight::kFillRule_NonZero, false);
}

IntRect GraphicsContextUltralight::clipBounds() const
{
    return enclosingIntRect(platformContext()->GetClipBounds());
}

void GraphicsContextUltralight::beginTransparencyLayer(float opacity)
{
    GraphicsContext::beginTransparencyLayer(opacity);

    save();

    platformContext()->BeginTransparencyLayer(opacity);
}

void GraphicsContextUltralight::endTransparencyLayer()
{
    GraphicsContext::endTransparencyLayer();

    platformContext()->EndTransparencyLayer();

    restore();
}

void GraphicsContextUltralight::didUpdateState(GraphicsContextState& state)
{
    if (!state.changes())
        return;

    auto context = platformContext();

    for (auto change : state.changes()) {
        switch (change) {
        case GraphicsContextState::Change::FillBrush:
            break;
        case GraphicsContextState::Change::StrokeThickness:
            break;
        case GraphicsContextState::Change::StrokeBrush:
            break;
        case GraphicsContextState::Change::CompositeMode: {
            context->SetCompositeOp((ultralight::CompositeOp)state.compositeMode().operation);
            context->SetBlendMode((ultralight::BlendMode)state.compositeMode().blendMode);
            break;
        }
        case GraphicsContextState::Change::DropShadow:
            break;
        case GraphicsContextState::Change::Style:
            break;
        case GraphicsContextState::Change::Alpha:
            context->SetAlpha(state.alpha());
            break;
        case GraphicsContextState::Change::ImageInterpolationQuality:
            break;
        case GraphicsContextState::Change::TextDrawingMode:
            break;
        case GraphicsContextState::Change::ShouldAntialias:
            break;
        case GraphicsContextState::Change::ShouldSmoothFonts:
            break;
        default:
            break;
        }
    }

    state.didApplyChanges();
}

void GraphicsContextUltralight::setMiterLimit(float limit)
{
    extraState().m_miterLimit = limit;
}

void GraphicsContextUltralight::clearRect(const FloatRect& rect)
{
    ProfiledZone;
    auto canvas = platformContext();
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
    canvas->DrawRect(aabb, UltralightColorTRANSPARENT);
    canvas->set_blending_enabled(true);
    canvas->set_scissor_enabled(false);

    canvas->Restore();
}

void GraphicsContextUltralight::strokeRect(const FloatRect& rect, float width)
{
    if (auto strokeGradient = this->strokeGradient()) {
        // Ultralight TODO
        return;
    }

    if (strokePattern())
        applyStrokePattern();

  if (width < 0.001)
        return;

    if (rect.isEmpty() || rect.isZero())
        return;

    Path path;
    path.addRect(rect);

    platformContext()->StrokePath(path.ensurePlatformPath(), strokeColor(), width);
    // TODO, handle shadow state
}

void GraphicsContextUltralight::setLineCap(LineCap cap)
{
    extraState().m_lineCap = cap;
}

void GraphicsContextUltralight::setLineDash(const DashArray& dashes, float dashOffset)
{
    extraState().m_lineDash = dashes;
    extraState().m_lineDashOffset = dashOffset;
}

void GraphicsContextUltralight::setLineJoin(LineJoin join)
{
    extraState().m_lineJoin = join;
}

void GraphicsContextUltralight::scale(const FloatSize& size)
{
    AffineTransform transform;
    transform.scale(size);

    platformContext()->Transform(transform);
}

void GraphicsContextUltralight::rotate(float radians)
{
    double cosAngle = cos(radians);
    double sinAngle = sin(radians);
    AffineTransform transform(cosAngle, sinAngle, -sinAngle, cosAngle, 0, 0);

    platformContext()->Transform(transform);
}

void GraphicsContextUltralight::translate(float x, float y)
{
    AffineTransform transform;
    transform.translate(x, y);

    platformContext()->Transform(transform);
}

void GraphicsContextUltralight::concatCTM(const AffineTransform& transform)
{
    platformContext()->Transform(transform);
}

void GraphicsContextUltralight::setCTM(const AffineTransform& transform)
{
    platformContext()->SetMatrix(transform);
}

AffineTransform GraphicsContextUltralight::getCTM(IncludeDeviceScale includeScale) const
{
    ultralight::Matrix m = platformContext()->GetMatrix();
    return AffineTransform(m.a(), m.b(), m.c(), m.d(), m.e(), m.f());
}

void GraphicsContextUltralight::drawFocusRing(const Path& path, float width, const Color& color)
{
    if (width <= 0.1 || path.isEmpty())
        return;

    platformContext()->StrokePath(path.platformPath(), color, width * 0.5 / scaleFactor().width());
}

void GraphicsContextUltralight::drawFocusRing(const Vector<FloatRect>& rects, float outlineOffset, float width, const Color& color)
{
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
    drawFocusRing(path, width, color);
}

FloatRect GraphicsContextUltralight::roundToDevicePixels(const FloatRect& rect, RoundingMode roundingMode) const
{
    // Ultralight TODO
    notImplemented();
    return rect;
}

void GraphicsContextUltralight::drawLinesForText(const FloatPoint& point, float thickness, const DashArray& widths, bool printing, bool doubleUnderlines, StrokeStyle strokeStyle)
{
    ProfiledZone;
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

    for (auto& dash : dashBounds)
        platformContext()->DrawRect(dash, localStrokeColor);
}

void GraphicsContextUltralight::drawDotsForDocumentMarker(const FloatRect&, DocumentMarkerLineStyle)
{
    // Ultralight TODO
    notImplemented();
}

void GraphicsContextUltralight::setURLForRect(const URL& link, const FloatRect& destRect)
{
    // Ultralight TODO
    notImplemented();
}

RenderingMode GraphicsContextUltralight::renderingMode() const
{
    return RenderingMode::Unaccelerated;
}

void GraphicsContextUltralight::applyDeviceScaleFactor(float deviceScaleFactor)
{
    GraphicsContext::applyDeviceScaleFactor(deviceScaleFactor);
    platformContext()->SetDeviceScaleHint(deviceScaleFactor);
}

void GraphicsContextUltralight::fillEllipse(const FloatRect& ellipse)
{
    fillEllipseAsPath(ellipse);
}

void GraphicsContextUltralight::strokeEllipse(const FloatRect& ellipse)
{
    strokeEllipseAsPath(ellipse);
}

bool GraphicsContextUltralight::supportsInternalLinks() const
{
    return false;
}

void GraphicsContextUltralight::setDestinationForRect(const String& name, const FloatRect& destRect)
{
    // Ultralight TODO
    notImplemented();
}

void GraphicsContextUltralight::addDestinationAtPoint(const String& name, const FloatPoint& position)
{
    // Ultralight TODO
    notImplemented();
}

bool GraphicsContextUltralight::canUseShadowBlur() const
{
    return (renderingMode() == RenderingMode::Unaccelerated) && hasBlurredShadow() && !m_state.shadowsIgnoreTransforms();
}

} // namespace WebCore

#endif // USE(ULTRALIGHT)