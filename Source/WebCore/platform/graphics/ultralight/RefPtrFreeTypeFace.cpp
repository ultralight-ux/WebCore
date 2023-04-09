#include "config.h"
#include "RefPtrFreeTypeFace.h"

#if USE(ULTRALIGHT)

#include "FreeTypeLib.h"

namespace WTF {

void DefaultRefDerefTraits<FT_FaceRec_>::refIfNotNull(FT_Face face)
{
    if (LIKELY(face))
        FT_Reference_Face(face);
}

void DefaultRefDerefTraits<FT_FaceRec_>::derefIfNotNull(FT_Face face)
{
    if (LIKELY(face))
        FT_Done_Face(face);
}

} // namespace WTF

#endif // USE(ULTRALIGHT)
