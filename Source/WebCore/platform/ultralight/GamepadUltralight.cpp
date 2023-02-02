#include "config.h"
#include "GamepadUltralight.h"

#if ENABLE(GAMEPAD)

namespace WebCore {

GamepadUltralight::GamepadUltralight(unsigned index, const String& gamepadID, const String& mapping, unsigned axisCount, unsigned buttonCount)
    : PlatformGamepad(index)
{
    m_connectTime = m_lastUpdateTime = MonotonicTime::now();
    updateDetails(gamepadID, mapping, axisCount, buttonCount);
}

void GamepadUltralight::updateDetails(const String& gamepadID, const String& mapping, unsigned axisCount, unsigned buttonCount)
{
    m_id = gamepadID;
    m_mapping = mapping;
    m_axisValues.clear();
    for (size_t i = 0; i < axisCount; ++i)
        m_axisValues.append({});
    m_buttonValues.clear();
    for (size_t i = 0; i < buttonCount; ++i)
        m_buttonValues.append({});
    m_lastUpdateTime = MonotonicTime::now();
}

bool GamepadUltralight::setAxisValue(unsigned index, double value)
{
    if (index >= m_axisValues.size()) {
        LOG_ERROR("GamepadUltralight (%u): Attempt to set value on axis %u which doesn't exist", m_index, index);
        return false;
    }

    m_axisValues[index].setValue(value);
    m_lastUpdateTime = MonotonicTime::now();
    return true;
}

bool GamepadUltralight::setButtonValue(unsigned index, double value)
{
    if (index >= m_buttonValues.size()) {
        LOG_ERROR("GamepadUltralight (%u): Attempt to set value on button %u which doesn't exist", m_index, index);
        return false;
    }

    m_buttonValues[index].setValue(value);
    m_lastUpdateTime = MonotonicTime::now();
    return true;
}

} // namespace WebCore

#endif // ENABLE(GAMEPAD)
