#include "config.h"
#include "RenderTheme.h"
#include "NeverDestroyed.h"
#include <Ultralight/platform/Platform.h>
#include <Ultralight/platform/Config.h>
#include "WebCore/platform/ultralight/StringUltralight.h"

namespace WebCore {

class RenderThemeUltralight : public RenderTheme {
public:
  RenderThemeUltralight() {}
  virtual ~RenderThemeUltralight() {}

  virtual String extraDefaultStyleSheet() { 
    auto& config = ultralight::Platform::instance().config();
    return Convert(config.user_stylesheet);
  }

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
