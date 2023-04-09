#include "config.h"
#include "JavaScriptCore/JavaScript.h"
#include "JavaScriptCore/JSTypedArray.h"
#include <pal/SessionID.h>

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
  JSObjectGetArrayBufferByteLength(nullptr, nullptr, nullptr);
  JSObjectGetArrayBufferBytesPtr(nullptr, nullptr, nullptr);
  JSObjectGetTypedArrayBuffer(nullptr, nullptr, nullptr);
  JSObjectGetTypedArrayByteLength(nullptr, nullptr, nullptr);
  JSObjectGetTypedArrayByteOffset(nullptr, nullptr, nullptr);
  JSObjectGetTypedArrayBytesPtr(nullptr, nullptr, nullptr);
  JSObjectGetTypedArrayLength(nullptr, nullptr, nullptr);
  JSObjectMakeArrayBufferWithBytesNoCopy(nullptr, nullptr, 0, nullptr, nullptr, nullptr);
  JSObjectMakeTypedArray(nullptr, kJSTypedArrayTypeNone, 0, nullptr);
  JSObjectMakeTypedArrayWithArrayBuffer(nullptr, kJSTypedArrayTypeNone, nullptr, nullptr);
  auto _0 = JSObjectMakeTypedArrayWithArrayBufferAndOffset(nullptr, kJSTypedArrayTypeNone, nullptr, 0, 0, nullptr);
  auto _1 = JSObjectMakeTypedArrayWithBytesNoCopy(nullptr, kJSTypedArrayTypeNone, nullptr, 0, nullptr, nullptr, nullptr);
  auto _2 = JSValueGetTypedArrayType(nullptr, nullptr, nullptr);

  auto _3 = PAL::SessionID::generateEphemeralSessionID();
  auto _4 = PAL::SessionID::generatePersistentSessionID();
  PAL::SessionID::enableGenerationProtection();
}

}
