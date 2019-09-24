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
  JSObjectGetArrayBufferByteLength(nullptr, nullptr, nullptr);
  JSObjectGetArrayBufferBytesPtr(nullptr, nullptr, nullptr);
  JSObjectGetTypedArrayBuffer(nullptr, nullptr, nullptr);
  JSObjectGetTypedArrayByteLength(nullptr, nullptr, nullptr);
  JSObjectGetTypedArrayByteOffset(nullptr, nullptr, nullptr);
  JSObjectGetTypedArrayBytesPtr(nullptr, nullptr, nullptr);
  JSObjectGetTypedArrayLength(nullptr, nullptr, nullptr);
  JSObjectMakeArrayBufferWithBytesNoCopy(nullptr, nullptr, nullptr, nullptr, nullptr, nullptr);
  JSObjectMakeTypedArray(nullptr, nullptr, nullptr, nullptr);
  JSObjectMakeTypedArrayWithArrayBuffer(nullptr, nullptr, nullptr, nullptr);
  auto _ = JSObjectMakeTypedArrayWithArrayBufferAndOffset(nullptr, kJSTypedArrayTypeNone, nullptr, 0, 0, nullptr);
  auto _ = JSObjectMakeTypedArrayWithBytesNoCopy(nullptr, kJSTypedArrayTypeNone, nullptr, 0, nullptr, nullptr, nullptr);
  auto _ = JSValueGetTypedArrayType(nullptr, nullptr, nullptr);
}

}
