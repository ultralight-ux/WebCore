#pragma once

#if ENABLE(GAMEPAD)

#include "PlatformGamepad.h"

namespace WebCore {

class GamepadUltralight : public PlatformGamepad {
public:
    GamepadUltralight(unsigned index, const String& gamepadID, const String& mapping, unsigned axisCount, unsigned buttonCount);

    const Vector<SharedGamepadValue>& axisValues() const final { return m_axisValues; }
    const Vector<SharedGamepadValue>& buttonValues() const final { return m_buttonValues; }

    void updateDetails(const String& gamepadID, const String& mapping, unsigned axisCount, unsigned buttonCount);
    bool setAxisValue(unsigned index, double value);
    bool setButtonValue(unsigned index, double value);

private:
    Vector<SharedGamepadValue> m_axisValues;
    Vector<SharedGamepadValue> m_buttonValues;
};

}

#endif // ENABLE(GAMEPAD)
