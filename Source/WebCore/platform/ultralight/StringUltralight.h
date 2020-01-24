#pragma once
#include <Ultralight/String16.h>
#include <wtf/text/WTFString.h>
//#include <wtf/unicode/UTF8.h>
#include <wtf/text/ASCIIFastPath.h>

namespace ultralight {

inline WTF::String Convert(const ultralight::String16& str) {
  return WTF::String(reinterpret_cast<const UChar*>(str.data()), str.length());
}

inline ultralight::String16 Convert(const WTF::String& str) {
  if (str.isEmpty())
    return ultralight::String16();

  if (str.is8Bit())
    return ultralight::String16(reinterpret_cast<const char*>(str.characters8()), str.length());

  return ultralight::String16(reinterpret_cast<const Char16*>(str.characters16()), str.length());
}

}  // namespace ultralight
