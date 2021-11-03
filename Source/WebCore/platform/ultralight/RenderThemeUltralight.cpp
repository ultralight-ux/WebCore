#include "config.h"
#include "../graphics/ultralight/PlatformContextUltralight.h"
#include "CSSToLengthConversionData.h"
#include "FloatRoundedRect.h"
#include "HTMLInputElement.h"
#include "HTMLSelectElement.h"
#include "NodeRenderStyle.h"
#include "RenderBox.h"
#include "RenderObject.h"
#include "RenderProgress.h"
#include "RenderStyle.h"
#include "RenderTheme.h"
#include "RenderView.h"
#include "StringUltralight.h"
#include "UserAgentStyleSheets.h"
#include "ResourceLoaderUltralight.h"
#include <Ultralight/platform/Config.h>
#include <Ultralight/platform/FileSystem.h>
#include <Ultralight/platform/Platform.h>
#include <Ultralight/private/Paint.h>
#include <wtf/NeverDestroyed.h>

namespace WebCore {

#if OS(DARWIN)
static const float ControlBaseHeight = 20;
#else
// Buttons are slightly shorter on non-MacOS systems
static const float ControlBaseHeight = 17;
#endif
static const float ControlBaseFontSize = 11;

class RenderThemeUltralight : public RenderTheme {
protected:
    bool m_mediaControlsStyleSheetLoaded = false;
    String m_mediaControlsStyleSheet;
    bool m_mediaControlsScriptLoaded = false;
    String m_mediaControlsScript;
public:
    RenderThemeUltralight() {}
    virtual ~RenderThemeUltralight() {}

    virtual String extraDefaultStyleSheet() override
    {
        auto& config = ultralight::Platform::instance().config();
        WTF::String configStylesheet = Convert(config.user_stylesheet);

        WTF::String platformStylesheet;
#if !OS(DARWIN)
        platformStylesheet = String(themeWinUserAgentStyleSheet, sizeof(themeWinUserAgentStyleSheet));
#endif

        return platformStylesheet + configStylesheet;
    }

    virtual String extraQuirksStyleSheet() override
    {
        WTF::String platformStylesheet;
#if !OS(DARWIN)
        platformStylesheet = String(themeWinQuirksUserAgentStyleSheet, sizeof(themeWinQuirksUserAgentStyleSheet));
#endif
        return platformStylesheet;
    }

#if ENABLE(VIDEO)
    virtual String mediaControlsStyleSheet() override
    {
#if ENABLE(MEDIA_CONTROLS_SCRIPT)
        if (!m_mediaControlsStyleSheetLoaded) {
            m_mediaControlsStyleSheet = ResourceLoader::readFileToString("mediaControls.css"_s);
            m_mediaControlsStyleSheetLoaded = true;
        }
        return m_mediaControlsStyleSheet;
#else
        return emptyString();
#endif
    }

    virtual String modernMediaControlsStyleSheet() override { return String(); }
    virtual String extraMediaControlsStyleSheet() override { return String(); }
    virtual String mediaControlsScript() override
    {
#if ENABLE(MEDIA_CONTROLS_SCRIPT)
        if (!m_mediaControlsScriptLoaded) {
            StringBuilder scriptBuilder;
            scriptBuilder.append(ResourceLoader::readFileToString("mediaControlsLocalizedStrings.js"_s));
            scriptBuilder.append(ResourceLoader::readFileToString("mediaControls.js"_s));
            m_mediaControlsScript = scriptBuilder.toString();
            m_mediaControlsScriptLoaded = true;
        }
        return m_mediaControlsScript;
#else
        return emptyString();
#endif
    }
    virtual String mediaControlsBase64StringForIconNameAndType(const String&, const String&) override { return String(); }
    virtual String mediaControlsFormattedStringForDuration(double) override { return String(); }
#endif

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
        ColorPair(float start[4], float end[4])
        {
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

    static ColorPair* getSliderTrackGradient()
    {
        static float end[4] = { 132 / 255.0, 132 / 255.0, 132 / 255.0, 1 };
        static float start[4] = { 74 / 255.0, 77 / 255.0, 80 / 255.0, 1 };
        static NeverDestroyed<ColorPair> gradient(start, end);
        return &gradient.get();
    }

    static ColorPair* getReadonlySliderTrackGradient()
    {
        static float end[4] = { 132 / 255.0, 132 / 255.0, 132 / 255.0, 0.4 };
        static float start[4] = { 74 / 255.0, 77 / 255.0, 80 / 255.0, 0.4 };
        static NeverDestroyed<ColorPair> gradient(start, end);
        return &gradient.get();
    }

    static ColorPair* getSliderThumbOpaquePressedGradient()
    {
        static float end[4] = { 144 / 255.0, 144 / 255.0, 144 / 255.0, 1 };
        static float start[4] = { 55 / 255.0, 55 / 255.0, 55 / 255.0, 1 };
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

    static inline ultralight::Color sampleExponential(ultralight::vec4 a, ultralight::vec4 b, float t)
    {
        t *= t;
        ultralight::vec4 sampledColor = a * (1.0f - t) + b * t;
        return UltralightRGBA(sampledColor.x, sampledColor.y, sampledColor.z, sampledColor.w);
    }

    static ultralight::Gradient setupGradient(ColorPair* colorPair, bool simulateExponential)
    {
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
        } else {
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
        ultralight::RefPtr<ultralight::Path> path = ultralight::Path::Create();
        path->MoveTo(points[0]);
        for (unsigned i = 1; i < points.size(); ++i)
            path->LineTo(points[i]);

        ultralight::Paint paint;
        paint.color = UltralightRGBA(color.red(), color.green(), color.blue(), color.alpha());

        canvas->StrokePath(path, paint, strokeWidth, use_square_cap ? ultralight::kLineCap_Square : ultralight::kLineCap_Butt);
    }

    // Adjustments
    virtual int baselinePosition(const RenderBox& box) const override
    {
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

    virtual void setCheckboxSize(RenderStyle&) const override {}

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
            drawAxialGradient(canvas, getConcaveGradient(), clip.location(), FloatPoint { clip.x(), clip.maxY() }, clip, false);
#else
            FloatPoint bottomCenter { clip.x() + width / 2.0f, clip.maxY() };

            drawAxialGradient(canvas, getShadeGradient(), clip.location(), FloatPoint { clip.x(), clip.maxY() }, clip, false);
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
            } else {
                line.uncheckedAppend(ultralight::Point(clip.x() + 3.5, clip.center().y()));
                line.uncheckedAppend(ultralight::Point(clip.maxX() - 3.5, clip.center().y()));

                shadow.uncheckedAppend(shortened(line[0], line[1], lineWidth / 4.0f));
                shadow.uncheckedAppend(shortened(line[1], line[0], lineWidth / 4.0f));
            }

#if OS(DARWIN)
            lineWidth = std::max<float>(lineWidth, 1);
            drawJoinedLines(canvas, Vector<ultralight::Point> { WTFMove(shadow) }, true, lineWidth, Color { 0.0f, 0.0f, 0.0f, 0.7f });

            lineWidth = std::max<float>(std::min(width, height) * thicknessRatio, 1);
            drawJoinedLines(canvas, Vector<ultralight::Point> { WTFMove(line) }, false, lineWidth, Color { 1.0f, 1.0f, 1.0f, 240 / 255.0f });
#else
            lineWidth = std::max<float>(std::min(width, height) * 1.5f * thicknessRatio, 1);
            drawJoinedLines(canvas, Vector<ultralight::Point> { WTFMove(line) }, false, lineWidth, Color { 0.0f, 0.0f, 0.0f, 0.7f });
#endif
        } else {
            auto clip = addRoundedBorderClip(box, paintInfo.context(), rect);
            auto width = clip.width();
            auto height = clip.height();
            FloatPoint bottomCenter { clip.x() + width / 2.0f, clip.maxY() };

            drawAxialGradient(canvas, getShadeGradient(), clip.location(), FloatPoint { clip.x(), clip.maxY() }, clip, false);
            drawRadialGradient(canvas, getShineGradient(), bottomCenter, 0, bottomCenter, sqrtf((width * width) / 4.0f + height * height), clip, true);
        }
        return false;
    }

    // Radio buttons
    virtual void adjustRadioStyle(StyleResolver&, RenderStyle& style, const Element*) const override
    {
        if (!style.width().isIntrinsicOrAuto() && !style.height().isAuto())
            return;

        int size = std::max(style.computedFontPixelSize(), 10U);
        style.setWidth({ size, Fixed });
        style.setHeight({ size, Fixed });
        style.setBorderRadius({ size / 2, size / 2 });
    }

    virtual bool paintRadio(const RenderObject&, const PaintInfo&, const IntRect&) override { return true; }
    virtual void setRadioSize(RenderStyle&) const override {}
    virtual bool paintRadioDecorations(const RenderObject& box, const PaintInfo& paintInfo, const IntRect& rect) override
    {
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
        } else {
            auto clip = addRoundedBorderClip(box, paintInfo.context(), rect);
            drawShadeAndShineGradients(clip);
        }
        return false;
    }

    // Buttons
    virtual bool paintButton(const RenderObject&, const PaintInfo&, const IntRect&) override { return true; }
    virtual void setButtonSize(RenderStyle& style) const override
    {
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

    static void applyCommonButtonPaddingToStyle(RenderStyle& style, const Element& element)
    {
        Document& document = element.document();
        auto emSize = CSSPrimitiveValue::create(0.5, CSSPrimitiveValue::CSS_EMS);
        int pixels = emSize->computeLength<int>(CSSToLengthConversionData(&style, document.renderStyle(), document.renderView(), document.frame()->pageZoomFactor()));
        style.setPaddingBox(LengthBox(0, pixels, 0, pixels));
    }

    static void adjustSelectListButtonStyle(RenderStyle& style, const Element& element)
    {
        // Enforce "padding: 0 0.5em".
        applyCommonButtonPaddingToStyle(style, element);

        // Enforce "line-height: normal".
        style.setLineHeight(Length(-100.0, Percent));
    }

    static void adjustInputElementButtonStyle(RenderStyle& style, const HTMLInputElement& inputElement)
    {
        // Always Enforce "padding: 0 0.5em".
        applyCommonButtonPaddingToStyle(style, inputElement);
    }

    virtual void adjustMenuListButtonStyle(StyleResolver&, RenderStyle& style, const Element* element) const override
    {
        // Set the min-height to be at least MenuListMinHeight.
        if (style.height().isAuto())
            style.setMinHeight(Length(std::max(MenuListMinHeight, static_cast<int>(MenuListBaseHeight / MenuListBaseFontSize * style.fontDescription().computedSize())), Fixed));
        else
            style.setMinHeight(Length(MenuListMinHeight, Fixed));

        if (!element)
            return;

        // Enforce some default styles in the case that this is a non-multiple <select> element,
        // or a date input. We don't force these if this is just an element with
        // "-webkit-appearance: menulist-button".
        if (is<HTMLSelectElement>(*element) && !element->hasAttributeWithoutSynchronization(HTMLNames::multipleAttr))
            adjustSelectListButtonStyle(style, *element);
        else if (is<HTMLInputElement>(*element))
            adjustInputElementButtonStyle(style, downcast<HTMLInputElement>(*element));
    }

    virtual bool paintMenuListButtonDecorations(const RenderBox& box, const PaintInfo& paintInfo, const FloatRect& rect) override
    {

        PlatformGraphicsContext* context = paintInfo.context().platformContext();
        PlatformCanvas canvas = context->canvas();

        auto& style = box.style();
        bool isRTL = style.direction() == TextDirection::RTL;
        float borderTopWidth = style.borderTopWidth();
        FloatRect clip(rect.x() + style.borderLeftWidth(), rect.y() + style.borderTopWidth(), rect.width() - style.borderLeftWidth() - style.borderRightWidth(), rect.height() - style.borderTopWidth() - style.borderBottomWidth());

        float adjustLeft = 0.5;
        float adjustRight = 0.5;
        float adjustTop = 0.5;
        float adjustBottom = 0.5;

        // Paint title portion.
        {
            float leftInset = isRTL ? MenuListButtonPaddingAfter : 0;
            FloatRect titleClip(clip.x() + leftInset - adjustLeft, clip.y() - adjustTop, clip.width() - MenuListButtonPaddingAfter + adjustLeft, clip.height() + adjustTop + adjustBottom);

            GraphicsContextStateSaver stateSaver(paintInfo.context());

            FloatSize topLeftRadius;
            FloatSize topRightRadius;
            FloatSize bottomLeftRadius;
            FloatSize bottomRightRadius;

            if (isRTL) {
                topRightRadius = FloatSize(valueForLength(style.borderTopRightRadius().width, rect.width()) - style.borderRightWidth(), valueForLength(style.borderTopRightRadius().height, rect.height()) - style.borderTopWidth());
                bottomRightRadius = FloatSize(valueForLength(style.borderBottomRightRadius().width, rect.width()) - style.borderRightWidth(), valueForLength(style.borderBottomRightRadius().height, rect.height()) - style.borderBottomWidth());
            } else {
                topLeftRadius = FloatSize(valueForLength(style.borderTopLeftRadius().width, rect.width()) - style.borderLeftWidth(), valueForLength(style.borderTopLeftRadius().height, rect.height()) - style.borderTopWidth());
                bottomLeftRadius = FloatSize(valueForLength(style.borderBottomLeftRadius().width, rect.width()) - style.borderLeftWidth(), valueForLength(style.borderBottomLeftRadius().height, rect.height()) - style.borderBottomWidth());
            }

            paintInfo.context().clipRoundedRect(FloatRoundedRect(titleClip,
                topLeftRadius, topRightRadius,
                bottomLeftRadius, bottomRightRadius));

            drawAxialGradient(canvas, getShadeGradient(), titleClip.location(), FloatPoint(titleClip.x(), titleClip.maxY()), clip);
            drawAxialGradient(canvas, getShineGradient(), FloatPoint(titleClip.x(), titleClip.maxY()), titleClip.location(), clip, true);
        }

        // Draw the separator after the initial padding.

        float separatorPosition = isRTL ? (clip.x() + MenuListButtonPaddingAfter) : (clip.maxX() - MenuListButtonPaddingAfter);

        box.drawLineForBoxSide(paintInfo.context(), FloatRect(FloatPoint(separatorPosition - borderTopWidth, clip.y()), FloatPoint(separatorPosition, clip.maxY())), BSRight, style.visitedDependentColor(CSSPropertyBorderTopColor), style.borderTopStyle(), 0, 0);

        FloatRect buttonClip;
        if (isRTL)
            buttonClip = FloatRect(clip.x() - adjustTop, clip.y() - adjustTop, MenuListButtonPaddingAfter + adjustTop + adjustLeft, clip.height() + adjustTop + adjustBottom);
        else
            buttonClip = FloatRect(separatorPosition - adjustTop, clip.y() - adjustTop, MenuListButtonPaddingAfter + adjustTop + adjustRight, clip.height() + adjustTop + adjustBottom);

        // Now paint the button portion.
        {
            GraphicsContextStateSaver stateSaver(paintInfo.context());

            FloatSize topLeftRadius;
            FloatSize topRightRadius;
            FloatSize bottomLeftRadius;
            FloatSize bottomRightRadius;

            if (isRTL) {
                topLeftRadius = FloatSize(valueForLength(style.borderTopLeftRadius().width, rect.width()) - style.borderLeftWidth(), valueForLength(style.borderTopLeftRadius().height, rect.height()) - style.borderTopWidth());
                bottomLeftRadius = FloatSize(valueForLength(style.borderBottomLeftRadius().width, rect.width()) - style.borderLeftWidth(), valueForLength(style.borderBottomLeftRadius().height, rect.height()) - style.borderBottomWidth());
            } else {
                topRightRadius = FloatSize(valueForLength(style.borderTopRightRadius().width, rect.width()) - style.borderRightWidth(), valueForLength(style.borderTopRightRadius().height, rect.height()) - style.borderTopWidth());
                bottomRightRadius = FloatSize(valueForLength(style.borderBottomRightRadius().width, rect.width()) - style.borderRightWidth(), valueForLength(style.borderBottomRightRadius().height, rect.height()) - style.borderBottomWidth());
            }

            paintInfo.context().clipRoundedRect(FloatRoundedRect(buttonClip,
                topLeftRadius, topRightRadius,
                bottomLeftRadius, bottomRightRadius));

            paintInfo.context().fillRect(buttonClip, style.visitedDependentColor(CSSPropertyBorderTopColor));

            drawAxialGradient(canvas, isFocused(box) && !isReadOnlyControl(box) ? getConcaveGradient() : getConvexGradient(), buttonClip.location(), FloatPoint(buttonClip.x(), buttonClip.maxY()), clip);
        }

        // Paint Indicators.

        if (box.isMenuList() && downcast<HTMLSelectElement>(box.element())->multiple()) {
            int size = 2;
            int count = 3;
            int padding = 3;

            FloatRect ellipse(buttonClip.x() + (buttonClip.width() - count * (size + padding) + padding) / 2.0, buttonClip.maxY() - 10.0, size, size);

            for (int i = 0; i < count; ++i) {
                paintInfo.context().drawRaisedEllipse(ellipse, Color::white, Color(0.0f, 0.0f, 0.0f, 0.5f));
                ellipse.move(size + padding, 0);
            }
        } else {
            float centerX = floorf(buttonClip.x() + buttonClip.width() / 2.0) - 0.5;
            float centerY = floorf(buttonClip.y() + buttonClip.height() * 3.0 / 8.0);

            Vector<FloatPoint> arrow = {
                { centerX - MenuListArrowWidth / 2, centerY },
                { centerX + MenuListArrowWidth / 2, centerY },
                { centerX, centerY + MenuListArrowHeight }
            };

            Vector<FloatPoint> shadow = {
                { arrow[0].x(), arrow[0].y() + 1 },
                { arrow[1].x(), arrow[1].y() + 1 },
                { arrow[2].x(), arrow[2].y() + 1 }
            };

            float opacity = isReadOnlyControl(box) ? 0.2 : 0.5;

            WebCore::Path shadowPath = Path::polygonPathFromPoints(shadow);
            ultralight::Paint paint;
            paint.color = UltralightRGBA(0, 0, 0, 255 * opacity);
            canvas->FillPath(shadowPath.ultralightPath(), paint, ultralight::kFillRule_EvenOdd);

            WebCore::Path arrowPath = Path::polygonPathFromPoints(arrow);
            paint.color = UltralightColorWHITE;
            canvas->FillPath(arrowPath.ultralightPath(), paint, ultralight::kFillRule_EvenOdd);
        }

        return false;
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

    virtual bool paintPushButtonDecorations(const RenderObject& box, const PaintInfo& paintInfo, const IntRect& rect) override
    {
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

    virtual bool paintButtonDecorations(const RenderObject& box, const PaintInfo& paintInfo, const IntRect& rect) override
    {
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
    virtual bool paintTextFieldDecorations(const RenderObject& box, const PaintInfo& paintInfo, const FloatRect& rect) override
    {
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
    virtual bool paintTextAreaDecorations(const RenderObject& box, const PaintInfo& paintInfo, const FloatRect& rect) override
    {
        return paintTextFieldDecorations(box, paintInfo, rect);
    }

    const int MenuListMinHeight = 15;

    const float MenuListBaseHeight = 20;
    const float MenuListBaseFontSize = 11;

    const float MenuListArrowWidth = 7;
    const float MenuListArrowHeight = 6;
    const float MenuListButtonPaddingAfter = 19;

    virtual LengthBox popupInternalPaddingBox(const RenderStyle& style) const override
    {
        if (style.appearance() == MenulistButtonPart) {
            if (style.direction() == TextDirection::RTL)
                return { 0, 0, 0, static_cast<int>(MenuListButtonPaddingAfter + style.borderTopWidth()) };
            return { 0, static_cast<int>(MenuListButtonPaddingAfter + style.borderTopWidth()), 0, 0 };
        }
        return { 0, 0, 0, 0 };
    }

    // Menu lists
    virtual bool paintMenuList(const RenderObject&, const PaintInfo&, const FloatRect&) override { return true; }
    virtual bool paintMenuListDecorations(const RenderObject&, const PaintInfo&, const IntRect&) override { return true; }

    virtual Seconds animationRepeatIntervalForProgressBar(RenderProgress&) const override
    {
        return 0_s;
    }

    virtual Seconds animationDurationForProgressBar(RenderProgress&) const override
    {
        return 0_s;
    }

    // Progress bars
    virtual bool paintProgressBar(const RenderObject& renderer, const PaintInfo& paintInfo, const IntRect& rect) override
    {
        if (!is<RenderProgress>(renderer))
            return true;

        const int progressBarHeight = 9;
        const float verticalOffset = (rect.height() - progressBarHeight) / 2.0;

        PlatformGraphicsContext* ctx = paintInfo.context().platformContext();
        PlatformCanvas canvas = ctx->canvas();

        GraphicsContextStateSaver stateSaver(paintInfo.context());
        if (rect.width() < 10 || rect.height() < 9) {
            // The rect is smaller than the standard progress bar. We clip to the element's rect to avoid
            // leaking pixels outside the repaint rect.
            paintInfo.context().clip(rect);
        }

        // 1) Draw the progress bar track.
        // 1.1) Draw the light grey background with grey border.
        GraphicsContext& context = paintInfo.context();

        const float verticalRenderingPosition = rect.y() + verticalOffset;

        Path trackPath;
        FloatRect trackRect(rect.x() + 0.25, verticalRenderingPosition + 0.25, rect.width() - 0.5, progressBarHeight - 0.5);
        FloatSize roundedCornerRadius(5, 4);
        trackPath.addRoundedRect(trackRect, roundedCornerRadius);

        ultralight::Paint paint;
        paint.color = UltralightRGBA(240, 240, 240, 255);
        canvas->FillPath(trackPath.ultralightPath(), paint, ultralight::kFillRule_EvenOdd);
        paint.color = UltralightRGBA(220, 220, 220, 255);
        canvas->StrokePath(trackPath.ultralightPath(), paint, 1.0f);

        FloatRect border(rect.x(), rect.y() + verticalOffset, rect.width(), progressBarHeight);
        paintInfo.context().clipRoundedRect(FloatRoundedRect(border, roundedCornerRadius, roundedCornerRadius, roundedCornerRadius, roundedCornerRadius));

        const auto& renderProgress = downcast<RenderProgress>(renderer);
        if (renderProgress.isDeterminate()) {
            // 2) Draw the progress bar.
            double position = clampTo(renderProgress.position(), 0.0, 1.0);
            double barWidth = position * rect.width();

            Path barPath;
            int left = rect.x();
            if (!renderProgress.style().isLeftToRightDirection())
                left = rect.maxX() - barWidth;
            FloatRect barRect(left + 0.25, verticalRenderingPosition + 0.25, std::max(barWidth - 0.5, 0.0), progressBarHeight - 0.5);
            barPath.addRoundedRect(barRect, roundedCornerRadius);

            // Blue for progress indicator
            paint.color = UltralightRGBA(0, 151, 255, 255);
            canvas->FillPath(barPath.ultralightPath(), paint, ultralight::kFillRule_EvenOdd);
        }

        return false;
    }

    const double kTrackThickness = 4.0;
    const double kTrackRadius = kTrackThickness / 2.0;
    const int kDefaultSliderThumbSize = 16;

    virtual void adjustSliderTrackStyle(StyleResolver& selector, RenderStyle& style, const Element* element) const override
    {
        RenderTheme::adjustSliderTrackStyle(selector, style, element);

        // FIXME: We should not be relying on border radius for the appearance of our controls <rdar://problem/7675493>.
        int radius = static_cast<int>(kTrackRadius);
        style.setBorderRadius({ { radius, Fixed }, { radius, Fixed } });
    }

    // Slider tracks
    virtual bool paintSliderTrack(const RenderObject& box, const PaintInfo& paintInfo, const IntRect& rect) override
    {
        IntRect trackClip = rect;
        auto& style = box.style();

        PlatformGraphicsContext* context = paintInfo.context().platformContext();
        PlatformCanvas canvas = context->canvas();

        bool isHorizontal = true;
        switch (style.appearance()) {
        case SliderHorizontalPart:
            isHorizontal = true;
            // Inset slightly so the thumb covers the edge.
            if (trackClip.width() > 2) {
                trackClip.setWidth(trackClip.width() - 2);
                trackClip.setX(trackClip.x() + 1);
            }
            trackClip.setHeight(static_cast<int>(kTrackThickness));
            trackClip.setY(rect.y() + rect.height() / 2 - kTrackThickness / 2);
            break;
        case SliderVerticalPart:
            isHorizontal = false;
            // Inset slightly so the thumb covers the edge.
            if (trackClip.height() > 2) {
                trackClip.setHeight(trackClip.height() - 2);
                trackClip.setY(trackClip.y() + 1);
            }
            trackClip.setWidth(kTrackThickness);
            trackClip.setX(rect.x() + rect.width() / 2 - kTrackThickness / 2);
            break;
        default:
            ASSERT_NOT_REACHED();
        }

        ASSERT(trackClip.width() >= 0);
        ASSERT(trackClip.height() >= 0);
        double cornerWidth = trackClip.width() < kTrackThickness ? trackClip.width() / 2.0f : kTrackRadius;
        double cornerHeight = trackClip.height() < kTrackThickness ? trackClip.height() / 2.0f : kTrackRadius;

        bool readonly = isReadOnlyControl(box);

#if ENABLE(DATALIST_ELEMENT)
        paintSliderTicks(box, paintInfo, trackClip);
#endif

        IntSize cornerSize(cornerWidth, cornerHeight);
        FloatRoundedRect innerBorder(trackClip, cornerSize, cornerSize, cornerSize, cornerSize);

        // Draw the track gradient.
        {
            GraphicsContextStateSaver stateSaver(paintInfo.context());
            paintInfo.context().clipRoundedRect(innerBorder);

            if (isHorizontal)
                drawAxialGradient(canvas, readonly ? getReadonlySliderTrackGradient() : getSliderTrackGradient(), trackClip.location(), FloatPoint(trackClip.x(), trackClip.maxY()), trackClip);
            else
                drawAxialGradient(canvas, readonly ? getReadonlySliderTrackGradient() : getSliderTrackGradient(), trackClip.location(), FloatPoint(trackClip.maxX(), trackClip.y()), trackClip);
        }

        // Draw the track border.
        {
            GraphicsContextStateSaver stateSaver(paintInfo.context());

            Path trackPath;
            trackPath.addRoundedRect(innerBorder);
            ultralight::Paint paint;

            if (readonly)
                paint.color = UltralightRGBA(178, 178, 178, 255);
            else
                paint.color = UltralightRGBA(76, 76, 76, 255);

            canvas->StrokePath(trackPath.ultralightPath(), paint, 1.0f);
        }

        return false;
    }

    // Slider thumbs
    virtual bool paintSliderThumb(const RenderObject&, const PaintInfo&, const IntRect&) override { return true; }

    virtual void adjustSliderThumbSize(RenderStyle& style, const Element*) const override
    {
        if (style.appearance() != SliderThumbHorizontalPart && style.appearance() != SliderThumbVerticalPart)
            return;

        // Enforce "border-radius: 50%".
        style.setBorderRadius({ { 50, Percent }, { 50, Percent } });

        // Enforce a 16x16 size if no size is provided.
        if (style.width().isIntrinsicOrAuto() || style.height().isAuto()) {
            style.setWidth({ kDefaultSliderThumbSize, Fixed });
            style.setHeight({ kDefaultSliderThumbSize, Fixed });
        }
    }

    virtual bool paintSliderThumbDecorations(const RenderObject& box, const PaintInfo& paintInfo, const IntRect& rect) override
    {
        GraphicsContextStateSaver stateSaver(paintInfo.context());
        FloatRect clip = addRoundedBorderClip(box, paintInfo.context(), rect);

        PlatformGraphicsContext* context = paintInfo.context().platformContext();
        PlatformCanvas canvas = context->canvas();

        FloatPoint bottomCenter(clip.x() + clip.width() / 2.0f, clip.maxY());
        if (isPressed(box))
            drawAxialGradient(canvas, getSliderThumbOpaquePressedGradient(), clip.location(), FloatPoint(clip.x(), clip.maxY()), clip);
        else {
            drawAxialGradient(canvas, getShadeGradient(), clip.location(), FloatPoint(clip.x(), clip.maxY()), clip);
            drawRadialGradient(canvas, getShineGradient(), bottomCenter, 0.0f, bottomCenter, std::max(clip.width(), clip.height()), clip, true);
        }

        return false;
    }
};

/*
RenderTheme& RenderTheme::singleton()
{
  static NeverDestroyed<Ref<RenderTheme>> theme(adoptRef(*new RenderThemeUltralight()));
  return theme.get();
}
*/

RenderTheme& RenderTheme::singleton()
{
    static NeverDestroyed<RenderThemeUltralight> theme;
    return theme;
}

} // namespace WebCore
