#pragma once

#include <string>
#include <vector>
#include <windows.h>

struct MacroKeyStep {
    std::string label;
    WORD vk = 0;
    WORD scanCode = 0;
    bool isExtended = false;
    bool isMouse = false;
    DWORD mouseFlagsDown = 0;
    DWORD mouseFlagsUp = 0;
};

class InputManager {
public:
    static bool ParseKeyToken(const std::string& rawToken, MacroKeyStep& outStep);
    static bool ParseSequence(const std::string& sequenceStr, std::vector<MacroKeyStep>& outSteps, std::string& outError, bool autoDoubleTap = false);

    static void SendKeyDown(const MacroKeyStep& step, int inputMode = 0);
    static void SendKeyUp(const MacroKeyStep& step, int inputMode = 0);
    static void ReleaseAllSimulatedKeys(int inputMode = 0);

    static bool IsVkDown(int vk);
    static size_t GetActiveTrackedKeysCount();

private:
    static void TrackKey(const MacroKeyStep& step, bool isDown);
};
