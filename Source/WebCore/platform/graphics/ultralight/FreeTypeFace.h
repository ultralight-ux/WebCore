#pragma once

#if USE(ULTRALIGHT)

#include "FreeTypeLib.h"
#include <wtf/ThreadSafeRefCounted.h>
#include <wtf/RefPtr.h>
#include <Ultralight/Buffer.h>

namespace WebCore {

// Thread-safe, ref-counted wrapper around FreeType FT_Face.
//
// Why this exists:
//   FreeType requires that FT_New_Face / FT_Done_Face calls on the same
//   FT_Library are serialized (see FreeType docs on multi-threading). We use
//   a single global FT_Library, so this class locks GetFreeTypeLock() during
//   face creation and destruction. Per-face operations (FT_Load_Glyph,
//   FT_Set_Pixel_Sizes, FT_Get_Kerning, etc.) do NOT need locking as long
//   as each FreeTypeFace instance is used from only one thread at a time.
//
// Buffer ownership:
//   FT_New_Memory_Face does not copy font data — it references the caller's
//   buffer directly. For createFromBuffer(), this class retains a RefPtr to
//   the ultralight::Buffer so the data is guaranteed to outlive the FT_Face.
//
// Per-instance isolation:
//   Each FontPlatformData and PlatformFontFreeType creates its own FreeTypeFace
//   from a shared in-memory buffer (cached in FontDatabase). This eliminates
//   cross-thread FT_Face sharing — different threads never touch the same
//   FT_Face, so per-face operations are inherently safe.
//
// File-path fonts:
//   FontDatabase reads file-path fonts into memory on first load so that
//   per-instance faces always use FT_New_Memory_Face (via createFromBuffer).
//   Re-opening the same font file via FT_New_Face fails on Windows when
//   another FT_Face already holds the file open.
class FreeTypeFace : public WTF::ThreadSafeRefCounted<FreeTypeFace> {
public:
    // Create from a file path on disk.
    static RefPtr<FreeTypeFace> createFromFile(const char* path, FT_Long index = 0);

    // Create from a raw memory buffer (caller must ensure data lives long enough,
    // or use the Buffer overload below).
    static RefPtr<FreeTypeFace> createFromMemory(const FT_Byte* data, FT_Long size, FT_Long index = 0);

    // Create from an ultralight::Buffer (retained to keep data alive).
    static RefPtr<FreeTypeFace> createFromBuffer(ultralight::RefPtr<ultralight::Buffer> buffer, FT_Long index = 0);

    ~FreeTypeFace();

    FT_Face face() const { return m_face; }

private:
    FreeTypeFace(FT_Face, ultralight::RefPtr<ultralight::Buffer> = nullptr);

    FT_Face m_face;
    ultralight::RefPtr<ultralight::Buffer> m_fontData;
};

}  // namespace WebCore

#endif  // USE(ULTRALIGHT)
