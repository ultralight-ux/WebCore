#include "config.h"
#include "PlatformKeyboardEvent.h"
#include "NotImplemented.h"

namespace WebCore {

void PlatformKeyboardEvent::disambiguateKeyDownEvent(Type type, bool backwardCompatibilityMode) {
#if __APPLE__
    // Can only change type from KeyDown to RawKeyDown or Char, as we lack information for other conversions.
    ASSERT(m_type == KeyDown);
    ASSERT(type == RawKeyDown || type == Char);
    m_type = type;
    if (backwardCompatibilityMode)
        return;

    if (type == RawKeyDown) {
        m_text = String();
        m_unmodifiedText = String();
    } else {
        m_keyIdentifier = String();
        m_windowsVirtualKeyCode = 0;
        if (m_text.length() == 1 && (m_text[0U] >= 0xF700 && m_text[0U] <= 0xF7FF)) {
            // According to NSEvents.h, OpenStep reserves the range 0xF700-0xF8FF for function keys. However, some actual private use characters
            // happen to be in this range, e.g. the Apple logo (Option+Shift+K).
            // 0xF7FF is an arbitrary cut-off.
            m_text = String();
            m_unmodifiedText = String();
        }
    }
#endif
}

OptionSet<PlatformEvent::Modifier> PlatformKeyboardEvent::currentStateOfModifierKeys()
{
    // Ultralight TODO
    notImplemented();

    return {}; // FIXME: Implement.
}

} // namespace WebCore
