#pragma once
#include <Ultralight/private/Font.h>

typedef struct FT_FaceRec_*  FT_Face;
typedef unsigned int  FT_UInt;

namespace WebCore {

class FontRenderer {
public:
  static bool RenderGlyph(ultralight::RefPtr<ultralight::Font> font, FT_Face face, FT_UInt glyph_index);
};

}  // namespace WebCore
