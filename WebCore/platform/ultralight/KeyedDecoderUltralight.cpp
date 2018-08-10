#include "config.h"
#include "KeyedCoding.h"
#include <wtf/RetainPtr.h>
#include <wtf/Vector.h>
#include <wtf/text/WTFString.h>
#include "NotImplemented.h"

namespace WebCore {

class KeyedDecoderUltralight final : public KeyedDecoder {
public:
  KeyedDecoderUltralight(const uint8_t* data, size_t) { notImplemented(); }
  ~KeyedDecoderUltralight() override {}

private:
  bool decodeBytes(const String& key, const uint8_t*&, size_t&) override { return false; }
  bool decodeBool(const String& key, bool&) override { return false; }
  bool decodeUInt32(const String& key, uint32_t&) override { return false; }
  bool decodeInt32(const String& key, int32_t&) override { return false; }
  bool decodeInt64(const String& key, int64_t&) override { return false; }
  bool decodeFloat(const String& key, float&) override { return false; }
  bool decodeDouble(const String& key, double&) override { return false; }
  bool decodeString(const String& key, String&) override { return false; }

  bool beginObject(const String& key) override { return false; }
  void endObject() override { }

  bool beginArray(const String& key) override { return false; }
  bool beginArrayElement() override { return false; }
  void endArrayElement() override { }
  void endArray() override { }
};

std::unique_ptr<KeyedDecoder> KeyedDecoder::decoder(const uint8_t* data, size_t size)
{
  return std::make_unique<KeyedDecoderUltralight>(data, size);
}

} // namespace WebCore
