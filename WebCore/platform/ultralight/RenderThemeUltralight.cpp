#include "config.h"
#include "RenderTheme.h"
#include "NeverDestroyed.h"

namespace WebCore {

class RenderThemeUltralight : public RenderTheme {
public:
  RenderThemeUltralight() {}
  virtual ~RenderThemeUltralight() {}

  virtual void updateCachedSystemFontDescription(CSSValueID systemFontID, FontCascadeDescription&) const {}
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
