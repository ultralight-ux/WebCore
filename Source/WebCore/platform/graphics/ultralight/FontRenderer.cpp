#include "FontRenderer.h"
#include <Ultralight/Bitmap.h>
#include <Ultralight/private/Path.h>
#include "FreeTypeLib.h"
#include "ft2build.h"
#include FT_FREETYPE_H
#include FT_OUTLINE_H
#include FT_BITMAP_H

namespace WebCore {

inline double TwentySixDotSix2FloatPixels(FT_Pos val) {
  return static_cast<double>(val) / 64.0;
}

bool RenderBitmapGlyph(ultralight::RefPtr<ultralight::Font> font, FT_Face face, FT_UInt glyph_index) {
  FT_Set_Pixel_Sizes(face, 0, (FT_UInt)font->font_size());

  FT_Error error;
  error = FT_Load_Glyph(face, glyph_index, FT_LOAD_RENDER | FT_LOAD_TARGET_NORMAL);
  if (error) {
    font->StoreGlyph(glyph_index, 0, 0, 0, 0, nullptr);
    return false;
  }

  double advance = TwentySixDotSix2FloatPixels(face->glyph->metrics.horiAdvance) * font->font_scale();
  double width = TwentySixDotSix2FloatPixels(face->glyph->metrics.width) * font->font_scale();
  double height = TwentySixDotSix2FloatPixels(face->glyph->metrics.height) * font->font_scale();
  double bearing = TwentySixDotSix2FloatPixels(face->glyph->metrics.horiBearingY) * font->font_scale();

  FT_GlyphSlot slot = face->glyph;

  if (slot->bitmap.width <= 0) {
    font->StoreGlyph(glyph_index, advance, width, height, bearing, nullptr);
    return false;
  }

  unsigned char bpp = 1;
  uint32_t bitmap_width = slot->bitmap.width;
  uint32_t bitmap_height = slot->bitmap.rows;
  ultralight::RefPtr<ultralight::Bitmap> bitmap;
  if (slot->bitmap.pixel_mode == FT_PIXEL_MODE_MONO) {
    FT_Library lib = GetFreeTypeLib();
    FT_Bitmap converted_bitmap;
    FT_Bitmap_Init(&converted_bitmap);
    error = FT_Bitmap_Convert(lib, &slot->bitmap, &converted_bitmap, 1);
    if (error) {
      FT_Bitmap_Done(lib, &converted_bitmap);
      font->StoreGlyph(glyph_index, advance, width, height, bearing, nullptr);
      return false;
    }

    bitmap = ultralight::Bitmap::Create(bitmap_width, bitmap_height,
      ultralight::kBitmapFormat_A8_UNORM, bitmap_width * bpp, converted_bitmap.buffer,
      bitmap_width * bitmap_height * bpp);
    FT_Bitmap_Done(lib, &converted_bitmap);

    // The bitmap values are currently 0 and 1, we need to scale 1 -> 255
    uint8_t* pixels = (uint8_t*)bitmap->LockPixels();
    for (size_t i = 0; i < bitmap->size(); ++i)
      if (pixels[i] > 0)
        pixels[i] = 255;
    bitmap->UnlockPixels();
  }
  else {
    assert(slot->bitmap.pixel_mode == FT_PIXEL_MODE_GRAY);
    bitmap = ultralight::Bitmap::Create(bitmap_width, bitmap_height,
      ultralight::kBitmapFormat_A8_UNORM, bitmap_width * bpp, slot->bitmap.buffer,
      bitmap_width * bitmap_height * bpp);
  }
  
  font->StoreGlyph(glyph_index, advance, width, height, bearing, bitmap,
    ultralight::Point((float)slot->bitmap_left, (float)slot->bitmap_top * -1.0f));
  return true;
}

struct RenderContext {
  ultralight::RefPtr<ultralight::Path> path;
};

#define MAP(x) ((float)(x) / 64.0f)
#define POINT(val) (ultralight::Point({MAP(val->x), MAP(val->y)}))


int MoveTo(const FT_Vector* p1, void* data) {
  auto path = static_cast<RenderContext*>(data)->path;
  auto pt = POINT(p1);
  path->MoveTo(pt);
  return 0;
}

int LineTo(const FT_Vector* p1, void* data) {
  auto path = static_cast<RenderContext*>(data)->path;
  auto pt = POINT(p1);
  path->LineTo(pt);
  return 0;
}

int ConicTo(const FT_Vector* p1, const FT_Vector* p2, void* data) {
  auto path = static_cast<RenderContext*>(data)->path;
  auto pt1 = POINT(p1);
  auto pt2 = POINT(p2);
  path->ConicTo(pt1, pt2);
  return 0;
}

int CubicTo(const FT_Vector* p1, const FT_Vector* p2, const FT_Vector* p3, void* data) {
  auto path = static_cast<RenderContext*>(data)->path;
  path->CubicTo(POINT(p1), POINT(p2), POINT(p3));
  return 0;
}

static FT_Outline_Funcs g_outline_funcs = { &MoveTo, &LineTo, &ConicTo, &CubicTo, 0, 0 };

bool RenderDistanceFieldGlyph(ultralight::RefPtr<ultralight::Font> font, FT_Face face, FT_UInt glyph_index) {
  FT_Set_Pixel_Sizes(face, 0, (FT_UInt)font->font_size());

  FT_Error error;
  error = FT_Load_Glyph(face, glyph_index, FT_LOAD_NO_HINTING);
  if (error) {
    font->StoreGlyph(glyph_index, 0, 0, 0, 0, nullptr);
    return false;
  }

  double advance = TwentySixDotSix2FloatPixels(face->glyph->metrics.horiAdvance) * font->font_scale();
  double width = TwentySixDotSix2FloatPixels(face->glyph->metrics.width) * font->font_scale();
  double height = TwentySixDotSix2FloatPixels(face->glyph->metrics.height) * font->font_scale();
  double bearing = TwentySixDotSix2FloatPixels(face->glyph->metrics.horiBearingY) * font->font_scale();

  FT_GlyphSlot slot = face->glyph;

  if (slot->format != FT_GLYPH_FORMAT_OUTLINE) {
    font->StoreGlyph(glyph_index, advance, width, height, bearing, nullptr);
    return false;
  }

  RenderContext ctx;
  ctx.path = ultralight::Path::Create();
  ctx.path->matrix().Scale(1.0, -1.0);

  FT_Outline outline = slot->outline;
  error = FT_Outline_Decompose(&outline, &g_outline_funcs, &ctx);
  if (error) {
    font->StoreGlyph(glyph_index, advance, width, height, bearing, nullptr);
    return false;
  }

  if (ctx.path->empty()) {
    font->StoreGlyph(glyph_index, advance, width, height, bearing, nullptr);
    return false;
  }

  ctx.path->Close();

  font->StoreGlyph(glyph_index, advance, width, height, bearing, ctx.path);
  return true;
}

bool FontRenderer::RenderGlyph(ultralight::RefPtr<ultralight::Font> font,
  FT_Face face, FT_UInt glyph_index) {
  if (font->is_distance_field())
    return RenderDistanceFieldGlyph(font, face, glyph_index);
  else
    return RenderBitmapGlyph(font, face, glyph_index);
}

}  // namespace WebCore
