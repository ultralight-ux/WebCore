#include "config.h"
#include "FontCascade.h"
#include "NotImplemented.h"
#include "platform/graphics/BitmapImage.h"
#include "platform/graphics/Ultralight/platformContextUltralight.h"
#include "GlyphBuffer.h"
#include "HarfBuzzShaper.h"
#include <Ultralight/Bitmap.h>
#include <Ultralight/private/Canvas.h>
#include <Ultralight/Platform/Platform.h>
#include <Ultralight/Platform/Config.h>
#include "FontRenderer.h"
#include <unicode/normlzr.h>
#include "ft2build.h"
#include FT_FREETYPE_H

static int TwentySixDotSix2Pixel(const int i)
{
  return (i >> 6) + (32 < (i & 63));
}

namespace WebCore {

bool FontCascade::canReturnFallbackFontsForComplexText()
{
  return false;
}

bool FontCascade::canExpandAroundIdeographsInComplexText()
{
  return false;
}

void FontCascade::adjustSelectionRectForComplexText(const TextRun& run, LayoutRect& selectionRect, unsigned from, unsigned to) const
{
  HarfBuzzShaper shaper(this, run);
  if (shaper.shape()) {
    // FIXME: This should mimic Mac port.
    FloatRect rect = shaper.selectionRect(FloatPoint(selectionRect.location()), selectionRect.height().toInt(), from, to);
    selectionRect = LayoutRect(rect);
    return;
  }
  LOG_ERROR("Shaper couldn't shape text run.");
}

float FontCascade::getGlyphsAndAdvancesForComplexText(const TextRun& run, unsigned from, unsigned to, GlyphBuffer& glyphBuffer, ForTextEmphasisOrNot forTextEmphasis) const
{
  HarfBuzzShaper shaper(this, run);
  if (!shaper.shape(&glyphBuffer)) {
    LOG_ERROR("Shaper couldn't shape glyphBuffer.");
    return 0.0f;
  }

  // FIXME: Mac returns an initial advance here.
  return 0.0f;
}

void FontCascade::drawEmphasisMarksForComplexText(GraphicsContext& /* context */, const TextRun& /* run */, const AtomicString& /* mark */, const FloatPoint& /* point */, unsigned /* from */, unsigned /* to */) const
{
  notImplemented();
}

float FontCascade::floatWidthForComplexText(const TextRun& run, HashSet<const Font*>* fallbackFonts, GlyphOverflow* glyphOverflow) const
{
  HarfBuzzShaper shaper(this, run);
  if (shaper.shape())
    return shaper.totalWidth();
  LOG_ERROR("Shaper couldn't shape text run.");
  return 0.0f;
}

int FontCascade::offsetForPositionForComplexText(const TextRun& run, float x, bool includePartialGlyphs) const
{
  HarfBuzzShaper shaper(this, run);
  if (shaper.shape())
    return shaper.offsetForPosition(x);
  LOG_ERROR("Shaper couldn't shape text run.");
  return 0;
}

void FontCascade::drawGlyphs(GraphicsContext& context, const Font& font, const GlyphBuffer& glyphBuffer,
  unsigned from, unsigned numGlyphs, const FloatPoint& point, FontSmoothingMode smoothing)
{
  WebCore::FontPlatformData& platform_font = const_cast<WebCore::FontPlatformData&>(font.platformData());
  GlyphBufferGlyph* glyphs = const_cast<GlyphBufferGlyph*>(glyphBuffer.glyphs(from));
  FT_Face face = platform_font.face();
  FT_GlyphSlot slot = face->glyph;
  FT_Bool use_kerning = FT_HAS_KERNING(face);
  FT_UInt glyph_index = 0;
  FT_UInt previous = 0;
  float pen_x = point.x();
  float pen_y = point.y();
  FT_Error error;
  ultralight::RefPtr<ultralight::Font> ultraFont = platform_font.font();

  Vector<ultralight::Glyph>& glyphBuf = platform_font.glyphBuffer();
  glyphBuf.reserveCapacity(numGlyphs);

  for (unsigned i = 0; i < numGlyphs; i++) {
    glyph_index = glyphs[i];

    if (use_kerning && previous && glyph_index) {
      FT_Vector delta;
      FT_Get_Kerning(face, previous, glyph_index, FT_KERNING_DEFAULT, &delta);
      int32_t kerning_delta = TwentySixDotSix2Pixel(delta.x);
      pen_x += kerning_delta;
    }

    if (platform_font.isDistanceField())
      glyphBuf.append({ glyph_index, ultralight::Point(pen_x, pen_y) });
    else
      glyphBuf.append({ glyph_index, ultralight::Point((int)pen_x, (int)pen_y) });

    pen_x += font.platformWidthForGlyph(glyph_index);
    previous = glyph_index;
  }

  if (glyphBuf.size()) {
    WebCore::PlatformContextUltralight* platformContext = context.platformContext();
    PlatformCanvas canvas = platformContext->canvas();
    ultralight::Paint paint;
    WebCore::Color color = context.fillColor();
    paint.color = UltralightRGBA(color.red(), color.green(), color.blue(), color.alpha());

    float glyph_scale = font.platformData().size() / ultraFont->font_size();
    canvas->DrawGlyphs(*ultraFont, paint, glyphBuf.data(), glyphBuf.size(), glyph_scale);
    glyphBuf.resize(0);
  }

  //FloatRect rect(point.x(), point.y(), 10.0f * numGlyphs, 5.0f);
  //context.fillRect(rect, Color::black);
  // TODO
  //notImplemented();
}

void Dump_SizeMetrics(const FT_Size_Metrics* metrics)
{
  printf("FT_Size_Metrics (scaled)\n");
  printf("\tx_ppem      = %d\n", metrics->x_ppem);
  printf("\ty_ppem      = %d\n", metrics->y_ppem);
  printf("\tx_scale     = %d\n", metrics->x_scale);
  printf("\ty_scale     = %d\n", metrics->y_scale);
  printf("\tascender    = %d (26.6 frac. pixel)\n", metrics->ascender);
  printf("\tdescender   = %d (26.6 frac. pixel)\n", metrics->descender);
  printf("\theight      = %d (26.6 frac. pixel)\n", metrics->height);
  //	printf("\tmax_advance = %d (26.6 frac. pixel)\n",	metrics->max_advance);
  printf("\n");
}



void Font::platformInit()
{
  // TODO, handle complex fonts. We force the code path to simple here because Harfbuzz isn't
  // returning proper widths/shape for complex fonts (tested on Stripe).
  FontCascade::setCodePath(FontCascade::Simple);

  float fontSize = m_platformData.size();
  FT_Face face = m_platformData.face();
  const FT_Size_Metrics& metrics = face->size->metrics;
  //Dump_SizeMetrics(&metrics);

  float ascent = TwentySixDotSix2Pixel(metrics.ascender);
  float descent = TwentySixDotSix2Pixel(-metrics.descender);
  float capHeight = TwentySixDotSix2Pixel(metrics.height);
  float lineGap = capHeight - ascent - descent;

  m_fontMetrics.setAscent(ascent);
  m_fontMetrics.setDescent(descent);
  m_fontMetrics.setCapHeight(capHeight);
  m_fontMetrics.setLineSpacing(ascent + descent + lineGap);
  m_fontMetrics.setLineGap(lineGap);

  m_fontMetrics.setUnitsPerEm(face->units_per_EM);

  FT_Error error;
  error = FT_Load_Char(face, (FT_ULong)'x', FT_LOAD_DEFAULT);
  assert(error == 0);
  int xHeight = TwentySixDotSix2Pixel(face->glyph->metrics.height);
  m_fontMetrics.setXHeight(xHeight);

  error = FT_Load_Char(face, (FT_ULong)' ', FT_LOAD_DEFAULT);
  assert(error == 0);
  m_spaceWidth = TwentySixDotSix2Pixel(face->glyph->metrics.horiAdvance);

  /*
  printf("From FaceMetrics\n");
  printf("\tascender     = %d\t(from FT_Face)\n", TwentySixDotSix2Pixel(FT_MulFix(face->ascender, face->size->metrics.y_scale)));
  printf("\tdescender    = %d\t(from FT_Face)\n", TwentySixDotSix2Pixel(FT_MulFix(-face->descender, face->size->metrics.y_scale)));
  printf("\theight       = %d\t(from FT_Face)\n", TwentySixDotSix2Pixel(FT_MulFix(face->height, face->size->metrics.y_scale)));
  printf("\tMaxAscender  = %d\t(from bbox.yMax)\n", TwentySixDotSix2Pixel(FT_MulFix(face->bbox.yMax, face->size->metrics.y_scale)));
  printf("\tMinDescender = %d\t(from bbox.yMin)\n", TwentySixDotSix2Pixel(FT_MulFix(-face->bbox.yMin, face->size->metrics.y_scale)));
  printf("\n");
  printf("From SizeMetrics\n");
  printf("\tascender     = %d\t(from FT_Size_Metrics)\n", TwentySixDotSix2Pixel(face->size->metrics.ascender));
  printf("\tdescender    = %d\t(from FT_Size_Metrics)\n", TwentySixDotSix2Pixel(-face->size->metrics.descender));
  printf("\theight       = %d\t(from FT_Size_Metrics)\n", TwentySixDotSix2Pixel(face->size->metrics.height));
  printf("\n");
  */

  // TODO
  //notImplemented();
}

float Font::platformWidthForGlyph(Glyph glyph) const
{
  auto& platformData = const_cast<WebCore::FontPlatformData&>(m_platformData);
  ultralight::RefPtr<ultralight::Font> ultraFont = platformData.font();
  if (!ultraFont->HasGlyph(glyph)) {
    FT_Face face = platformData.face();
    FontRenderer::RenderGlyph(ultraFont, face, glyph);
  }

  float glyph_scale = platformData.size() / ultraFont->font_size();
  return TwentySixDotSix2Pixel(ultraFont->GetGlyphAdvance(glyph) * glyph_scale);
}

FloatRect Font::platformBoundsForGlyph(Glyph glyph) const
{
  auto& platformData = const_cast<WebCore::FontPlatformData&>(m_platformData);
  ultralight::RefPtr<ultralight::Font> ultraFont = platformData.font();
  if (!ultraFont->HasGlyph(glyph)) {
    FT_Face face = platformData.face();
    FontRenderer::RenderGlyph(ultraFont, face, glyph);
  }

  float glyph_scale = platformData.size() / ultraFont->font_size();
  float width = TwentySixDotSix2Pixel(ultraFont->GetGlyphWidth(glyph) * glyph_scale);
  float height = TwentySixDotSix2Pixel(ultraFont->GetGlyphHeight(glyph) * glyph_scale);
  return FloatRect(0.0f, 0.0f, width, height);
}

void Font::platformCharWidthInit()
{
  m_avgCharWidth = 0.f;
  m_maxCharWidth = 0.f;
  initCharWidths();
}

void Font::determinePitch()
{
  m_treatAsFixedPitch = m_platformData.isFixedPitch();
}

RefPtr<Font> Font::platformCreateScaledFont(const FontDescription& fontDescription, float scaleFactor) const
{
  // TODO
  notImplemented();
  return nullptr;
}

#if USE(HARFBUZZ)
bool Font::canRenderCombiningCharacterSequence(const UChar* characters, size_t length) const
{
  if (!m_combiningCharacterSequenceSupport)
    m_combiningCharacterSequenceSupport = std::make_unique<HashMap<String, bool>>();

  WTF::HashMap<String, bool>::AddResult addResult = m_combiningCharacterSequenceSupport->add(String(characters, length), false);
  if (!addResult.isNewEntry)
    return addResult.iterator->value;

  UErrorCode error = U_ZERO_ERROR;
  Vector<UChar, 4> normalizedCharacters(length);
  int32_t normalizedLength = unorm_normalize(characters, length, UNORM_NFC, UNORM_UNICODE_3_2, &normalizedCharacters[0], length, &error);
  // Can't render if we have an error or no composition occurred.
  if (U_FAILURE(error) || (static_cast<size_t>(normalizedLength) == length))
    return false;

  FT_Face face = m_platformData.face();
  if (!face)
    return false;

  if(FT_Get_Char_Index(face, normalizedCharacters[0]))
    addResult.iterator->value = true;

  return addResult.iterator->value;
}
#endif

} // namespace WebCore
