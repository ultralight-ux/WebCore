#include "config.h"

#if USE(ULTRALIGHT)

#include "FreeTypeFace.h"
#include <wtf/Lock.h>

namespace WebCore {

FreeTypeFace::FreeTypeFace(FT_Face face, ultralight::RefPtr<ultralight::Buffer> fontData)
    : m_face(face)
    , m_fontData(fontData)
{
}

FreeTypeFace::~FreeTypeFace()
{
    if (m_face) {
        Locker locker { GetFreeTypeLock() };
        FT_Done_Face(m_face);
    }
}

RefPtr<FreeTypeFace> FreeTypeFace::createFromFile(const char* path, FT_Long index)
{
    FT_Library lib = GetFreeTypeLib();
    if (!lib)
        return nullptr;

    FT_Face face = nullptr;
    FT_Error error;
    {
        Locker locker { GetFreeTypeLock() };
        error = FT_New_Face(lib, path, index, &face);
    }

    if (error != 0 || !face)
        return nullptr;

    return adoptRef(*new FreeTypeFace(face));
}

RefPtr<FreeTypeFace> FreeTypeFace::createFromMemory(const FT_Byte* data, FT_Long size, FT_Long index)
{
    FT_Library lib = GetFreeTypeLib();
    if (!lib)
        return nullptr;

    FT_Face face = nullptr;
    FT_Error error;
    {
        Locker locker { GetFreeTypeLock() };
        error = FT_New_Memory_Face(lib, data, size, index, &face);
    }

    if (error != 0 || !face)
        return nullptr;

    return adoptRef(*new FreeTypeFace(face));
}

RefPtr<FreeTypeFace> FreeTypeFace::createFromBuffer(ultralight::RefPtr<ultralight::Buffer> buffer, FT_Long index)
{
    if (!buffer || !buffer->size())
        return nullptr;

    FT_Library lib = GetFreeTypeLib();
    if (!lib)
        return nullptr;

    FT_Face face = nullptr;
    FT_Error error;
    {
        Locker locker { GetFreeTypeLock() };
        error = FT_New_Memory_Face(lib, (const FT_Byte*)buffer->data(), buffer->size(), index, &face);
    }

    if (error != 0 || !face)
        return nullptr;

    // Retain the buffer — FT_New_Memory_Face does not copy the data.
    return adoptRef(*new FreeTypeFace(face, buffer));
}

}  // namespace WebCore

#endif  // USE(ULTRALIGHT)
