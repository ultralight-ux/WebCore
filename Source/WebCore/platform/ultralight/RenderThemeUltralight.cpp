#include "config.h"
#include "RenderTheme.h"
#include "NeverDestroyed.h"
#include <Ultralight/platform/Platform.h>
#include <Ultralight/platform/Config.h>
#include <Ultralight/private/Paint.h>
#include "WebCore/platform/ultralight/StringUltralight.h"
#include "WebCore/platform/graphics/ultralight/PlatformContextUltralight.h"
#include "WebCore/rendering/style/RenderStyle.h"
#include "WebCore/rendering/RenderObject.h"
#include "WebCore/rendering/RenderView.h"
#include "WebCore/FloatRoundedRect.h"
#include "WebCore/rendering/RenderBox.h"
#include "WebCore/css/CSSToLengthConversionData.h"

namespace WebCore {

static const float ControlBaseHeight = 20;
static const float ControlBaseFontSize = 11;

class RenderThemeUltralight : public RenderTheme {
public:
  RenderThemeUltralight() {}
  virtual ~RenderThemeUltralight() {}

  virtual String extraDefaultStyleSheet() { 
    auto& config = ultralight::Platform::instance().config();
    return Convert(config.user_stylesheet);
  }

  FloatRect addRoundedBorderClip(const RenderObject& box, GraphicsContext& context, const IntRect& rect)
  {
    // To fix inner border bleeding issues <rdar://problem/9812507>, we clip to the outer border and assert that
    // the border is opaque or transparent, unless we're checked because checked radio/checkboxes show no bleeding.
    auto& style = box.style();
    RoundedRect border = isChecked(box) ? style.getRoundedInnerBorderFor(rect) : style.getRoundedBorderFor(rect);

    if (border.isRounded())
      context.clipRoundedRect(FloatRoundedRect(border));
    else
      context.clip(border.rect());

    if (isChecked(box)) {
      ASSERT(style.visitedDependentColor(CSSPropertyBorderTopColor).alpha() % 255 == 0);
      ASSERT(style.visitedDependentColor(CSSPropertyBorderRightColor).alpha() % 255 == 0);
      ASSERT(style.visitedDependentColor(CSSPropertyBorderBottomColor).alpha() % 255 == 0);
      ASSERT(style.visitedDependentColor(CSSPropertyBorderLeftColor).alpha() % 255 == 0);
    }

    return border.rect();
  }

  virtual Color platformFocusRingColor() const override { return Color(0, 151, 255); }

  virtual Color platformActiveSelectionBackgroundColor() const override { return Color(0, 151, 255); }

  virtual void updateCachedSystemFontDescription(CSSValueID systemFontID, FontCascadeDescription&) const {}

  // Graphics / drawing

  struct ColorPair {
    ultralight::Color color1;
    ultralight::Color color2;
    ColorPair(float start[4], float end[4]) {
      color1 = UltralightRGBA(start[0] * 255, start[1] * 255, start[2] * 255, start[3] * 255);
      color2 = UltralightRGBA(end[0] * 255, end[1] * 255, end[2] * 255, end[3] * 255);
    }
  };

  static ColorPair* getConcaveGradient()
  {
    static float end[4] = { 255 / 255.0, 255 / 255.0, 255 / 255.0, 0.46 };
    static float start[4] = { 255 / 255.0, 255 / 255.0, 255 / 255.0, 0 };
    static NeverDestroyed<ColorPair> gradient(start, end);
    return &gradient.get();
  }

  static ColorPair* getConvexGradient()
  {
    static float end[4] = { 255 / 255.0, 255 / 255.0, 255 / 255.0, 0.05 };
    static float start[4] = { 255 / 255.0, 255 / 255.0, 255 / 255.0, 0.43 };
    static NeverDestroyed<ColorPair> gradient(start, end);
    return &gradient.get();
  }

  static ColorPair* getInsetGradient()
  {
    static float end[4] = { 0 / 255.0, 0 / 255.0, 0 / 255.0, 0 };
    static float start[4] = { 0 / 255.0, 0 / 255.0, 0 / 255.0, 0.2 };
    static NeverDestroyed<ColorPair> gradient(start, end);
    return &gradient.get();
  }

  static ColorPair* getShineGradient()
  {
    static float end[4] = { 1, 1, 1, 0.8 };
    static float start[4] = { 1, 1, 1, 0 };
    static NeverDestroyed<ColorPair> gradient(start, end);
    return &gradient.get();
  }

  static ColorPair* getShadeGradient()
  {
    static float end[4] = { 178 / 255.0, 178 / 255.0, 178 / 255.0, 0.65 };
    static float start[4] = { 252 / 255.0, 252 / 255.0, 252 / 255.0, 0.65 };
    static NeverDestroyed<ColorPair> gradient(start, end);
    return &gradient.get();
  }

  const Color& shadowColor() const
  {
    static Color color(0.0f, 0.0f, 0.0f, 0.7f);
    return color;
  }

  static ultralight::Point shortened(ultralight::Point start, ultralight::Point end, float width)
  {
    float x = end.x - start.x;
    float y = end.y - start.y;
    float ratio = (!x && !y) ? 0 : width / sqrtf(x * x + y * y);
    return ultralight::Point(start.x + x * ratio, start.y + y * ratio);
  }

  static void drawAxialGradient(PlatformCanvas canvas, ColorPair* colorPair, const FloatPoint& startPoint, const FloatPoint& stopPoint, const FloatRect& clip)
  {
    ultralight::Gradient gradient;
    gradient.num_stops = 2;
    gradient.is_radial = false;
    gradient.p0 = ultralight::Point(startPoint.x(), startPoint.y());
    gradient.p1 = ultralight::Point(stopPoint.x(), stopPoint.y());
    gradient.stops[0].color = colorPair->color1;
    gradient.stops[0].stop = 0.0f;
    gradient.stops[1].color = colorPair->color2;
    gradient.stops[1].stop = 1.0f;

    canvas->DrawGradient(&gradient, clip);
  }

  static void drawRadialGradient(PlatformCanvas canvas, ColorPair* colorPair, const FloatPoint& startPoint, float startRadius, const FloatPoint& stopPoint, float stopRadius, const FloatRect& clip)
  {
    ultralight::Gradient gradient;
    gradient.num_stops = 2;
    gradient.is_radial = true;
    gradient.p0 = ultralight::Point(startPoint.x(), startPoint.y());
    gradient.p1 = ultralight::Point(stopPoint.x(), stopPoint.y());
    gradient.r0 = startRadius;
    gradient.r1 = stopRadius;
    gradient.stops[0].color = colorPair->color1;
    gradient.stops[0].stop = 0.0f;
    gradient.stops[1].color = colorPair->color2;
    gradient.stops[1].stop = 1.0f;

    canvas->DrawGradient(&gradient, clip);
  }

  static void drawJoinedLines(PlatformCanvas canvas, ultralight::Point points[], unsigned count, float strokeWidth, Color color)
  {
    ultralight::Ref<ultralight::Path> path = ultralight::Path::Create();
    path->MoveTo(points[0]);
    for (unsigned i = 1; i < count; ++i)
      path->LineTo(points[i]);

    ultralight::Paint paint;
    paint.color = UltralightRGBA(color.red(), color.green(), color.blue(), color.alpha());

    canvas->StrokePath(path, paint, strokeWidth);
  }

  // Adjustments
  virtual int baselinePosition(const RenderBox& box) const override {
    if (box.style().appearance() == CheckboxPart || box.style().appearance() == RadioPart)
      return box.marginTop() + box.height() - 2; // The baseline is 2px up from the bottom of the checkbox/radio in AppKit.
    if (box.style().appearance() == MenulistPart)
      return box.marginTop() + box.height() - 5; // This is to match AppKit. There might be a better way to calculate this though.
    return RenderTheme::baselinePosition(box);
  }

  virtual bool isControlStyled(const RenderStyle& style, const BorderData& border, const FillLayer& background, const Color& backgroundColor) const override
  {
    // Buttons and MenulistButtons are styled if they contain a background image.
    if (style.appearance() == PushButtonPart || style.appearance() == MenulistButtonPart)
      return !style.visitedDependentColor(CSSPropertyBackgroundColor).alpha() || style.backgroundLayers()->hasImage();

    if (style.appearance() == TextFieldPart || style.appearance() == TextAreaPart)
      return *style.backgroundLayers() != background;

    return RenderTheme::isControlStyled(style, border, background, backgroundColor);
  }

  // Check boxes

  virtual void adjustCheckboxStyle(StyleResolver&, RenderStyle& style, const Element*) const override
  {
    if (!style.width().isIntrinsicOrAuto() && !style.height().isAuto())
      return;

    Length length = Length(static_cast<int>(ceilf(std::max(style.fontSize(), 10))), Fixed);

    style.setWidth(length);
    style.setHeight(length);
  }

  virtual bool paintCheckbox(const RenderObject&, const PaintInfo&, const IntRect&) override { return true; }

  virtual void setCheckboxSize(RenderStyle&) const override { }

  virtual bool paintCheckboxDecorations(const RenderObject& box, const PaintInfo& paintInfo, const IntRect& rect) override
  {
    GraphicsContextStateSaver stateSaver(paintInfo.context());
    FloatRect clip = addRoundedBorderClip(box, paintInfo.context(), rect);
    PlatformGraphicsContext* context = paintInfo.context().platformContext();
    PlatformCanvas canvas = context->canvas();
    float width = clip.width();
    float height = clip.height();

    if (isChecked(box)) {
      drawAxialGradient(canvas, getConcaveGradient(), clip.location(), FloatPoint(clip.x(), clip.maxY()), clip);

      static const float thicknessRatio = 2 / 14.0;
      static const FloatSize size(14.0f, 14.0f);
      static const ultralight::Point pathRatios[3] = {
        { 2.5f / size.width(), 7.5f / size.height() },
        { 5.5f / size.width(), 10.5f / size.height() },
        { 11.5f / size.width(), 2.5f / size.height() }
      };

      float lineWidth = std::min(width, height) * 2.0f * thicknessRatio;

      ultralight::Point line[3] = {
        ultralight::Point(clip.x() + width * pathRatios[0].x, clip.y() + height * pathRatios[0].y),
        ultralight::Point(clip.x() + width * pathRatios[1].x, clip.y() + height * pathRatios[1].y),
        ultralight::Point(clip.x() + width * pathRatios[2].x, clip.y() + height * pathRatios[2].y)
      };
      ultralight::Point shadow[3] = {
        shortened(line[0], line[1], lineWidth / 4.0f),
        line[1],
        shortened(line[2], line[1], lineWidth / 4.0f)
      };

      lineWidth = std::max<float>(lineWidth, 1);
      // TODO: handle line cap (square)
      drawJoinedLines(canvas, shadow, 3, lineWidth, Color(0.0f, 0.0f, 0.0f, 0.7f));

      lineWidth = std::max<float>(std::min(clip.width(), clip.height()) * thicknessRatio, 1);
      // TODO: handle line cap (butt)-- (though I think this is the default).
      drawJoinedLines(canvas, line, 3, lineWidth, Color(1.0f, 1.0f, 1.0f, 240 / 255.0f));
    }
    else {
      FloatPoint bottomCenter(clip.x() + clip.width() / 2.0f, clip.maxY());
      drawAxialGradient(canvas, getShadeGradient(), clip.location(), FloatPoint(clip.x(), clip.maxY()), clip);
      // TODO: handle exponential interpolation
      drawRadialGradient(canvas, getShineGradient(), bottomCenter, 0, bottomCenter, sqrtf((width * width) / 4.0f + height * height), clip);
    }

    return false;
  }

  // Radio buttons
  virtual void adjustRadioStyle(StyleResolver&, RenderStyle& style, const Element*) const override { 
    if (!style.width().isIntrinsicOrAuto() && !style.height().isAuto())
      return;

    Length length = Length(static_cast<int>(ceilf(std::max(style.fontSize(), 10))), Fixed);
    style.setWidth(length);
    style.setHeight(length);
    style.setBorderRadius(IntSize(length.value() / 2.0f, length.value() / 2.0f));
  }

  virtual bool paintRadio(const RenderObject&, const PaintInfo&, const IntRect&) override { return true; }
  virtual void setRadioSize(RenderStyle&) const override { }
  virtual bool paintRadioDecorations(const RenderObject& box, const PaintInfo& paintInfo, const IntRect& rect) override { 
    GraphicsContextStateSaver stateSaver(paintInfo.context());
    FloatRect clip = addRoundedBorderClip(box, paintInfo.context(), rect);
    PlatformGraphicsContext* context = paintInfo.context().platformContext();
    PlatformCanvas canvas = context->canvas();

    if (isChecked(box)) {
      drawAxialGradient(canvas, getConcaveGradient(), clip.location(), FloatPoint(clip.x(), clip.maxY()), clip);

      // The inner circle is 6 / 14 the size of the surrounding circle, 
      // leaving 8 / 14 around it. (8 / 14) / 2 = 2 / 7.

      static const float InnerInverseRatio = 2 / 7.0;

      clip.inflateX(-clip.width() * InnerInverseRatio);
      clip.inflateY(-clip.height() * InnerInverseRatio);

      paintInfo.context().drawRaisedEllipse(clip, Color::white, shadowColor());

      // TODO: Handle clipRoundedRect properly (not creating rounded clip?), bail early instead of doing extra shading...
      return false;

      FloatSize radius(clip.width() / 2.0f, clip.height() / 2.0f);
      paintInfo.context().clipRoundedRect(FloatRoundedRect(clip, radius, radius, radius, radius));
    }
    FloatPoint bottomCenter(clip.x() + clip.width() / 2.0, clip.maxY());
    drawAxialGradient(canvas, getShadeGradient(), clip.location(), FloatPoint(clip.x(), clip.maxY()), clip);
    // TODO: handle exponential interpolation
    drawRadialGradient(canvas, getShineGradient(), bottomCenter, 0, bottomCenter, std::max(clip.width(), clip.height()), clip);
    return false;
  }

  // Buttons
  virtual bool paintButton(const RenderObject&, const PaintInfo&, const IntRect&) override { return true; }
  virtual void setButtonSize(RenderStyle& style) const override { 
    // If the width and height are both specified, then we have nothing to do.
    if (!style.width().isIntrinsicOrAuto() && !style.height().isAuto())
      return;

    // Use the font size to determine the intrinsic width of the control.
    style.setHeight(Length(static_cast<int>(ControlBaseHeight / ControlBaseFontSize * style.fontDescription().computedSize()), Fixed));
  }

  void adjustRoundBorderRadius(RenderStyle& style, RenderBox& box) const
  {
    if (style.appearance() == NoControlPart || style.backgroundLayers()->hasImage())
      return;

    // FIXME: We should not be relying on border radius for the appearance of our controls <rdar://problem/7675493>
    Length radiusWidth(static_cast<int>(std::min(box.width(), box.height()) / 2.0), Fixed);
    Length radiusHeight(static_cast<int>(box.height() / 2.0), Fixed);
    style.setBorderRadius(LengthSize(radiusWidth, radiusHeight));
  }

  virtual void adjustButtonStyle(StyleResolver& selector, RenderStyle& style, const Element* element) const override
  {
    RenderTheme::adjustButtonStyle(selector, style, element);

    // TODO: Not working correctly with styled buttons. Disable these adjustments for now:
    return;

    // Set padding: 0 1.0em; on buttons.
    // CSSPrimitiveValue::computeLengthInt only needs the element's style to calculate em lengths.
    // Since the element might not be in a document, just pass nullptr for the root element style
    // and the render view.
    RefPtr<CSSPrimitiveValue> emSize = CSSPrimitiveValue::create(1.0, CSSPrimitiveValue::CSS_EMS);
    int pixels = emSize->computeLength<int>(CSSToLengthConversionData(&style, nullptr, nullptr, 1.0, false));
    style.setPaddingBox(LengthBox(0, pixels, 0, pixels));

    if (!element)
      return;

    RenderBox* box = element->renderBox();
    if (!box)
      return;

    adjustRoundBorderRadius(style, *box);
  }

  virtual bool paintPushButtonDecorations(const RenderObject& box, const PaintInfo& paintInfo, const IntRect& rect) override {
    GraphicsContextStateSaver stateSaver(paintInfo.context());
    FloatRect clip = addRoundedBorderClip(box, paintInfo.context(), rect);

    PlatformGraphicsContext* context = paintInfo.context().platformContext();
    PlatformCanvas canvas = context->canvas();
    if (box.style().visitedDependentColor(CSSPropertyBackgroundColor).isDark())
      drawAxialGradient(canvas, getConvexGradient(), clip.location(), FloatPoint(clip.x(), clip.maxY()), clip);
    else {
      drawAxialGradient(canvas, getShadeGradient(), clip.location(), FloatPoint(clip.x(), clip.maxY()), clip);
      drawAxialGradient(canvas, getShineGradient(), FloatPoint(clip.x(), clip.maxY()), clip.location(), clip);
    }
    return false;
  }

  virtual bool paintButtonDecorations(const RenderObject& box, const PaintInfo& paintInfo, const IntRect& rect) override {
    return paintPushButtonDecorations(box, paintInfo, rect);
  }

  virtual void adjustSearchFieldStyle(StyleResolver& selector, RenderStyle& style, const Element* element) const override
  {
    RenderTheme::adjustSearchFieldStyle(selector, style, element);

    if (!element)
      return;

    if (!style.hasBorder())
      return;

    RenderBox* box = element->renderBox();
    if (!box)
      return;

    adjustRoundBorderRadius(style, *box);
  }

  virtual bool paintSearchFieldDecorations(const RenderObject& box, const PaintInfo& paintInfo, const IntRect& rect) override
  {
    return paintTextFieldDecorations(box, paintInfo, rect);
  }

  // Text fields
  virtual bool paintTextField(const RenderObject&, const PaintInfo&, const FloatRect&) override { return true; }
  virtual bool paintTextFieldDecorations(const RenderObject& box, const PaintInfo& paintInfo, const FloatRect& rect) override {
    auto& style = box.style();
    FloatPoint point(rect.x() + style.borderLeftWidth(), rect.y() + style.borderTopWidth());

    GraphicsContextStateSaver stateSaver(paintInfo.context());

    //paintInfo.context().clipRoundedRect(style.getRoundedBorderFor(LayoutRect(rect)).pixelSnappedRoundedRectForPainting(box.document().deviceScaleFactor()));
    FloatRect clip = addRoundedBorderClip(box, paintInfo.context(), IntRect(rect));
    PlatformGraphicsContext* context = paintInfo.context().platformContext();
    PlatformCanvas canvas = context->canvas();

    // This gradient gets drawn black when printing.
    // Do not draw the gradient if there is no visible top border.
    bool topBorderIsInvisible = !style.hasBorder() || !style.borderTopWidth() || style.borderTopIsTransparent();
    if (!box.view().printing() && !topBorderIsInvisible)
      drawAxialGradient(canvas, getInsetGradient(), point, FloatPoint(point.x(), point.y() + 3.0f), clip);
    return false;
  }

  // Text areas
  virtual bool paintTextArea(const RenderObject&, const PaintInfo&, const FloatRect&) override { return true; }
  virtual bool paintTextAreaDecorations(const RenderObject& box, const PaintInfo& paintInfo, const FloatRect& rect) override {
    return paintTextFieldDecorations(box, paintInfo, rect);
  }

  // Menu lists
  virtual bool paintMenuList(const RenderObject&, const PaintInfo&, const FloatRect&) override { return true; }
  virtual bool paintMenuListDecorations(const RenderObject&, const PaintInfo&, const IntRect&) override { return true; }

  // Progress bars
  virtual bool paintProgressBar(const RenderObject&, const PaintInfo&, const IntRect&) override { return true; }

  // Slider tracks
  virtual bool paintSliderTrack(const RenderObject&, const PaintInfo&, const IntRect&) override { return true; }

  // Slider thumbs
  virtual bool paintSliderThumb(const RenderObject&, const PaintInfo&, const IntRect&) override { return true; }
  virtual bool paintSliderThumbDecorations(const RenderObject&, const PaintInfo&, const IntRect&) override { return true; }
};

/*
RenderTheme& RenderTheme::singleton()
{
  static NeverDestroyed<Ref<RenderTheme>> theme(adoptRef(*new RenderThemeUltralight()));
  return theme.get();
}
*/

Ref<RenderTheme> RenderTheme::themeForPage(Page*) {
  static RenderThemeUltralight theme;
  return theme;
}

} // namespace WebCore
