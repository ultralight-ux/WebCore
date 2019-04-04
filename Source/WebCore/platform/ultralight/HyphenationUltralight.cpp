#include "config.h"
#include "Hyphenation.h"
#include <wtf/text/StringView.h>
#include "NotImplemented.h"

namespace WebCore {

bool canHyphenate(const AtomicString& localeIdentifier)
{
  notImplemented();
  return false;
}

size_t lastHyphenLocation(StringView text, size_t beforeIndex, const AtomicString& localeIdentifier)
{
  notImplemented();
  return 0;
}

} // namespace WebCore