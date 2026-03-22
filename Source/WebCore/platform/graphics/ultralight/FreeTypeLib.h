#pragma once

#include "ft2build.h"
#include FT_FREETYPE_H
#include FT_MULTIPLE_MASTERS_H

namespace WTF { class Lock; }

namespace WebCore {

FT_Library GetFreeTypeLib();

// Mutex that must be held during FT_Library-level operations (FT_New_Face, FT_Done_Face, etc.).
// Per-face operations (FT_Load_Glyph, FT_Set_Pixel_Sizes, etc.) do NOT need this lock
// as long as each FT_Face is used from only one thread at a time.
WTF::Lock& GetFreeTypeLock();

}  // namespace WebCore
