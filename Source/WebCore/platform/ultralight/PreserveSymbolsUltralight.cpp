#include "config.h"
#include "JavaScriptCore/JavaScript.h"

namespace WebCore {

// TODO: This shouldn't be necessary, for whatever reason MSVC isn't re-exporting certain
// JavaScriptCore symbols in the WebCore DLL.
WEBCORE_EXPORT void PreserveSymbols() {
  JSContextGroupCreate();
  JSContextGroupRetain(nullptr);
  JSContextGroupRelease(nullptr);
  JSGlobalContextCreate(nullptr);
  JSGlobalContextCreateInGroup(nullptr, nullptr);
  JSGlobalContextRetain(nullptr);
  JSGlobalContextRelease(nullptr);
  JSContextGetGroup(nullptr);
  JSContextGetGlobalContext(nullptr);
  JSGlobalContextCopyName(nullptr);
  JSGlobalContextSetName(nullptr, nullptr);
}

}