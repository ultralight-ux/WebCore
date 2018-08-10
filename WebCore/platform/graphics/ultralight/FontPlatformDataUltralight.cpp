#include "config.h"
#include "FontPlatformData.h"
#include "NotImplemented.h"
#include "wtf/text/WTFString.h"
#include "WebCore/platform/graphics/FontDescription.h"
#include <Ultralight/Platform/Platform.h>
#include <Ultralight/Platform/Config.h>
#include <Ultralight/private/FontCache.h>
#include "ft2build.h"
#include FT_FREETYPE_H
#include FT_TRUETYPE_TABLES_H

namespace WebCore {

#define DISTANCE_FIELD_FONT_SIZE 24

float RoundUpDistanceFieldFontSize(float size) {
  // We round up to next highest multiple of 16
  return std::ceilf((size + 16.0f) / 16.0f) * 16.0f;
}

FontPlatformData::FontPlatformData(FT_Face face, ultralight::RefPtr<ultralight::Buffer> data, const FontDescription& description)
  : m_face(face), m_data(data) {
  m_size = description.computedPixelSize();
  m_fixedWidth = m_face->face_flags & FT_FACE_FLAG_FIXED_WIDTH;
  auto config = ultralight::Platform::instance().config();
  m_distanceField = config.use_distance_field_fonts && 
                    (m_face->face_flags & FT_FACE_FLAG_SCALABLE);

  if (isDistanceField())
    m_internalSize = RoundUpDistanceFieldFontSize(m_size * config.device_scale_hint);
  else
    m_internalSize = m_size;

  //
  // This is an important part of distance field fonts: we want to share the distance field representation
  // of each Font, regardless of font size, to prevent duplication of efforts. We use a separate cache in
  // Ultralight to hold cached glyph images (for both raster / distance field fonts) and query this here.
  // Please note that m_size and m_internalSize may not match when distance field fonts are used.
  //
  auto font_cache = ultralight::FontCache::instance();
  m_font = font_cache->GetFont((uint64_t)hash(), isDistanceField(), m_internalSize);
}

FontPlatformData::FontPlatformData(const FontPlatformData& other) {
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
  FT_Reference_Face(m_face);
  m_font = other.m_font;
  m_data = other.m_data;
  m_harfBuzzFace = other.m_harfBuzzFace;

  return *this;
}

FontPlatformData::~FontPlatformData() {
  FT_Done_Face(m_face);
  m_font = nullptr;
}

FT_Face FontPlatformData::face() const {
  // We always set the current font-size before accessing the underlying FT_Face
  FT_Set_Pixel_Sizes(m_face, 0, (FT_UInt)m_size);
  return m_face;
}

bool FontPlatformData::isFixedPitch() const
{
  return m_fixedWidth;
}

bool FontPlatformData::platformIsEqual(const FontPlatformData& platformFont) const {
  return this->m_face == platformFont.m_face;
}

HarfBuzzFace* FontPlatformData::harfBuzzFace() const
{
  if (!m_harfBuzzFace)
    m_harfBuzzFace = HarfBuzzFace::create(const_cast<FontPlatformData*>(this), hash());

  return m_harfBuzzFace.get();
}

RefPtr<SharedBuffer> FontPlatformData::openTypeTable(uint32_t table) const {
  FT_Face freeTypeFace = m_face;
  if (!freeTypeFace)
    return nullptr;

  FT_ULong tableSize = 0;
  // Tag bytes need to be reversed because OT_MAKE_TAG uses big-endian order.
  uint32_t tag = FT_MAKE_TAG((table & 0xff), (table & 0xff00) >> 8, (table & 0xff0000) >> 16, table >> 24);
  if (FT_Load_Sfnt_Table(freeTypeFace, tag, 0, 0, &tableSize))
    return nullptr;

  RefPtr<SharedBuffer> buffer = SharedBuffer::create(tableSize);
  if (buffer->size() != tableSize)
    return nullptr;

  FT_ULong expectedTableSize = tableSize;
  FT_Error error = FT_Load_Sfnt_Table(freeTypeFace, tag, 0, reinterpret_cast<FT_Byte*>(const_cast<char*>(buffer->data())), &tableSize);
  if (error || tableSize != expectedTableSize)
    return nullptr;

  return buffer;
}

#ifndef NDEBUG
String FontPlatformData::description() const
{
  return String("FontPlatformDataUltralight");
}
#endif

} // namespace WebCore


