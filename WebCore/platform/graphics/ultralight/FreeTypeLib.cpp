#include "FreeTypeLib.h"
#include <wtf/Assertions.h>

namespace WebCore {

FT_Library GetFreeTypeLib() {
  static FT_Library library;
  if (!library && FT_Init_FreeType(&library)) {
    library = nullptr;
    ASSERT_WITH_MESSAGE(library, "Unable to load FreeType.");
    return library;
  }

  return library;
}

}  // namespace WebCore
