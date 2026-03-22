#include "config.h"
#include "FreeTypeLib.h"
#include <wtf/Assertions.h>
#include <wtf/Lock.h>
#include <wtf/Shutdown.h>
#include <mutex>

static FT_Library g_freetype = nullptr;
static std::once_flag g_freetype_once;
static WTF::Lock g_freeTypeLock;

namespace WebCore {

void DestroyFreeType() {
  if (g_freetype) {
    FT_Done_FreeType(g_freetype);
    g_freetype = nullptr;
  }
}

FT_Library GetFreeTypeLib() {
  std::call_once(g_freetype_once, []() {
    ProfiledMemoryZone(MemoryTag::Font);
    if (FT_Init_FreeType(&g_freetype) != 0) {
      g_freetype = nullptr;
      ASSERT_WITH_MESSAGE(g_freetype, "Unable to load FreeType.");
    }
  });

  return g_freetype;
}

WTF::Lock& GetFreeTypeLock() {
  return g_freeTypeLock;
}

}  // namespace WebCore
