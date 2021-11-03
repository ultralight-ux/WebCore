#pragma once
#include <Ultralight/String16.h>
#include <wtf/text/WTFString.h>
//#include <wtf/unicode/UTF8.h>
#include <wtf/text/ASCIIFastPath.h>

namespace ultralight {

inline WTF::String Convert(const ultralight::String& str) {
    return WTF::String::fromUTF8(str.utf8().data(), str.utf8().length());
}

inline ultralight::String Convert(const WTF::String& str) {
  if (str.isEmpty())
    return ultralight::String();

  if (str.is8Bit())
    return ultralight::String(reinterpret_cast<const char*>(str.characters8()), str.length());

  return ultralight::String(reinterpret_cast<const Char16*>(str.characters16()), str.length());
}

}  // namespace ultralight
