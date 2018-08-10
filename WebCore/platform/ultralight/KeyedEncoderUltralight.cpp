#include "config.h"
#include "KeyedEncoderUltralight.h"
#include "NotImplemented.h"
#include "SharedBuffer.h"

namespace WebCore {

KeyedEncoderUltralight::KeyedEncoderUltralight() { notImplemented(); }
KeyedEncoderUltralight::~KeyedEncoderUltralight() {}

PassRefPtr<WebCore::SharedBuffer> KeyedEncoderUltralight::finishEncoding() { return nullptr; }

void KeyedEncoderUltralight::encodeBytes(const String& key, const uint8_t*, size_t) {}
void KeyedEncoderUltralight::encodeBool(const String& key, bool) {}
void KeyedEncoderUltralight::encodeUInt32(const String& key, uint32_t) {}
void KeyedEncoderUltralight::encodeInt32(const String& key, int32_t) {}
void KeyedEncoderUltralight::encodeInt64(const String& key, int64_t) {}
void KeyedEncoderUltralight::encodeFloat(const String& key, float) {}
void KeyedEncoderUltralight::encodeDouble(const String& key, double) {}
void KeyedEncoderUltralight::encodeString(const String& key, const String&) {}

void KeyedEncoderUltralight::beginObject(const String& key) {}
void KeyedEncoderUltralight::endObject() {}

void KeyedEncoderUltralight::beginArray(const String& key) {}
void KeyedEncoderUltralight::beginArrayElement() {}
void KeyedEncoderUltralight::endArrayElement() {}
void KeyedEncoderUltralight::endArray() {}


std::unique_ptr<KeyedEncoder> KeyedEncoder::encoder()
{
  return std::make_unique<KeyedEncoderUltralight>();
}

} // namespace WebCore
