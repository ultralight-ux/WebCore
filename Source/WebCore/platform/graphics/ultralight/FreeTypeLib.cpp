#include "config.h"
#include "FreeTypeLib.h"
#include <wtf/Assertions.h>
#include <wtf/Shutdown.h>

static FT_Library g_freetype = nullptr;

namespace WebCore {

void DestroyFreeType() {
  if (g_freetype) {
    FT_Done_FreeType(g_freetype);
    g_freetype = nullptr;
  }
}

FT_Library GetFreeTypeLib() {
  if (!g_freetype) {
    if(FT_Init_FreeType(&g_freetype) == 0) {
      WTF::CallOnShutdown([]() mutable {
        DestroyFreeType();
      });
    } else {
      g_freetype = nullptr;
      ASSERT_WITH_MESSAGE(g_freetype, "Unable to load FreeType.");
    }
  }

  return g_freetype;
}

}  // namespace WebCore
