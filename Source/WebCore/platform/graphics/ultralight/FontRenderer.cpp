#include "FontRenderer.h"
#include <Ultralight/Bitmap.h>
#include <Ultralight/private/Path.h>
#include "ft2build.h"
#include FT_FREETYPE_H
#include FT_OUTLINE_H

namespace WebCore {

bool RenderBitmapGlyph(ultralight::RefPtr<ultralight::Font> font, FT_Face face, FT_UInt glyph_index) {
  FT_Error error;
  error = FT_Load_Glyph(face, glyph_index, FT_LOAD_RENDER | FT_LOAD_FORCE_AUTOHINT | FT_LOAD_TARGET_NORMAL);
  if (error) {
    font->StoreGlyph(glyph_index, 0, 0, 0, 0, nullptr);
    return false;
  }

  int advance = face->glyph->metrics.horiAdvance;
  int width = face->glyph->metrics.width;
  int height = face->glyph->metrics.height;
  int bearing = face->glyph->metrics.horiBearingY;

  FT_GlyphSlot slot = face->glyph;

  if (slot->bitmap.width <= 0) {
    font->StoreGlyph(glyph_index, advance, width, height, bearing, nullptr);
    return false;
  }

  unsigned char bpp = 1;
  uint32_t bitmap_width = slot->bitmap.width;
  uint32_t bitmap_height = slot->bitmap.rows;
  assert(slot->bitmap.pixel_mode == FT_PIXEL_MODE_GRAY);
  auto bitmap = ultralight::Bitmap::Create(bitmap_width, bitmap_height, 
    ultralight::kBitmapFormat_A8, bitmap_width * bpp, slot->bitmap.buffer,
    bitmap_width * bitmap_height * bpp);
  
  ultralight::Point offset = { (float)slot->bitmap_left, (float)slot->bitmap_top };
  font->StoreGlyph(glyph_index, advance, width, height, bearing, bitmap, offset);
  return true;
}

struct RenderContext {
  ultralight::RefPtr<ultralight::Path> path;
};

#define MAP(x) ((float)(x) / 64.0f)
#define POINT(val) (ultralight::Point({MAP(val->x), MAP(val->y)}))


int MoveTo(const FT_Vector* p1, void* data) {
  auto path = static_cast<RenderContext*>(data)->path;
  //path->Close();
  auto pt = POINT(p1);
  path->MoveTo(pt);
  return 0;
}

int LineTo(const FT_Vector* p1, void* data) {
  auto path = static_cast<RenderContext*>(data)->path;
  auto pt = POINT(p1);
  path->LineTo(pt);
  //path->ConicTo(POINT(p1), POINT(p1));
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
  // We make sure to set the current pixel size before rendering since
  // distance field fonts may have a different internal size.
  FT_Set_Pixel_Sizes(face, 0, (FT_UInt)font->font_size());

  FT_Error error;
  error = FT_Load_Glyph(face, glyph_index, FT_LOAD_NO_HINTING);
  if (error) {
    font->StoreGlyph(glyph_index, 0, 0, 0, 0, nullptr);
    return false;
  }

  int advance = face->glyph->metrics.horiAdvance;
  int width = face->glyph->metrics.width;
  int height = face->glyph->metrics.height;
  int bearing = face->glyph->metrics.horiBearingY;

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
