#pragma once

#if USE(ULTRALIGHT)

#include "TextFlags.h"
#include <wtf/Forward.h>
#include <wtf/Noncopyable.h>
#include <wtf/RefPtr.h>
#include <Ultralight/Vector.h>
#include <Ultralight/Buffer.h>

typedef struct FT_FaceRec_*  FT_Face;

namespace WebCore {

  class FontDescription;
  class FontPlatformData;
  class SharedBuffer;

  struct FontCustomPlatformData {
    WTF_MAKE_NONCOPYABLE(FontCustomPlatformData);
  public:
    FontCustomPlatformData(FT_Face, ultralight::RefPtr<ultralight::Buffer>);
    ~FontCustomPlatformData();
    FontPlatformData fontPlatformData(const FontDescription&, bool bold, bool italic);
    static bool supportsFormat(const String&);

  private:
    FT_Face m_face;
    ultralight::RefPtr<ultralight::Buffer> m_data;
  };

  std::unique_ptr<FontCustomPlatformData> createFontCustomPlatformData(SharedBuffer&);

} // namespace WebCore

#endif // USE(ULTRALIGHT)
