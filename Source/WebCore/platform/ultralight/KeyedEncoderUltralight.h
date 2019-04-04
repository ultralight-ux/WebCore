#ifndef KeyedEncoderUltralight_H_
#define KeyedEncoderUltralight_H_

#include "KeyedCoding.h"
#include <wtf/RetainPtr.h>
#include <wtf/Vector.h>

namespace WebCore {

class KeyedEncoderUltralight final : public KeyedEncoder {
public:
  KeyedEncoderUltralight();
  ~KeyedEncoderUltralight();

  PassRefPtr<WebCore::SharedBuffer> finishEncoding() override;

private:
  void encodeBytes(const String& key, const uint8_t*, size_t) override;
  void encodeBool(const String& key, bool) override;
  void encodeUInt32(const String& key, uint32_t) override;
  void encodeInt32(const String& key, int32_t) override;
  void encodeInt64(const String& key, int64_t) override;
  void encodeFloat(const String& key, float) override;
  void encodeDouble(const String& key, double) override;
  void encodeString(const String& key, const String&) override;

  void beginObject(const String& key) override;
  void endObject() override;

  void beginArray(const String& key) override;
  void beginArrayElement() override;
  void endArrayElement() override;
  void endArray() override;
};

} // namespace WebCore

#endif
