#pragma once

#include <string>
#include <vector>
#include <windows.h>

struct MacroSettings {
    // Sequence string representation (e.g. "2, 2, z, z, 2, 2, x, x, 3, 3")
    std::string sequenceString = "2, 2, z, z, 2, 2, x, x, 3, 3";

    // Trigger Key (default: VK_XBUTTON1 = Mouse Back / XBUTTON1)
    int triggerKeyVk = VK_XBUTTON1;

    // Global toggle hotkey (default: VK_F6)
    int toggleHotkeyVk = VK_F6;

    // Timings in milliseconds
    int pressDelayMs = 500;         // Half-second delay between key presses (as requested)
    int keyHoldDurationMs = 35;     // Duration a key is held down
    int doubleTapDelayMs = 60;      // Fast delay between identical double-taps when enabled
    bool useFastDoubleTap = false;  // When false, every press uses pressDelayMs (500ms)
    int jitterMs = 25;              // Humanization variance (+/- ms)
    int loopDelayMs = 0;            // Extra delay before restarting sequence (0 = seamless 500ms cycle repetition)

    // Input mode: 0 = Hardware ScanCode (Best for Deadlock/Source 2), 1 = Virtual Key
    int inputMode = 0;

    // Audio & UX
    bool soundBeepOnToggle = true;  // System beep when toggling F6
    bool macroEnabled = true;       // Master enabled state
    bool autoDoubleTap = true;      // Auto Double-Tap toggle in settings and UI (enabled by default)

    // Defaults helper
    void ResetToDefaults();

    // Validation & clamping
    void ValidateAndClamp();

    // Persistence
    bool LoadFromFile(const std::string& filepath);
    bool SaveToFile(const std::string& filepath) const;
};

// Available trigger keys for UI dropdown
struct KeyOption {
    int vk;
    const char* name;
};

const std::vector<KeyOption>& GetTriggerKeyOptions();
const std::vector<KeyOption>& GetHotkeyOptions();
const char* GetKeyNameByVk(int vk, const std::vector<KeyOption>& options, const char* fallback = "Unknown");
