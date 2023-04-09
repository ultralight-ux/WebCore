#pragma once

#if USE(ULTRALIGHT)

#include <wtf/RefPtr.h>

typedef struct FT_FaceRec_* FT_Face;

namespace WTF {

template <>
struct DefaultRefDerefTraits<FT_FaceRec_> {
    static void refIfNotNull(FT_Face face);
    static void derefIfNotNull(FT_Face face);
};

} // namespace WTF

#endif // USE(ULTRALIGHT)
