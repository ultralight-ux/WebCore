#include "config.h"
#if USE(ULTRALIGHT)
#include "PlatformFontFreeType.h"
#include <Ultralight/private/PlatformFont.h>
#include <Ultralight/private/tracy/Tracy.hpp>
#include "FontPlatformData.h"
#include "FreeTypeLib.h"
#include "ft2build.h"
#include FT_FREETYPE_H
#include FT_OUTLINE_H
#include FT_BITMAP_H
#include FT_STROKER_H
#include FT_GLYPH_H

struct RenderContext {
  ultralight::RefPtr<ultralight::Path> path;
};

#define MAP(x) ((float)(x) / 64.0f)
#define POINT(val) (ultralight::Point({MAP(val->x), MAP(val->y)}))

inline int MoveTo(const FT_Vector* p1, void* data) {
  auto path = static_cast<RenderContext*>(data)->path;
  auto pt = POINT(p1);
  path->MoveTo(pt);
  return 0;
}

inline int LineTo(const FT_Vector* p1, void* data) {
  auto path = static_cast<RenderContext*>(data)->path;
  auto pt = POINT(p1);
  path->LineTo(pt);
  return 0;
}

inline int ConicTo(const FT_Vector* p1, const FT_Vector* p2, void* data) {
  auto path = static_cast<RenderContext*>(data)->path;
  auto pt1 = POINT(p1);
  auto pt2 = POINT(p2);
  path->ConicTo(pt1, pt2);
  return 0;
}

inline int CubicTo(const FT_Vector* p1, const FT_Vector* p2, const FT_Vector* p3, void* data) {
  auto path = static_cast<RenderContext*>(data)->path;
  path->CubicTo(POINT(p1), POINT(p2), POINT(p3));
  return 0;
}

static FT_Outline_Funcs g_outline_funcs = { &MoveTo, &LineTo, &ConicTo, &CubicTo, 0, 0 };

namespace ultralight {

class PlatformFontFreeType : public PlatformFont {
public:
  PlatformFontFreeType(ultralight::RefPtr<ultralight::FontFace>* face) : font_face_(*face) {
  }

  virtual ~PlatformFontFreeType() {
  }

  inline FT_Face face() const {
    return font_face_->face().get();
  }

  virtual void SetFontSize(float font_size) override {
    FT_Size_RequestRec req;
    FT_Long pixel_size = (FT_Long)std::round(font_size * 64.0f);
    if (pixel_size < 1)
      pixel_size = 1;

    req.type = FT_SIZE_REQUEST_TYPE_NOMINAL;
    req.width = pixel_size;
    req.height = pixel_size;
    req.horiResolution = 0;
    req.vertResolution = 0;

    FT_Request_Size(face(), &req);
  }

  virtual bool GetGlyphMetrics(uint32_t glyph_index, FontHinting hinting,
    double& advance, double& width, double& height, double& bearing) override {
    ProfiledZone;
    FT_Int32 load_flags = FT_LOAD_DEFAULT;
    if (hinting == ultralight::FontHinting::Smooth)
      load_flags |= FT_LOAD_TARGET_LIGHT;
    else if (hinting == ultralight::FontHinting::Normal)
      load_flags |= FT_LOAD_TARGET_NORMAL;
    else if (hinting == ultralight::FontHinting::Monochrome)
      load_flags |= FT_LOAD_TARGET_MONO;
    else if (hinting == ultralight::FontHinting::None)
      load_flags |= FT_LOAD_NO_HINTING;
    FT_Error error = FT_Load_Glyph(face(), glyph_index, load_flags);
    if (error)
      return false;

    advance = static_cast<double>(face()->glyph->metrics.horiAdvance) / 64.0;
    width = static_cast<double>(face()->glyph->metrics.width) / 64.0;
    height = static_cast<double>(face()->glyph->metrics.height) / 64.0;
    bearing = static_cast<double>(face()->glyph->metrics.horiBearingY) / 64.0;

    return true;
  }

  virtual bool RenderGlyph(uint32_t glyph_index, FontHinting hinting,
    RefPtr<Bitmap>& out_bitmap, Point& out_offset, float& out_lsb_delta,
    float& out_rsb_delta) override {
    ProfiledZone;
    FT_Int32 load_flags = FT_LOAD_RENDER;
    if (hinting == ultralight::FontHinting::Smooth)
      load_flags |= FT_LOAD_TARGET_LIGHT;
    else if (hinting == ultralight::FontHinting::Normal)
      load_flags |= FT_LOAD_TARGET_NORMAL;
    else if (hinting == ultralight::FontHinting::Monochrome)
      load_flags |= FT_LOAD_TARGET_MONO;
    else if (hinting == ultralight::FontHinting::None)
      load_flags |= FT_LOAD_NO_HINTING;

    FT_Error error = FT_Load_Glyph(face(), glyph_index, load_flags);
    if (error)
      return false;

    FT_GlyphSlot slot = face()->glyph;

    // Certain glyphs (like space) have an empty bitmap
    if (slot->bitmap.width <= 0)
      return false;

    unsigned char bpp = 1;
    uint32_t bitmap_width = slot->bitmap.width;
    uint32_t bitmap_height = slot->bitmap.rows;
    ultralight::RefPtr<ultralight::Bitmap> bitmap;
    if (slot->bitmap.pixel_mode == FT_PIXEL_MODE_MONO) {
      FT_Library lib = WebCore::GetFreeTypeLib();
      FT_Bitmap converted_bitmap;
      FT_Bitmap_Init(&converted_bitmap);
      error = FT_Bitmap_Convert(lib, &slot->bitmap, &converted_bitmap, 1);
      if (error) {
        FT_Bitmap_Done(lib, &converted_bitmap);
        return false;
      }

      bitmap = ultralight::Bitmap::Create(bitmap_width, bitmap_height,
        ultralight::BitmapFormat::A8_UNORM, bitmap_width * bpp, converted_bitmap.buffer,
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
        ultralight::BitmapFormat::A8_UNORM, bitmap_width * bpp, slot->bitmap.buffer,
        bitmap_width * bitmap_height * bpp);
    }

    out_bitmap = bitmap;
    out_offset = ultralight::Point((float)slot->bitmap_left, (float)slot->bitmap_top * -1.0f);
    out_lsb_delta = static_cast<float>(slot->lsb_delta) / 64.0f;
    out_rsb_delta = static_cast<float>(slot->rsb_delta) / 64.0f;
    return true;
  }

  virtual bool GetGlyphPath(uint32_t glyph_index, RefPtr<Path>& out_path) override {
    ProfiledZone;
    FT_Error error;
    error = FT_Load_Glyph(face(), glyph_index, FT_LOAD_NO_HINTING);
    if (error)
      return false;

    FT_GlyphSlot slot = face()->glyph;

    if (slot->format != FT_GLYPH_FORMAT_OUTLINE)
      return false;

    RenderContext ctx;
    ctx.path = ultralight::Path::Create();

    bool flip_y = true;
    if (flip_y)
      ctx.path->matrix().Scale(1.0, -1.0);

    FT_Outline outline = slot->outline;
    error = FT_Outline_Decompose(&outline, &g_outline_funcs, &ctx);
    if (error)
      return false;

    if (ctx.path->empty())
      return false;

    ctx.path->Close();

    out_path = ctx.path;
    return true;
  }

  virtual bool RenderStrokedGlyph(uint32_t glyph_index, FontHinting hinting,
      float stroke_width, RefPtr<Bitmap>& out_bitmap, Point& out_offset) override {
    ProfiledZone;

    FT_Library lib = WebCore::GetFreeTypeLib();

    // Load glyph outline (not rendered)
    FT_Int32 load_flags = FT_LOAD_NO_BITMAP;
    if (hinting == ultralight::FontHinting::Smooth)
      load_flags |= FT_LOAD_TARGET_LIGHT;
    else if (hinting == ultralight::FontHinting::Normal)
      load_flags |= FT_LOAD_TARGET_NORMAL;
    else if (hinting == ultralight::FontHinting::Monochrome)
      load_flags |= FT_LOAD_TARGET_MONO;
    else if (hinting == ultralight::FontHinting::None)
      load_flags |= FT_LOAD_NO_HINTING;

    FT_Error error = FT_Load_Glyph(face(), glyph_index, load_flags);
    if (error)
      return false;

    FT_GlyphSlot slot = face()->glyph;
    if (slot->format != FT_GLYPH_FORMAT_OUTLINE)
      return false;

    // Get glyph object
    FT_Glyph glyph;
    error = FT_Get_Glyph(slot, &glyph);
    if (error)
      return false;

    // Create and configure stroker
    FT_Stroker stroker;
    error = FT_Stroker_New(lib, &stroker);
    if (error) {
      FT_Done_Glyph(glyph);
      return false;
    }

    // stroke_width is full width, radius is half
    // Convert to 26.6 fixed point (multiply by 64)
    FT_Fixed radius = static_cast<FT_Fixed>(stroke_width * 0.5f * 64.0f);
    // Miter limit 4.0 in 16.16 fixed point (CSS default, matches Chrome/Safari)
    FT_Fixed miter_limit = 0x40000;

    // Use MITER_FIXED for sharp corners (PostScript/PDF style, matches Chrome)
    // Use BUTT line cap since glyph outlines are closed paths
    FT_Stroker_Set(stroker, radius,
        FT_STROKER_LINECAP_BUTT,
        FT_STROKER_LINEJOIN_MITER_FIXED,
        miter_limit);

    // Use FT_Glyph_Stroke to create a proper hollow stroke ring (both inner and outer borders)
    // FT_Glyph_StrokeBorder only returns one border which fills solid when rasterized
    error = FT_Glyph_Stroke(&glyph, stroker, 1 /*destroy orig*/);
    FT_Stroker_Done(stroker);
    if (error) {
      FT_Done_Glyph(glyph);
      return false;
    }

    // Render to bitmap
    error = FT_Glyph_To_Bitmap(&glyph, FT_RENDER_MODE_NORMAL, nullptr, 1);
    if (error) {
      FT_Done_Glyph(glyph);
      return false;
    }

    FT_BitmapGlyph bitmap_glyph = reinterpret_cast<FT_BitmapGlyph>(glyph);
    FT_Bitmap* bitmap = &bitmap_glyph->bitmap;

    if (bitmap->width == 0 || bitmap->rows == 0) {
      FT_Done_Glyph(glyph);
      return false;
    }

    // Create Ultralight bitmap
    out_bitmap = Bitmap::Create(bitmap->width, bitmap->rows,
        BitmapFormat::A8_UNORM, bitmap->width, bitmap->buffer,
        bitmap->width * bitmap->rows);

    out_offset = Point(static_cast<float>(bitmap_glyph->left),
                       static_cast<float>(bitmap_glyph->top) * -1.0f);

    FT_Done_Glyph(glyph);
    return true;
  }

protected:
  ultralight::RefPtr<ultralight::FontFace> font_face_;
};

class PlatformFontFreeTypeFactory : public PlatformFontFactory {
public:
  PlatformFontFreeTypeFactory() {
  }

  virtual ~PlatformFontFreeTypeFactory() {
    set_platform_font_factory(nullptr);
  }

  virtual PlatformFont* CreatePlatformFont(void* data) override {
    return new PlatformFontFreeType(static_cast<ultralight::RefPtr<ultralight::FontFace>*>(data));
  }

  virtual void DestroyPlatformFont(PlatformFont* platform_font) override {
    delete static_cast<PlatformFontFreeType*>(platform_font);
  }
};

void EnsurePlatformFontFactory() {
  static PlatformFontFreeTypeFactory g_platform_font_freetype_factory;

  if (!platform_font_factory())
    set_platform_font_factory(&g_platform_font_freetype_factory);
}

}  // namespace ultralight

#endif  // USE(ULTRALIGHT)
