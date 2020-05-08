#pragma once
#include <Ultralight/private/Font.h>
#include <Ultralight/private/Path.h>

typedef struct FT_FaceRec_*  FT_Face;
typedef unsigned int  FT_UInt;

namespace WebCore {

class FontRenderer {
public:
  static ultralight::RefPtr<ultralight::Path> GetPath(ultralight::RefPtr<ultralight::Font> font, FT_Face face, FT_UInt glyph_index, bool flip_y);

  static bool RenderGlyph(ultralight::RefPtr<ultralight::Font> font, FT_Face face, FT_UInt glyph_index);
};

}  // namespace WebCore
