#include "config.h"
#include "EventLoop.h"
#include "NotImplemented.h"
#include <JavaScriptCore/JavaScript.h>

namespace WebCore {

  void EventLoop::cycle()
  {
    // TODO
    notImplemented();
  }

} // namespace WebCore

// For whatever crazy reason, if we don't reference these symbols MSVC doesn't export them in DLL.
void PreserveSymbols() {
  JSEvaluateScript(0, 0, 0, 0, 0, 0);
  JSCheckScriptSyntax(0, 0, 0, 0, 0);
  JSGarbageCollect(0);
}
