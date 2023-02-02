
#include "config.h"
#include "GamepadProviderUltralight.h"

#if ENABLE(GAMEPAD)

#include "GamepadProviderClient.h"
#include "GamepadUltralight.h"
#include <wtf/MainThread.h>
#include <wtf/NeverDestroyed.h>

namespace WebCore {

GamepadProviderUltralight& GamepadProviderUltralight::singleton()
{
    static NeverDestroyed<GamepadProviderUltralight> sharedProvider;
    return sharedProvider;
}

GamepadProviderUltralight::GamepadProviderUltralight() = default;

GamepadProviderUltralight::~GamepadProviderUltralight()
{
}

void GamepadProviderUltralight::startMonitoringGamepads(GamepadProviderClient& client)
{
    ASSERT(!m_clients.contains(&client));
    m_clients.add(&client);
}

void GamepadProviderUltralight::stopMonitoringGamepads(GamepadProviderClient& client)
{
    ASSERT(m_clients.contains(&client));
    m_clients.remove(&client);
}

void GamepadProviderUltralight::setGamepadDetails(unsigned index, const String& gamepadID, const String& mapping, unsigned axisCount, unsigned buttonCount)
{
    if (index >= m_gamepadVector.size())
        m_gamepadVector.resize(index + 1);

    if (m_gamepadVector[index])
        m_gamepadVector[index]->updateDetails(gamepadID, mapping, axisCount, buttonCount);
    else
        m_gamepadVector[index] = std::make_unique<GamepadUltralight>(index, gamepadID, mapping, axisCount, buttonCount);
}

bool GamepadProviderUltralight::connectGamepad(unsigned index)
{
    if (index < m_connectedGamepadVector.size() && m_connectedGamepadVector[index]) {
        LOG_ERROR("GamepadProviderUltralight: Attempt to connect a gamepad that is already connected (%u)", index);
        return false;
    }

    if (index >= m_gamepadVector.size() || !m_gamepadVector[index]) {
        LOG_ERROR("GamepadProviderUltralight: Attempt to connect a gamepad that doesn't have details set(%u)", index);
        return false;
    }

    if (m_connectedGamepadVector.size() <= index)
        m_connectedGamepadVector.reserveCapacity(index + 1);

    while (m_connectedGamepadVector.size() <= index)
        m_connectedGamepadVector.uncheckedAppend(nullptr);

    m_connectedGamepadVector[index] = m_gamepadVector[index].get();

    for (auto& client : m_clients)
        client->platformGamepadConnected(*m_connectedGamepadVector[index], EventMakesGamepadsVisible::Yes);

    return true;
}

bool GamepadProviderUltralight::disconnectGamepad(unsigned index)
{
    if (index >= m_connectedGamepadVector.size() || !m_connectedGamepadVector[index]) {
        LOG_ERROR("GamepadProviderUltralight: Attempt to disconnect a gamepad that is not connected (%u)", index);
        return false;
    }
    if (m_connectedGamepadVector[index] != m_gamepadVector[index].get()) {
        LOG_ERROR("GamepadProviderUltralight: Vectors of gamepads and connected gamepads have gotten out of sync");
        return false;
    }

    m_connectedGamepadVector[index] = nullptr;

    for (auto& client : m_clients)
        client->platformGamepadDisconnected(*m_gamepadVector[index]);

    return true;
}

bool GamepadProviderUltralight::setGamepadAxisValue(unsigned index, unsigned axisIndex, double value)
{
    if (index >= m_gamepadVector.size() || !m_gamepadVector[index]) {
        LOG_ERROR("GamepadProviderUltralight: Attempt to set axis value on a gamepad that doesn't exist (%u)", index);
        return false;
    }

    m_gamepadVector[index]->setAxisValue(axisIndex, value);
    gamepadInputActivity();
    return true;
}

bool GamepadProviderUltralight::setGamepadButtonValue(unsigned index, unsigned buttonIndex, double value)
{
    if (index >= m_gamepadVector.size() || !m_gamepadVector[index]) {
        LOG_ERROR("GamepadProviderUltralight: Attempt to set button value on a gamepad that doesn't exist (%u)", index);
        return false;
    }

    m_gamepadVector[index]->setButtonValue(buttonIndex, value);
    setShouldMakeGamepadsVisibile();
    gamepadInputActivity();
    return true;
}

void GamepadProviderUltralight::gamepadInputActivity()
{
    if (!m_shouldScheduleActivityCallback)
        return;

    m_shouldScheduleActivityCallback = false;
    callOnMainThread([this]() {
        dispatchPlatformGamepadInputActivity();

        m_shouldScheduleActivityCallback = true;
    });
}

} // namespace WebCore

#endif // ENABLE(GAMEPAD)
