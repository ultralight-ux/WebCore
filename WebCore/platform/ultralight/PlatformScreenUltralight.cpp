#include "config.h"
#include "PlatformScreen.h"
#include "NotImplemented.h"
#include "FloatRect.h"

namespace WebCore {

int screenDepth(Widget*) {
  // TODO
  notImplemented();
  return 0;
}

int screenDepthPerComponent(Widget*) {
  // TODO
  notImplemented();
  return 0;
}

bool screenIsMonochrome(Widget*) {
  // TODO
  notImplemented();
  return false;
}

bool screenHasInvertedColors() {
  // TODO
  notImplemented();
  return false;
}

FloatRect screenRect(Widget*) {
  // TODO
  notImplemented();
  return FloatRect();
}

FloatRect screenAvailableRect(Widget*) {
  // TODO
  notImplemented();
  return FloatRect();
}

bool screenSupportsExtendedColor(Widget*) {
  // TODO
  notImplemented();
  return false;
}

} // namespace WebCore
