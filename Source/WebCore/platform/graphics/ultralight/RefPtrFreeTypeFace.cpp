#include "config.h"
#include "RefPtrFreeTypeFace.h"

#if USE(ULTRALIGHT)

#include "FreeTypeLib.h"

namespace WTF {

template<> void refIfNotNull(FT_Face face)
{
    if (LIKELY(face))
        FT_Reference_Face(face);
}

template<> void derefIfNotNull(FT_Face face)
{
    if (LIKELY(face))
        FT_Done_Face(face);
}

} // namespace WTF

#endif // USE(ULTRALIGHT)
