#include "config.h"
#include "GamepadUltralight.h"

#if ENABLE(GAMEPAD)

namespace WebCore {

GamepadUltralight::GamepadUltralight(unsigned index, const String& gamepadID, unsigned axisCount, unsigned buttonCount)
    : PlatformGamepad(index)
{
    m_connectTime = m_lastUpdateTime = MonotonicTime::now();
    updateDetails(gamepadID, axisCount, buttonCount);
}

void GamepadUltralight::updateDetails(const String& gamepadID, unsigned axisCount, unsigned buttonCount)
{
    m_id = gamepadID;
    m_axisValues = Vector<double>(axisCount, 0.0);
    m_buttonValues = Vector<double>(buttonCount, 0.0);
    m_lastUpdateTime = MonotonicTime::now();
}

bool GamepadUltralight::setAxisValue(unsigned index, double value)
{
    if (index >= m_axisValues.size()) {
        LOG_ERROR("GamepadUltralight (%u): Attempt to set value on axis %u which doesn't exist", m_index, index);
        return false;
    }

    m_axisValues[index] = value;
    m_lastUpdateTime = MonotonicTime::now();
    return true;
}

bool GamepadUltralight::setButtonValue(unsigned index, double value)
{
    if (index >= m_buttonValues.size()) {
        LOG_ERROR("GamepadUltralight (%u): Attempt to set value on button %u which doesn't exist", m_index, index);
        return false;
    }

    m_buttonValues[index] = value;
    m_lastUpdateTime = MonotonicTime::now();
    return true;
}

} // namespace WebCore

#endif // ENABLE(GAMEPAD)
