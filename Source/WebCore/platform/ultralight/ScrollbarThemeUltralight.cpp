#include "config.h"
#include "ScrollbarTheme.h"
#include "NotImplemented.h"
#include "ScrollbarThemeComposite.h"
#include "Scrollbar.h"
#include "GraphicsContext.h"
#include "FloatRoundedRect.h"

namespace WebCore {

//static const int thickness = 16;
static const int thickness = 12;
static const RGBA32 trackColor = 0xFFE6E6E6;
static bool enableButtons = false;

class ScrollbarThemeUltralight : public ScrollbarThemeComposite {
public:
protected:
  bool hasButtons(Scrollbar&) override { return enableButtons; }

  bool hasThumb(Scrollbar&) override { return true; }

  virtual int scrollbarThickness(ScrollbarControlSize = RegularScrollbar, ScrollbarExpansionState = ScrollbarExpansionState::Expanded) override { return thickness; }

  IntRect backButtonRect(Scrollbar& scrollbar, ScrollbarPart part, bool painting) override {
    if (!enableButtons || part != BackButtonStartPart)
      return IntRect();

    return IntRect(scrollbar.x(), scrollbar.y(), scrollbarThickness(), scrollbarThickness());
  }

  IntRect forwardButtonRect(Scrollbar& scrollbar, ScrollbarPart part, bool painting) override {
    if (!enableButtons || part != ForwardButtonStartPart)
      return IntRect();

    int x = scrollbar.x();
    int y = scrollbar.y();
    int thickness = scrollbarThickness();
    if (scrollbar.orientation() == ScrollbarOrientation::VerticalScrollbar)
      y += scrollbar.height() - thickness;
    else
      x += scrollbar.width() - thickness;
    
    return IntRect(x, y, thickness, thickness);
  }

  IntRect trackRect(Scrollbar& scrollbar, bool painting = false) override {
    if (!enableButtons)
      return scrollbar.frameRect();

    int thickness = scrollbarThickness();
    if (scrollbar.orientation() == HorizontalScrollbar) {
      return IntRect(scrollbar.x() + thickness, scrollbar.y(), scrollbar.width() - 2 * thickness, thickness);
    }
    return IntRect(scrollbar.x(), scrollbar.y() + thickness, thickness, scrollbar.height() - 2 * thickness);
  }

  void paintTrackBackground(GraphicsContext& context, Scrollbar& scrollbar, const IntRect& trackRect) override {
    if (scrollbar.enabled()) {
      //context.fillRect(trackRect, trackColor);
    }
  }

  void paintButton(GraphicsContext& context, Scrollbar& scrollbar, const IntRect& buttonRect, ScrollbarPart part) override {
    if (scrollbar.enabled()) {
      //if (part == BackButtonStartPart || part == ForwardButtonStartPart)
      //  context.fillRect(buttonRect, trackColor);
    }
  }

  void paintThumb(GraphicsContext& context, Scrollbar& scrollbar, const IntRect& thumbRect) override {
    if (scrollbar.enabled()) {
      IntRect r = thumbRect;
      r.contract(6, 6);
      r.move(3, 3);
      float minDimension = std::min(r.width(), r.height());
      FloatRoundedRect rrect(r, FloatRoundedRect::Radii(minDimension * 0.5f));
      context.fillRoundedRect(rrect, Color(0.2f, 0.2f, 0.2f, 0.6f));
    }
  }

  int maxOverlapBetweenPages() override { return 40; }

  bool usesOverlayScrollbars() const override {
    return true;
  }

private:
  bool isMockTheme() const override { return false; }
};

ScrollbarTheme& ScrollbarTheme::nativeTheme() {
  static ScrollbarThemeUltralight platformTheme;
  return platformTheme;
}

} // namespace WebCore
