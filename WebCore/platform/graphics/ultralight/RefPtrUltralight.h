#pragma once

#if USE(ULTRALIGHT)

#include <Ultralight/private/Path.h>
#include <wtf/RefPtr.h>

namespace WTF {

#define DECLARE_REF_DEREF_ULTRALIGHT(X)       \
  template<> void refIfNotNull(X * ptr);  \
  template<> void derefIfNotNull(X * ptr);

DECLARE_REF_DEREF_ULTRALIGHT(ultralight::Path)

#undef DECLARE_REF_DEREF_ULTRALIGHT

}  // namespace WTF

#endif // USE(ULTRALIGHT)
