#include "Config.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>

static inline std::string Trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) return "";
    size_t last = str.find_last_not_of(" \t\r\n");
    return str.substr(first, (last - first + 1));
}

void MacroSettings::ResetToDefaults() {
    sequenceString = "2, 2, z, z, 2, 2, x, x, 3, 3";
    triggerKeyVk = VK_XBUTTON1;
    toggleHotkeyVk = VK_F6;
    pressDelayMs = 500;
    keyHoldDurationMs = 35;
    doubleTapDelayMs = 60;
    useFastDoubleTap = false;
    jitterMs = 25;
    loopDelayMs = 0;
    inputMode = 0;
    soundBeepOnToggle = true;
    macroEnabled = true;
    autoDoubleTap = true;
}

void MacroSettings::ValidateAndClamp() {
    if (sequenceString.empty()) {
        sequenceString = "2, 2, z, z, 2, 2, x, x, 3, 3";
    }
    pressDelayMs = (std::clamp)(pressDelayMs, 20, 5000);
    keyHoldDurationMs = (std::clamp)(keyHoldDurationMs, 5, 500);
    doubleTapDelayMs = (std::clamp)(doubleTapDelayMs, 10, 1000);
    jitterMs = (std::clamp)(jitterMs, 0, 200);
    loopDelayMs = (std::clamp)(loopDelayMs, 0, 5000);
    if (inputMode != 0 && inputMode != 1) {
        inputMode = 0;
    }
}

bool MacroSettings::LoadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }

    std::string line;
    while (std::getline(file, line)) {
        line = Trim(line);
        if (line.empty() || line[0] == '#' || line[0] == ';' || line[0] == '[') {
            continue;
        }

        size_t eqPos = line.find('=');
        if (eqPos == std::string::npos) continue;

        std::string key = Trim(line.substr(0, eqPos));
        std::string val = Trim(line.substr(eqPos + 1));

        if (key == "sequence") {
            if (!val.empty()) sequenceString = val;
        } else if (key == "trigger_key_vk") {
            triggerKeyVk = std::stoi(val);
        } else if (key == "toggle_hotkey_vk") {
            toggleHotkeyVk = std::stoi(val);
        } else if (key == "press_delay_ms") {
            pressDelayMs = std::stoi(val);
        } else if (key == "key_hold_ms") {
            keyHoldDurationMs = std::stoi(val);
        } else if (key == "double_tap_delay_ms") {
            doubleTapDelayMs = std::stoi(val);
        } else if (key == "use_fast_double_tap") {
            useFastDoubleTap = (val == "1" || val == "true" || val == "TRUE");
        } else if (key == "jitter_ms") {
            jitterMs = std::stoi(val);
        } else if (key == "loop_delay_ms") {
            loopDelayMs = std::stoi(val);
        } else if (key == "input_mode") {
            inputMode = std::stoi(val);
        } else if (key == "sound_beep") {
            soundBeepOnToggle = (val == "1" || val == "true" || val == "TRUE");
        } else if (key == "macro_enabled") {
            macroEnabled = (val == "1" || val == "true" || val == "TRUE");
        } else if (key == "auto_double_tap") {
            autoDoubleTap = (val == "1" || val == "true" || val == "TRUE");
        }
    }

    ValidateAndClamp();
    return true;
}

bool MacroSettings::SaveToFile(const std::string& filepath) const {
    std::ofstream file(filepath, std::ios::trunc);
    if (!file.is_open()) {
        return false;
    }

    file << "; ================================================================\n";
    file << "; EMIL COMBAT SYSTEM · MACRO CONFIGURATION\n";
    file << "; Auto-generated settings file\n";
    file << "; ================================================================\n\n";

    file << "[Settings]\n";
    file << "sequence = " << sequenceString << "\n";
    file << "trigger_key_vk = " << triggerKeyVk << "\n";
    file << "toggle_hotkey_vk = " << toggleHotkeyVk << "\n";
    file << "press_delay_ms = " << pressDelayMs << "\n";
    file << "key_hold_ms = " << keyHoldDurationMs << "\n";
    file << "double_tap_delay_ms = " << doubleTapDelayMs << "\n";
    file << "use_fast_double_tap = " << (useFastDoubleTap ? "1" : "0") << "\n";
    file << "jitter_ms = " << jitterMs << "\n";
    file << "loop_delay_ms = " << loopDelayMs << "\n";
    file << "input_mode = " << inputMode << "\n";
    file << "sound_beep = " << (soundBeepOnToggle ? "1" : "0") << "\n";
    file << "macro_enabled = " << (macroEnabled ? "1" : "0") << "\n";
    file << "auto_double_tap = " << (autoDoubleTap ? "1" : "0") << "\n";

    return true;
}

static const std::vector<KeyOption> g_triggerOptions = {
    { VK_XBUTTON1, "Mouse Back (XBUTTON1 / Mouse 4)" },
    { VK_XBUTTON2, "Mouse Forward (XBUTTON2 / Mouse 5)" },
    { VK_MBUTTON,  "Middle Mouse Button (MBUTTON)" },
    { VK_RBUTTON,  "Right Mouse Button (RBUTTON)" },
    { VK_LBUTTON,  "Left Mouse Button (LBUTTON)" },
    { 'V',         "Keyboard [V] Key" },
    { 'C',         "Keyboard [C] Key" },
    { 'F',         "Keyboard [F] Key" },
    { 'Q',         "Keyboard [Q] Key" },
    { 'E',         "Keyboard [E] Key" },
    { VK_CAPITAL,  "Caps Lock" },
    { VK_LSHIFT,   "Left Shift" },
    { VK_LCONTROL, "Left Control" }
};

static const std::vector<KeyOption> g_hotkeyOptions = {
    { VK_F6,       "F6 [Default]" },
    { VK_F5,       "F5" },
    { VK_F7,       "F7" },
    { VK_F8,       "F8" },
    { VK_F9,       "F9" },
    { VK_F10,      "F10" },
    { VK_F11,      "F11" },
    { VK_F12,      "F12" },
    { VK_INSERT,   "Insert" },
    { VK_HOME,     "Home" },
    { VK_PRIOR,    "Page Up" },
    { VK_NEXT,     "Page Down" }
};

const std::vector<KeyOption>& GetTriggerKeyOptions() {
    return g_triggerOptions;
}

const std::vector<KeyOption>& GetHotkeyOptions() {
    return g_hotkeyOptions;
}

const char* GetKeyNameByVk(int vk, const std::vector<KeyOption>& options, const char* fallback) {
    for (const auto& opt : options) {
        if (opt.vk == vk) return opt.name;
    }
    return fallback;
}
