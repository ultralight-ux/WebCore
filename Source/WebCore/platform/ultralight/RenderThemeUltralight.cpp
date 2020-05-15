#include "config.h"
#include "RenderTheme.h"
#include <wtf/NeverDestroyed.h>
#include <Ultralight/platform/Platform.h>
#include <Ultralight/platform/Config.h>
#include <Ultralight/private/Paint.h>
#include "StringUltralight.h"
#include "../graphics/ultralight/PlatformContextUltralight.h"
#include "RenderStyle.h"
#include "RenderObject.h"
#include "RenderView.h"
#include "FloatRoundedRect.h"
#include "RenderBox.h"
#include "CSSToLengthConversionData.h"
#include "UserAgentStyleSheets.h"

namespace WebCore {

#if OS(DARWIN)
static const float ControlBaseHeight = 20;
#else
// Buttons are slightly shorter on non-MacOS systems
static const float ControlBaseHeight = 17;
#endif
static const float ControlBaseFontSize = 11;

class RenderThemeUltralight : public RenderTheme {
public:
  RenderThemeUltralight() {}
  virtual ~RenderThemeUltralight() {}

  virtual String extraDefaultStyleSheet() override { 
    auto& config = ultralight::Platform::instance().config();
    WTF::String configStylesheet = Convert(config.user_stylesheet);

    WTF::String platformStylesheet;
#if !OS(DARWIN)
    platformStylesheet = String(themeWinUserAgentStyleSheet, sizeof(themeWinUserAgentStyleSheet));
#endif

    return platformStylesheet + configStylesheet;
  }

  virtual String extraQuirksStyleSheet() override { 
    WTF::String platformStylesheet; 
#if !OS(DARWIN)
    platformStylesheet = String(themeWinQuirksUserAgentStyleSheet, sizeof(themeWinQuirksUserAgentStyleSheet));
#endif
    return platformStylesheet;
  }

  // Force WebCore to draw the focus ring
  virtual bool supportsFocusRing(const RenderStyle&) const { return false; }

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

  virtual Color platformActiveSelectionBackgroundColor(OptionSet<StyleColor::Options>) const override { return Color(0, 151, 255); }

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

  static inline ultralight::Color sampleExponential(ultralight::vec4 a, ultralight::vec4 b, float t) {
    t *= t;
    ultralight::vec4 sampledColor = a * (1.0f - t) + b * t;
    return UltralightRGBA(sampledColor.x, sampledColor.y, sampledColor.z, sampledColor.w);
  }

  static ultralight::Gradient setupGradient(ColorPair* colorPair, bool simulateExponential) {
    ultralight::Gradient gradient;
    if (simulateExponential) {
      constexpr size_t num_samples = 7;
      gradient.num_stops = num_samples;
      ultralight::vec4 colorA = UltralightColorGetFloat4(colorPair->color1);
      ultralight::vec4 colorB = UltralightColorGetFloat4(colorPair->color2);

      for (size_t i = 0; i < num_samples; ++i) {
        float t = i / (num_samples - 1.0f);
        gradient.stops[i].color = sampleExponential(colorA, colorB, t);
        gradient.stops[i].stop = t;
      }
    }
    else {
      gradient.num_stops = 2;
      gradient.stops[0].color = colorPair->color1;
      gradient.stops[0].stop = 0.0f;
      gradient.stops[1].color = colorPair->color2;
      gradient.stops[1].stop = 1.0f;
    }
    return gradient;
  }

  static void drawAxialGradient(PlatformCanvas canvas, ColorPair* colorPair, const FloatPoint& startPoint, const FloatPoint& stopPoint, const FloatRect& clip, bool simulateExponential = false)
  {
    ultralight::Gradient gradient = setupGradient(colorPair, simulateExponential);
    gradient.is_radial = false;
    gradient.p0 = ultralight::Point(startPoint.x(), startPoint.y());
    gradient.p1 = ultralight::Point(stopPoint.x(), stopPoint.y());

    canvas->DrawGradient(&gradient, clip);
  }

  static void drawRadialGradient(PlatformCanvas canvas, ColorPair* colorPair, const FloatPoint& startPoint, float startRadius, const FloatPoint& stopPoint, float stopRadius, const FloatRect& clip, bool simulateExponential = false)
  {
    ultralight::Gradient gradient = setupGradient(colorPair, simulateExponential);
    gradient.is_radial = true;
    gradient.p0 = ultralight::Point(startPoint.x(), startPoint.y());
    gradient.p1 = ultralight::Point(stopPoint.x(), stopPoint.y());
    gradient.r0 = startRadius;
    gradient.r1 = stopRadius;

    canvas->DrawGradient(&gradient, clip);
  }

  static void drawJoinedLines(PlatformCanvas canvas, const Vector<ultralight::Point>& points, bool use_square_cap, float strokeWidth, Color color)
  {
    ultralight::Ref<ultralight::Path> path = ultralight::Path::Create();
    path->MoveTo(points[0]);
    for (unsigned i = 1; i < points.size(); ++i)
      path->LineTo(points[i]);

    ultralight::Paint paint;
    paint.color = UltralightRGBA(color.red(), color.green(), color.blue(), color.alpha());

    canvas->StrokePath(path, paint, strokeWidth, use_square_cap ? ultralight::kLineCap_Square : ultralight::kLineCap_Butt);
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
      return !style.visitedDependentColor(CSSPropertyBackgroundColor).isVisible() || style.backgroundLayers().hasImage();

    if (style.appearance() == TextFieldPart || style.appearance() == TextAreaPart)
      return style.backgroundLayers() != background;

    return RenderTheme::isControlStyled(style, border, background, backgroundColor);
  }

  // Check boxes

  virtual void adjustCheckboxStyle(StyleResolver&, RenderStyle& style, const Element*) const override
  {
    if (!style.width().isIntrinsicOrAuto() && !style.height().isAuto())
      return;

    int size = std::max(style.computedFontPixelSize(), 10U);
    style.setWidth({ size, Fixed });
    style.setHeight({ size, Fixed });
  }

  virtual bool paintCheckbox(const RenderObject&, const PaintInfo&, const IntRect&) override { return true; }

  virtual void setCheckboxSize(RenderStyle&) const override { }

  virtual bool paintCheckboxDecorations(const RenderObject& box, const PaintInfo& paintInfo, const IntRect& rect) override
  {
    bool checked = isChecked(box);
    bool indeterminate = isIndeterminate(box);
    PlatformGraphicsContext* context = paintInfo.context().platformContext();
    PlatformCanvas canvas = context->canvas();
    GraphicsContextStateSaver stateSaver(paintInfo.context());

    if (checked || indeterminate) {
      auto border = box.style().getRoundedBorderFor(rect);
#if OS(DARWIN)
      paintInfo.context().fillRoundedRect(border.pixelSnappedRoundedRectForPainting(box.document().deviceScaleFactor()), Color(0.0f, 0.0f, 0.0f, 0.8f));
#endif

      auto clip = addRoundedBorderClip(box, paintInfo.context(), rect);
      auto width = clip.width();
      auto height = clip.height();
#if OS(DARWIN)
      drawAxialGradient(canvas, getConcaveGradient(), clip.location(), FloatPoint{ clip.x(), clip.maxY() }, clip, false);
#else
      FloatPoint bottomCenter{ clip.x() + width / 2.0f, clip.maxY() };

      drawAxialGradient(canvas, getShadeGradient(), clip.location(), FloatPoint{ clip.x(), clip.maxY() }, clip, false);
      drawRadialGradient(canvas, getShineGradient(), bottomCenter, 0, bottomCenter, sqrtf((width * width) / 4.0f + height * height), clip, true);
#endif

      constexpr float thicknessRatio = 2 / 14.0;
      float lineWidth = std::min(width, height) * 2.0f * thicknessRatio;

      Vector<ultralight::Point, 3> line;
      Vector<ultralight::Point, 3> shadow;
      if (checked) {
        FloatSize size(14.0f, 14.0f);
        ultralight::Point pathRatios[] = {
            { 2.5f / size.width(), 7.5f / size.height() },
            { 5.5f / size.width(), 10.5f / size.height() },
            { 11.5f / size.width(), 2.5f / size.height() }
        };

        line.uncheckedAppend(ultralight::Point(clip.x() + width * pathRatios[0].x, clip.y() + height * pathRatios[0].y));
        line.uncheckedAppend(ultralight::Point(clip.x() + width * pathRatios[1].x, clip.y() + height * pathRatios[1].y));
        line.uncheckedAppend(ultralight::Point(clip.x() + width * pathRatios[2].x, clip.y() + height * pathRatios[2].y));

        shadow.uncheckedAppend(shortened(line[0], line[1], lineWidth / 4.0f));
        shadow.uncheckedAppend(line[1]);
        shadow.uncheckedAppend(shortened(line[2], line[1], lineWidth / 4.0f));
      }
      else {
        line.uncheckedAppend(ultralight::Point(clip.x() + 3.5, clip.center().y()));
        line.uncheckedAppend(ultralight::Point(clip.maxX() - 3.5, clip.center().y()));

        shadow.uncheckedAppend(shortened(line[0], line[1], lineWidth / 4.0f));
        shadow.uncheckedAppend(shortened(line[1], line[0], lineWidth / 4.0f));
      }

#if OS(DARWIN)
      lineWidth = std::max<float>(lineWidth, 1);
      drawJoinedLines(canvas, Vector<ultralight::Point> { WTFMove(shadow) }, true, lineWidth, Color{ 0.0f, 0.0f, 0.0f, 0.7f });

      lineWidth = std::max<float>(std::min(width, height) * thicknessRatio, 1);
      drawJoinedLines(canvas, Vector<ultralight::Point> { WTFMove(line) }, false, lineWidth, Color{ 1.0f, 1.0f, 1.0f, 240 / 255.0f });
#else
      lineWidth = std::max<float>(std::min(width, height) * 1.5f * thicknessRatio, 1);
      drawJoinedLines(canvas, Vector<ultralight::Point> { WTFMove(line) }, false, lineWidth, Color{ 0.0f, 0.0f, 0.0f, 0.7f });
#endif
    }
    else {
      auto clip = addRoundedBorderClip(box, paintInfo.context(), rect);
      auto width = clip.width();
      auto height = clip.height();
      FloatPoint bottomCenter{ clip.x() + width / 2.0f, clip.maxY() };

      drawAxialGradient(canvas, getShadeGradient(), clip.location(), FloatPoint{ clip.x(), clip.maxY() }, clip, false);
      drawRadialGradient(canvas, getShineGradient(), bottomCenter, 0, bottomCenter, sqrtf((width * width) / 4.0f + height * height), clip, true);
    }
    return false;
  }

  // Radio buttons
  virtual void adjustRadioStyle(StyleResolver&, RenderStyle& style, const Element*) const override { 
    if (!style.width().isIntrinsicOrAuto() && !style.height().isAuto())
      return;

    int size = std::max(style.computedFontPixelSize(), 10U);
    style.setWidth({ size, Fixed });
    style.setHeight({ size, Fixed });
    style.setBorderRadius({ size / 2, size / 2 });
  }

  virtual bool paintRadio(const RenderObject&, const PaintInfo&, const IntRect&) override { return true; }
  virtual void setRadioSize(RenderStyle&) const override { }
  virtual bool paintRadioDecorations(const RenderObject& box, const PaintInfo& paintInfo, const IntRect& rect) override { 
    GraphicsContextStateSaver stateSaver(paintInfo.context());
    PlatformGraphicsContext* context = paintInfo.context().platformContext();
    PlatformCanvas canvas = context->canvas();

    auto drawShadeAndShineGradients = [&](auto clip) {
      FloatPoint bottomCenter(clip.x() + clip.width() / 2.0, clip.maxY());
      drawAxialGradient(canvas, getShadeGradient(), clip.location(), FloatPoint(clip.x(), clip.maxY()), clip);
      drawRadialGradient(canvas, getShineGradient(), bottomCenter, 0, bottomCenter, std::max(clip.width(), clip.height()), clip, true);
    };
    
    if (isChecked(box)) {
      auto border = box.style().getRoundedBorderFor(rect);
#if OS(DARWIN)
      paintInfo.context().fillRoundedRect(border.pixelSnappedRoundedRectForPainting(box.document().deviceScaleFactor()), Color(0.0f, 0.0f, 0.0f, 0.8f));
#endif

      auto clip = addRoundedBorderClip(box, paintInfo.context(), rect);
#if OS(DARWIN)
      drawAxialGradient(canvas, getConcaveGradient(), clip.location(), FloatPoint(clip.x(), clip.maxY()), clip);
#else
      drawShadeAndShineGradients(clip);
#endif

#if OS(DARWIN)
      // The inner circle is 6 / 14 the size of the surrounding circle, 
      // leaving 8 / 14 around it. (8 / 14) / 2 = 2 / 7.
      static const float InnerInverseRatio = 2 / 7.0;
#else
      static const float InnerInverseRatio = 2 / 9.0;
#endif

      clip.inflateX(-clip.width() * InnerInverseRatio);
      clip.inflateY(-clip.height() * InnerInverseRatio);

#if OS(DARWIN)
      paintInfo.context().drawRaisedEllipse(clip, Color::white, shadowColor());

      FloatSize radius(clip.width() / 2.0f, clip.height() / 2.0f);
      paintInfo.context().clipRoundedRect(FloatRoundedRect(clip, radius, radius, radius, radius));
      drawShadeAndShineGradients(clip);
#else
      paintInfo.context().setFillColor(Color(0.0f, 0.0f, 0.0f, 0.7f));
      paintInfo.context().drawEllipse(clip);
#endif
    }
    else {
      auto clip = addRoundedBorderClip(box, paintInfo.context(), rect);
      drawShadeAndShineGradients(clip);
    }
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
    if (style.appearance() == NoControlPart || style.backgroundLayers().hasImage())
      return;

    // FIXME: We should not be relying on border radius for the appearance of our controls <rdar://problem/7675493>.
    style.setBorderRadius({ { std::min(box.width(), box.height()) / 2, Fixed }, { box.height() / 2, Fixed } });
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
    bool flip = isPressed(box);
    FloatPoint start = flip ? FloatPoint(clip.x(), clip.maxY()) : clip.location();
    FloatPoint end = flip ? clip.location() : FloatPoint(clip.x(), clip.maxY());
    if (box.style().visitedDependentColor(CSSPropertyBackgroundColor).isDark())
      drawAxialGradient(canvas, getConvexGradient(), start, end, clip);
    else {
      drawAxialGradient(canvas, getShadeGradient(), start, end, clip);
      drawAxialGradient(canvas, getShineGradient(), end, start, clip);
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
#if OS(DARWIN)
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
#endif
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

RenderTheme& RenderTheme::singleton() {
  static NeverDestroyed<RenderThemeUltralight> theme;
  return theme;
}

} // namespace WebCore
