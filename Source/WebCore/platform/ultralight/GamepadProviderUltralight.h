
#pragma once

#if ENABLE(GAMEPAD)

#include "GamepadProvider.h"
#include <wtf/text/WTFString.h>
#include <wtf/CompletionHandler.h>

namespace WebCore {

class GamepadUltralight;

class GamepadProviderUltralight : public GamepadProvider {
    WTF_MAKE_NONCOPYABLE(GamepadProviderUltralight);
    friend class NeverDestroyed<GamepadProviderUltralight>;

public:
    WEBCORE_EXPORT static GamepadProviderUltralight& singleton();

    WEBCORE_EXPORT void startMonitoringGamepads(GamepadProviderClient&) final;
    WEBCORE_EXPORT void stopMonitoringGamepads(GamepadProviderClient&) final;
    const Vector<PlatformGamepad*>& platformGamepads() final { return m_connectedGamepadVector; }
    bool isMockGamepadProvider() const final { return false; }

    void playEffect(unsigned gamepadIndex, const String& gamepadID, GamepadHapticEffectType, const GamepadEffectParameters&, CompletionHandler<void(bool)>&& completionHandler) final
    {
        completionHandler(false);
    }

    void stopEffects(unsigned gamepadIndex, const String& gamepadID, CompletionHandler<void()>&& completionHandler) final
    {
        completionHandler();
    }

    WEBCORE_EXPORT void setGamepadDetails(unsigned index, const String& gamepadID, const String& mapping, unsigned axisCount, unsigned buttonCount);
    WEBCORE_EXPORT bool setGamepadAxisValue(unsigned index, unsigned axisIndex, double value);
    WEBCORE_EXPORT bool setGamepadButtonValue(unsigned index, unsigned buttonIndex, double value);
    WEBCORE_EXPORT bool connectGamepad(unsigned index);
    WEBCORE_EXPORT bool disconnectGamepad(unsigned index);

private:
    GamepadProviderUltralight();
    virtual ~GamepadProviderUltralight();

    void gamepadInputActivity();

    Vector<PlatformGamepad*> m_connectedGamepadVector;
    Vector<std::unique_ptr<GamepadUltralight>> m_gamepadVector;

    bool m_shouldScheduleActivityCallback { true };
};

}

#endif // ENABLE(GAMEPAD)
