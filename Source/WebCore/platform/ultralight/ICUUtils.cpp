#include "config.h"
#include "ICUUtils.h"
#include <unicode/putil.h>

void setICUDataDirectory(const char* dir) {
  u_setDataDirectory(dir);
}
