#include "config.h"
#include "GlyphPage.h"
#include "NotImplemented.h"
#include "Font.h"
#include "FontPlatformData.h"
#include "UTF16UChar32Iterator.h"
#include "ft2build.h"
#include FT_FREETYPE_H

namespace WebCore {

  bool GlyphPage::fill(UChar* buffer, unsigned bufferLength)
  {
    const Font& font = this->font();
    FT_Face face = font.platformData().face();

    if (!face)
      return false;

    bool haveGlyphs = false;
    UTF16UChar32Iterator iterator(buffer, bufferLength);
    for (unsigned i = 0; i < GlyphPage::size; i++) {
      UChar32 character = iterator.next();
      if (character == iterator.end())
        break;

      Glyph glyph = FT_Get_Char_Index(face, character);
      if (!glyph)
        setGlyphForIndex(i, 0);
      else {
        setGlyphForIndex(i, glyph);
        haveGlyphs = true;
      }
    }

    return haveGlyphs;
  }

}
