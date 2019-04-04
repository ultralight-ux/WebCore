#include "config.h"
#include "PlatformUserPreferredLanguages.h"
#include <wtf/Vector.h>
#include <wtf/text/WTFString.h>

namespace WTF {

void setPlatformUserPreferredLanguagesChangedCallback(void (*)()) { }

static String platformLanguage()
{
    return ASCIILiteral("en-US");
}

Vector<String> platformUserPreferredLanguages()
{
    return { platformLanguage() };
}

} // namespace WTF
