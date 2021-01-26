#include "config.h"
#include "FontPlatformData.h"
#include "NotImplemented.h"
#include "wtf/text/WTFString.h"
#include "../../FontDescription.h"
#include <Ultralight/platform/Platform.h>
#include <Ultralight/platform/Config.h>
#include <Ultralight/private/FontCache.h>
#include "FontRenderer.h"
#include "Glyph.h"
#include "FloatRect.h"
#include "ft2build.h"
#include FT_FREETYPE_H
#include FT_TRUETYPE_TABLES_H
#include <hb-ft.h>
#include <hb-ot.h>

static int TwentySixDotSix2Pixel(const int i)
{
  return (i >> 6) + (32 < (i & 63));
}

namespace WebCore {

#define ENABLE_DISTANCE_FIELD_FONTS 0

FontPlatformData::FontPlatformData(ultralight::RefPtr<ultralight::FontFace> face, const FontDescription& description)
  : m_face(face) {
  
  m_fixedWidth = m_face->face()->face_flags & FT_FACE_FLAG_FIXED_WIDTH;
  auto config = ultralight::Platform::instance().config();

  m_size = description.computedPixelSize();

#if ENABLE_DISTANCE_FIELD_FONTS
  m_distanceField = m_face->face_flags & FT_FACE_FLAG_SCALABLE;
#else
  m_distanceField = false;
#endif

  auto font_cache = ultralight::FontCache::instance();
  m_font = font_cache->GetFont(&face, (uint64_t)hash(), m_size);
  m_font->set_device_scale_hint(description.deviceScale());
}

FontPlatformData::FontPlatformData(const FontPlatformData& other) {
  m_face = nullptr;
  *this = other;
}

FontPlatformData& FontPlatformData::operator=(const FontPlatformData& other)
{
  // Check for self-assignment.
  if (this == &other)
    return *this;

  m_size = other.m_size;
  m_orientation = other.m_orientation;
  m_widthVariant = other.m_widthVariant;
  m_textRenderingMode = other.m_textRenderingMode;
  m_syntheticBold = other.m_syntheticBold;
  m_syntheticOblique = other.m_syntheticOblique;
  m_isColorBitmapFont = other.m_isColorBitmapFont;
  m_isHashTableDeletedValue = other.m_isHashTableDeletedValue;
  m_isSystemFont = other.m_isSystemFont;
  m_fixedWidth = other.m_fixedWidth;
  m_face = other.m_face;
  m_font = other.m_font;

  return *this;
}

FontPlatformData::~FontPlatformData() {
  m_face = nullptr;
  m_font = nullptr;
}

FT_Face FontPlatformData::face() const {
  // We always set the current font-size before accessing the underlying FT_Face
  FT_Face ft_face = m_face->face().get();
  FT_Set_Pixel_Sizes(ft_face, 0, (FT_UInt)m_size);
  return ft_face;
}

bool FontPlatformData::isFixedPitch() const
{
  return m_fixedWidth;
}

bool FontPlatformData::platformIsEqual(const FontPlatformData& platformFont) const {
  return this->m_face == platformFont.m_face && this->m_size == platformFont.m_size && this->m_font == platformFont.m_font;
}

double FontPlatformData::glyphWidth(Glyph glyph) {
  ultralight::RefPtr<ultralight::Font> ultraFont = font();
  return ultraFont->GetGlyphAdvance(glyph);
}

FloatRect FontPlatformData::glyphExtents(Glyph glyph) {
  ultralight::RefPtr<ultralight::Font> ultraFont = font();
  float width = ultraFont->GetGlyphWidth(glyph);
  float height = ultraFont->GetGlyphHeight(glyph);
  return FloatRect(0.0f, 0.0f, width, height);
}

RefPtr<SharedBuffer> FontPlatformData::openTypeTable(uint32_t table) const {
  FT_Face freeTypeFace = face();
  if (!freeTypeFace)
    return nullptr;

  FT_ULong tableSize = 0;
  // Tag bytes need to be reversed because OT_MAKE_TAG uses big-endian order.
  uint32_t tag = FT_MAKE_TAG((table & 0xff), (table & 0xff00) >> 8, (table & 0xff0000) >> 16, table >> 24);
  if (FT_Load_Sfnt_Table(freeTypeFace, tag, 0, 0, &tableSize))
    return nullptr;

  Vector<char> data(tableSize);
  FT_ULong expectedTableSize = tableSize;
  FT_Error error = FT_Load_Sfnt_Table(freeTypeFace, tag, 0, reinterpret_cast<FT_Byte*>(data.data()), &tableSize);
  if (error || tableSize != expectedTableSize)
    return nullptr;

  return SharedBuffer::create(WTFMove(data));
}

#ifndef NDEBUG
String FontPlatformData::description() const
{
  return String("FontPlatformDataUltralight");
}
#endif

unsigned FontPlatformData::hash() const
{
  uintptr_t flags = static_cast<uintptr_t>(static_cast<unsigned>(m_widthVariant) << 6
    | m_isHashTableDeletedValue << 5
    | static_cast<unsigned>(m_textRenderingMode) << 3
    | static_cast<unsigned>(m_orientation) << 2
    | m_syntheticBold << 1
    | m_syntheticOblique);

  uintptr_t font_hash = m_face->font_file()->hash();
  uintptr_t hashCodes[] = { font_hash, flags, (FT_UInt)m_size };
  return StringHasher::hashMemory<sizeof(hashCodes)>(hashCodes);
}

#if USE(HARFBUZZ) && !ENABLE(OPENTYPE_MATH)
HbUniquePtr<hb_font_t> FontPlatformData::createOpenTypeMathHarfBuzzFont() const
{
	FT_Face ftFace = face();
	if (!ftFace)
		return nullptr;

	HbUniquePtr<hb_face_t> face(hb_ft_face_create_cached(ftFace));
	if (!hb_ot_math_has_data(face.get()))
		return nullptr;

	return HbUniquePtr<hb_font_t>(hb_font_create(face.get()));
}
#endif

} // namespace WebCore


