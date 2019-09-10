#include "config.h"
#include "HarfBuzzFace.h"

#include "Font.h"
#include "FontPlatformData.h"
#include "GlyphBuffer.h"
#include "HarfBuzzShaper.h"
#include "TextEncoding.h"
#include <hb.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_TRUETYPE_TABLES_H
#include <wtf/text/CString.h>
#include <wtf/text/StringView.h>

namespace WebCore {


struct HarfBuzzFontData {
    HarfBuzzFontData(WTF::HashMap<uint32_t, uint16_t>* glyphCacheForFaceCacheEntry, FontPlatformData* platformData)
        : m_glyphCacheForFaceCacheEntry(glyphCacheForFaceCacheEntry)
        , m_platformData(platformData)
    { }
    WTF::HashMap<uint32_t, uint16_t>* m_glyphCacheForFaceCacheEntry;
    FontPlatformData* m_platformData;
};

static hb_position_t floatToHarfBuzzPosition(float value)
{
    return static_cast<hb_position_t>(value * (1 << 16));
}

static hb_position_t doubleToHarfBuzzPosition(double value)
{
    return static_cast<hb_position_t>(value * (1 << 16));
}

static void UltralightGetGlyphWidthAndExtents(FontPlatformData* platformData, hb_codepoint_t codepoint, hb_position_t* advance, hb_glyph_extents_t* extents)
{
  FT_UInt glyph = (FT_UInt)codepoint;

  if (advance) {
    float width = platformData->glyphWidth(glyph);
    *advance = doubleToHarfBuzzPosition(width);
  }

  if (extents) {
    FloatRect rect = platformData->glyphExtents(glyph);
    extents->x_bearing = doubleToHarfBuzzPosition(rect.x());
    extents->y_bearing = doubleToHarfBuzzPosition(rect.y());
    extents->width = doubleToHarfBuzzPosition(rect.width());
    extents->height = doubleToHarfBuzzPosition(rect.height());
  }
   
  /*cairo_text_extents_t glyphExtents;
    cairo_glyph_t glyph;
    glyph.index = codepoint;
    glyph.x = 0;
    glyph.y = 0;
    cairo_scaled_font_glyph_extents(scaledFont, &glyph, 1, &glyphExtents);
    

    bool hasVerticalGlyphs = glyphExtents.y_advance;
    if (advance)
        *advance = doubleToHarfBuzzPosition(hasVerticalGlyphs ? -glyphExtents.y_advance : glyphExtents.x_advance);

    if (extents) {
        extents->x_bearing = doubleToHarfBuzzPosition(glyphExtents.x_bearing);
        extents->y_bearing = doubleToHarfBuzzPosition(hasVerticalGlyphs ? -glyphExtents.y_bearing : glyphExtents.y_bearing);
        extents->width = doubleToHarfBuzzPosition(hasVerticalGlyphs ? -glyphExtents.height : glyphExtents.width);
        extents->height = doubleToHarfBuzzPosition(hasVerticalGlyphs ? glyphExtents.width : glyphExtents.height);
    }
    */
}

static hb_bool_t harfBuzzGetGlyph(hb_font_t*, void* fontData, hb_codepoint_t unicode, hb_codepoint_t, hb_codepoint_t* glyph, void*)
{
    HarfBuzzFontData* hbFontData = reinterpret_cast<HarfBuzzFontData*>(fontData);

    WTF::HashMap<uint32_t, uint16_t>::AddResult result = hbFontData->m_glyphCacheForFaceCacheEntry->add(unicode, 0);
    if (result.isNewEntry) {
      FT_Face face = hbFontData->m_platformData->face();
      FT_UInt index = FT_Get_Char_Index(face, unicode);
      result.iterator->value = index;
    }
    *glyph = result.iterator->value;
    return !!*glyph;
}

static hb_position_t harfBuzzGetGlyphHorizontalAdvance(hb_font_t*, void* fontData, hb_codepoint_t glyph, void*)
{
    HarfBuzzFontData* hbFontData = reinterpret_cast<HarfBuzzFontData*>(fontData);

    hb_position_t advance = 0;
    UltralightGetGlyphWidthAndExtents(hbFontData->m_platformData, glyph, &advance, 0);
    return advance;
}

static hb_bool_t harfBuzzGetGlyphHorizontalOrigin(hb_font_t*, void*, hb_codepoint_t, hb_position_t*, hb_position_t*, void*)
{
    // Just return true, following the way that Harfbuzz-FreeType
    // implementation does.
    return true;
}

static hb_bool_t harfBuzzGetGlyphExtents(hb_font_t*, void* fontData, hb_codepoint_t glyph, hb_glyph_extents_t* extents, void*)
{
    HarfBuzzFontData* hbFontData = reinterpret_cast<HarfBuzzFontData*>(fontData);

    UltralightGetGlyphWidthAndExtents(hbFontData->m_platformData, glyph, 0, extents);
    return true;
}

static hb_font_funcs_t* harfBuzzUltralightGetFontFuncs()
{
    static hb_font_funcs_t* harfBuzzUltralightFontFuncs = 0;

    // We don't set callback functions which we can't support.
    // Harfbuzz will use the fallback implementation if they aren't set.
    if (!harfBuzzUltralightFontFuncs) {
      harfBuzzUltralightFontFuncs = hb_font_funcs_create();
      hb_font_funcs_set_glyph_func(harfBuzzUltralightFontFuncs, harfBuzzGetGlyph, 0, 0);
      hb_font_funcs_set_glyph_h_advance_func(harfBuzzUltralightFontFuncs, harfBuzzGetGlyphHorizontalAdvance, 0, 0);
      hb_font_funcs_set_glyph_h_origin_func(harfBuzzUltralightFontFuncs, harfBuzzGetGlyphHorizontalOrigin, 0, 0);
      hb_font_funcs_set_glyph_extents_func(harfBuzzUltralightFontFuncs, harfBuzzGetGlyphExtents, 0, 0);
      hb_font_funcs_make_immutable(harfBuzzUltralightFontFuncs);
    }
    return harfBuzzUltralightFontFuncs;
}

static hb_blob_t* harfBuzzUltralightGetTable(hb_face_t*, hb_tag_t tag, void* userData)
{
    FontPlatformData* platformData = reinterpret_cast<FontPlatformData*>(userData);
    if (!platformData)
        return 0;

    FT_Face ftFont = platformData->face();
    if (!ftFont)
        return nullptr;

    FT_ULong tableSize = 0;
    FT_Error error = FT_Load_Sfnt_Table(ftFont, tag, 0, 0, &tableSize);
    if (error)
        return 0;

    FT_Byte* buffer = reinterpret_cast<FT_Byte*>(fastMalloc(tableSize));
    if (!buffer)
        return 0;
    FT_ULong expectedTableSize = tableSize;
    error = FT_Load_Sfnt_Table(ftFont, tag, 0, buffer, &tableSize);
    if (error || tableSize != expectedTableSize) {
        fastFree(buffer);
        return 0;
    }

    return hb_blob_create(reinterpret_cast<const char*>(buffer), tableSize, HB_MEMORY_MODE_WRITABLE, buffer, fastFree);
}

static void destroyHarfBuzzFontData(void* userData)
{
    HarfBuzzFontData* hbFontData = reinterpret_cast<HarfBuzzFontData*>(userData);
    delete hbFontData;
}

hb_face_t* HarfBuzzFace::createFace()
{
    hb_face_t* face = hb_face_create_for_tables(harfBuzzUltralightGetTable, m_platformData, 0);
    ASSERT(face);
    return face;
}

hb_font_t* HarfBuzzFace::createFont()
{
    hb_font_t* font = hb_font_create(m_face);
    HarfBuzzFontData* hbFontData = new HarfBuzzFontData(m_glyphCacheForFaceCacheEntry, m_platformData);
    hb_font_set_funcs(font, harfBuzzUltralightGetFontFuncs(), hbFontData, destroyHarfBuzzFontData);
    const float size = m_platformData->size() * m_platformData->font()->font_scale();
    if (floorf(size) == size)
        hb_font_set_ppem(font, size, size);
    int scale = floatToHarfBuzzPosition(size);
    hb_font_set_scale(font, scale, scale);
    hb_font_make_immutable(font);
    return font;
}

GlyphBufferAdvance HarfBuzzShaper::createGlyphBufferAdvance(float width, float height)
{
    return GlyphBufferAdvance(width, height);
}


  //////////////////////////////////////////////

/*
hb_face_t* HarfBuzzFace::createFace()
{
  return hb_ft_face_create(m_platformData->face(), NULL);
}

hb_font_t* HarfBuzzFace::createFont()
{
  return hb_ft_font_create(m_platformData->face(), NULL);
}

GlyphBufferAdvance HarfBuzzShaper::createGlyphBufferAdvance(float width, float height)
{
  return GlyphBufferAdvance(width, height);
}
*/

}  // namespace WebCore
