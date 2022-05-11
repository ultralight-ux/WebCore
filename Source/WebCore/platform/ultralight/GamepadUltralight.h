#pragma once

#if ENABLE(GAMEPAD)

#include "PlatformGamepad.h"

namespace WebCore {

class GamepadUltralight : public PlatformGamepad {
public:
    GamepadUltralight(unsigned index, const String& gamepadID, unsigned axisCount, unsigned buttonCount);

    const Vector<double>& axisValues() const final { return m_axisValues; }
    const Vector<double>& buttonValues() const final { return m_buttonValues; }

    void updateDetails(const String& gamepadID, unsigned axisCount, unsigned buttonCount);
    bool setAxisValue(unsigned index, double value);
    bool setButtonValue(unsigned index, double value);

private:
    Vector<double> m_axisValues;
    Vector<double> m_buttonValues;
};

}

#endif // ENABLE(GAMEPAD)
