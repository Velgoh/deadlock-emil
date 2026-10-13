#include "InputManager.h"
#include <algorithm>
#include <cctype>
#include <sstream>
#include <set>
#include <mutex>

struct TrackedKey {
    WORD vk = 0;
    WORD scanCode = 0;
    bool isExtended = false;
    bool isMouse = false;
    DWORD mouseFlagsUp = 0;

    bool operator<(const TrackedKey& o) const {
        if (vk != o.vk) return vk < o.vk;
        if (scanCode != o.scanCode) return scanCode < o.scanCode;
        if (isExtended != o.isExtended) return isExtended < o.isExtended;
        return isMouse < o.isMouse;
    }
};

static std::mutex g_trackedKeysMutex;
static std::set<TrackedKey> g_activeDownKeys;

static inline std::string Trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) return "";
    size_t last = str.find_last_not_of(" \t\r\n");
    return str.substr(first, (last - first + 1));
}

static inline std::string ToUpper(std::string str) {
    for (char& c : str) c = (char)std::toupper((unsigned char)c);
    return str;
}

bool InputManager::ParseKeyToken(const std::string& rawToken, MacroKeyStep& outStep) {
    std::string token = ToUpper(Trim(rawToken));
    if (token.empty()) return false;

    outStep.label = token;
    outStep.isExtended = false;
    outStep.isMouse = false;
    outStep.mouseFlagsDown = 0;
    outStep.mouseFlagsUp = 0;

    // Mouse buttons
    if (token == "LMB" || token == "MOUSE1" || token == "LBUTTON") {
        outStep.vk = VK_LBUTTON;
        outStep.isMouse = true;
        outStep.mouseFlagsDown = MOUSEEVENTF_LEFTDOWN;
        outStep.mouseFlagsUp = MOUSEEVENTF_LEFTUP;
        return true;
    }
    if (token == "RMB" || token == "MOUSE2" || token == "RBUTTON") {
        outStep.vk = VK_RBUTTON;
        outStep.isMouse = true;
        outStep.mouseFlagsDown = MOUSEEVENTF_RIGHTDOWN;
        outStep.mouseFlagsUp = MOUSEEVENTF_RIGHTUP;
        return true;
    }
    if (token == "MMB" || token == "MOUSE3" || token == "MBUTTON") {
        outStep.vk = VK_MBUTTON;
        outStep.isMouse = true;
        outStep.mouseFlagsDown = MOUSEEVENTF_MIDDLEDOWN;
        outStep.mouseFlagsUp = MOUSEEVENTF_MIDDLEUP;
        return true;
    }
    if (token == "M4" || token == "MOUSE4" || token == "XBUTTON1") {
        outStep.vk = VK_XBUTTON1;
        outStep.isMouse = true;
        outStep.mouseFlagsDown = MOUSEEVENTF_XDOWN;
        outStep.mouseFlagsUp = MOUSEEVENTF_XUP;
        return true;
    }
    if (token == "M5" || token == "MOUSE5" || token == "XBUTTON2") {
        outStep.vk = VK_XBUTTON2;
        outStep.isMouse = true;
        outStep.mouseFlagsDown = MOUSEEVENTF_XDOWN;
        outStep.mouseFlagsUp = MOUSEEVENTF_XUP;
        return true;
    }

    // Single character (alphanumeric & printable ASCII symbols)
    if (token.length() == 1) {
        char c = token[0];
        if ((c >= '0' && c <= '9') || (c >= 'A' && c <= 'Z')) {
            outStep.vk = (WORD)c;
            outStep.scanCode = (WORD)MapVirtualKeyA(outStep.vk, MAPVK_VK_TO_VSC);
            return true;
        }
        SHORT vkRes = VkKeyScanA(c);
        if (vkRes != -1) {
            outStep.vk = LOBYTE(vkRes);
            outStep.scanCode = (WORD)MapVirtualKeyA(outStep.vk, MAPVK_VK_TO_VSC);
            return true;
        }
    }

    // Arrow keys
    if (token == "UP") {
        outStep.vk = VK_UP;
        outStep.scanCode = (WORD)MapVirtualKeyA(VK_UP, MAPVK_VK_TO_VSC);
        outStep.isExtended = true;
        return true;
    }
    if (token == "DOWN") {
        outStep.vk = VK_DOWN;
        outStep.scanCode = (WORD)MapVirtualKeyA(VK_DOWN, MAPVK_VK_TO_VSC);
        outStep.isExtended = true;
        return true;
    }
    if (token == "LEFT") {
        outStep.vk = VK_LEFT;
        outStep.scanCode = (WORD)MapVirtualKeyA(VK_LEFT, MAPVK_VK_TO_VSC);
        outStep.isExtended = true;
        return true;
    }
    if (token == "RIGHT") {
        outStep.vk = VK_RIGHT;
        outStep.scanCode = (WORD)MapVirtualKeyA(VK_RIGHT, MAPVK_VK_TO_VSC);
        outStep.isExtended = true;
        return true;
    }

    // Common named keys
    if (token == "SPACE") {
        outStep.vk = VK_SPACE;
        outStep.scanCode = (WORD)MapVirtualKeyA(outStep.vk, MAPVK_VK_TO_VSC);
        return true;
    }
    if (token == "SHIFT" || token == "LSHIFT") {
        outStep.vk = VK_LSHIFT;
        outStep.scanCode = (WORD)MapVirtualKeyA(VK_SHIFT, MAPVK_VK_TO_VSC);
        return true;
    }
    if (token == "RSHIFT") {
        outStep.vk = VK_RSHIFT;
        outStep.scanCode = (WORD)MapVirtualKeyA(VK_RSHIFT, MAPVK_VK_TO_VSC);
        return true;
    }
    if (token == "CTRL" || token == "LCTRL") {
        outStep.vk = VK_LCONTROL;
        outStep.scanCode = (WORD)MapVirtualKeyA(VK_CONTROL, MAPVK_VK_TO_VSC);
        return true;
    }
    if (token == "RCTRL") {
        outStep.vk = VK_RCONTROL;
        outStep.scanCode = (WORD)MapVirtualKeyA(VK_CONTROL, MAPVK_VK_TO_VSC);
        outStep.isExtended = true;
        return true;
    }
    if (token == "ALT" || token == "LALT") {
        outStep.vk = VK_LMENU;
        outStep.scanCode = (WORD)MapVirtualKeyA(VK_MENU, MAPVK_VK_TO_VSC);
        return true;
    }
    if (token == "RALT") {
        outStep.vk = VK_RMENU;
        outStep.scanCode = (WORD)MapVirtualKeyA(VK_MENU, MAPVK_VK_TO_VSC);
        outStep.isExtended = true;
        return true;
    }
    if (token == "TAB") {
        outStep.vk = VK_TAB;
        outStep.scanCode = (WORD)MapVirtualKeyA(outStep.vk, MAPVK_VK_TO_VSC);
        return true;
    }
    if (token == "ENTER" || token == "RETURN") {
        outStep.vk = VK_RETURN;
        outStep.scanCode = (WORD)MapVirtualKeyA(outStep.vk, MAPVK_VK_TO_VSC);
        return true;
    }
    if (token == "ESC" || token == "ESCAPE") {
        outStep.vk = VK_ESCAPE;
        outStep.scanCode = (WORD)MapVirtualKeyA(outStep.vk, MAPVK_VK_TO_VSC);
        return true;
    }
    if (token == "CAPS" || token == "CAPSLOCK") {
        outStep.vk = VK_CAPITAL;
        outStep.scanCode = (WORD)MapVirtualKeyA(outStep.vk, MAPVK_VK_TO_VSC);
        return true;
    }
    if (token == "BACKSPACE" || token == "BKSP") {
        outStep.vk = VK_BACK;
        outStep.scanCode = (WORD)MapVirtualKeyA(outStep.vk, MAPVK_VK_TO_VSC);
        return true;
    }
    if (token == "DELETE" || token == "DEL") {
        outStep.vk = VK_DELETE;
        outStep.scanCode = (WORD)MapVirtualKeyA(outStep.vk, MAPVK_VK_TO_VSC);
        outStep.isExtended = true;
        return true;
    }
    if (token == "INSERT" || token == "INS") {
        outStep.vk = VK_INSERT;
        outStep.scanCode = (WORD)MapVirtualKeyA(outStep.vk, MAPVK_VK_TO_VSC);
        outStep.isExtended = true;
        return true;
    }
    if (token == "HOME") {
        outStep.vk = VK_HOME;
        outStep.scanCode = (WORD)MapVirtualKeyA(outStep.vk, MAPVK_VK_TO_VSC);
        outStep.isExtended = true;
        return true;
    }
    if (token == "END") {
        outStep.vk = VK_END;
        outStep.scanCode = (WORD)MapVirtualKeyA(outStep.vk, MAPVK_VK_TO_VSC);
        outStep.isExtended = true;
        return true;
    }
    if (token == "PGUP" || token == "PAGEUP") {
        outStep.vk = VK_PRIOR;
        outStep.scanCode = (WORD)MapVirtualKeyA(outStep.vk, MAPVK_VK_TO_VSC);
        outStep.isExtended = true;
        return true;
    }
    if (token == "PGDN" || token == "PAGEDOWN") {
        outStep.vk = VK_NEXT;
        outStep.scanCode = (WORD)MapVirtualKeyA(outStep.vk, MAPVK_VK_TO_VSC);
        outStep.isExtended = true;
        return true;
    }

    // Numpad 0-9
    if (token.rfind("NUMPAD", 0) == 0 && token.length() == 7 && std::isdigit((unsigned char)token[6])) {
        int n = token[6] - '0';
        outStep.vk = (WORD)(VK_NUMPAD0 + n);
        outStep.scanCode = (WORD)MapVirtualKeyA(outStep.vk, MAPVK_VK_TO_VSC);
        return true;
    }
    if (token.rfind("NUM", 0) == 0 && token.length() == 4 && std::isdigit((unsigned char)token[3])) {
        int n = token[3] - '0';
        outStep.vk = (WORD)(VK_NUMPAD0 + n);
        outStep.scanCode = (WORD)MapVirtualKeyA(outStep.vk, MAPVK_VK_TO_VSC);
        return true;
    }

    // Function keys F1 - F24
    if (token.length() >= 2 && token[0] == 'F') {
        int fNum = std::atoi(token.substr(1).c_str());
        if (fNum >= 1 && fNum <= 24) {
            outStep.vk = (WORD)(VK_F1 + (fNum - 1));
            outStep.scanCode = (WORD)MapVirtualKeyA(outStep.vk, MAPVK_VK_TO_VSC);
            return true;
        }
    }

    return false;
}

bool InputManager::ParseSequence(const std::string& sequenceStr, std::vector<MacroKeyStep>& outSteps, std::string& outError, bool autoDoubleTap) {
    outSteps.clear();
    outError.clear();

    std::string cleaned = sequenceStr;
    // Replace separators (comma, semicolon, pipe, arrow "->") with spaces
    for (size_t i = 0; i < cleaned.length(); ++i) {
        if (cleaned[i] == ',' || cleaned[i] == ';' || cleaned[i] == '|') {
            cleaned[i] = ' ';
        } else if (cleaned[i] == '-' && i + 1 < cleaned.length() && cleaned[i + 1] == '>') {
            cleaned[i] = ' ';
            cleaned[i + 1] = ' ';
        }
    }

    std::stringstream ss(cleaned);
    std::string token;
    std::vector<MacroKeyStep> rawSteps;

    while (ss >> token) {
        MacroKeyStep singleStep;
        // 1. Direct match as a single token (e.g. "SPACE", "1", "F6", "LMB", etc.)
        if (ParseKeyToken(token, singleStep)) {
            rawSteps.push_back(singleStep);
            continue;
        }

        // 2. Not a direct single token: attempt greedy decomposition for shorthand strings (e.g. "123zc", "112233zzcc")
        size_t pos = 0;
        bool decompOk = true;
        std::vector<MacroKeyStep> tokenSteps;

        while (pos < token.length()) {
            bool matchedSub = false;

            // Try multi-character named keys (e.g. "SPACE", "SHIFT", "NUMPAD1", "F12")
            size_t maxLen = (std::min)((size_t)10, token.length() - pos);
            for (size_t len = maxLen; len >= 2; --len) {
                std::string sub = token.substr(pos, len);
                MacroKeyStep subStep;
                if (ParseKeyToken(sub, subStep)) {
                    tokenSteps.push_back(subStep);
                    pos += len;
                    matchedSub = true;
                    break;
                }
            }

            if (matchedSub) continue;

            // Try single alphanumeric character
            char c = token[pos];
            if ((c >= '0' && c <= '9') || (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z')) {
                std::string singleCharStr(1, c);
                MacroKeyStep subStep;
                if (ParseKeyToken(singleCharStr, subStep)) {
                    tokenSteps.push_back(subStep);
                    pos += 1;
                    matchedSub = true;
                }
            }

            if (!matchedSub) {
                decompOk = false;
                break;
            }
        }

        if (!decompOk || tokenSteps.empty()) {
            outError = "Unrecognized key token: '" + token + "'";
            return false;
        }

        rawSteps.insert(rawSteps.end(), tokenSteps.begin(), tokenSteps.end());
    }

    if (rawSteps.empty()) {
        outError = "Sequence contains no valid key commands";
        return false;
    }

    // Auto Double-Tap expansion
    if (autoDoubleTap) {
        for (size_t i = 0; i < rawSteps.size(); ) {
            bool isAlreadyDoubled = false;
            if (i + 1 < rawSteps.size()) {
                const auto& cur = rawSteps[i];
                const auto& next = rawSteps[i + 1];
                if (cur.vk == next.vk && cur.isMouse == next.isMouse && cur.scanCode == next.scanCode) {
                    isAlreadyDoubled = true;
                }
            }

            if (isAlreadyDoubled) {
                // Already doubled in input sequence -> preserve pair without quadruple-expanding
                outSteps.push_back(rawSteps[i]);
                outSteps.push_back(rawSteps[i + 1]);
                i += 2;
            } else {
                // Shorthand single key -> expand to two sequential presses
                outSteps.push_back(rawSteps[i]);
                outSteps.push_back(rawSteps[i]);
                i += 1;
            }
        }
    } else {
        outSteps = rawSteps;
    }

    return true;
}

void InputManager::TrackKey(const MacroKeyStep& step, bool isDown) {
    std::lock_guard<std::mutex> lock(g_trackedKeysMutex);
    TrackedKey key;
    key.vk = step.vk;
    key.scanCode = step.scanCode;
    key.isExtended = step.isExtended;
    key.isMouse = step.isMouse;
    key.mouseFlagsUp = step.mouseFlagsUp;

    if (isDown) {
        g_activeDownKeys.insert(key);
    } else {
        g_activeDownKeys.erase(key);
    }
}

size_t InputManager::GetActiveTrackedKeysCount() {
    std::lock_guard<std::mutex> lock(g_trackedKeysMutex);
    return g_activeDownKeys.size();
}

void InputManager::SendKeyDown(const MacroKeyStep& step, int inputMode) {
    if (step.isMouse) {
        INPUT input = {};
        input.type = INPUT_MOUSE;
        input.mi.dwFlags = step.mouseFlagsDown;
        if (step.mouseFlagsDown == MOUSEEVENTF_XDOWN) {
            input.mi.mouseData = (step.vk == VK_XBUTTON1) ? XBUTTON1 : XBUTTON2;
        }
        SendInput(1, &input, sizeof(INPUT));
    } else {
        INPUT input = {};
        input.type = INPUT_KEYBOARD;

        if (inputMode == 0 && step.scanCode > 0) { // Hardware ScanCode (Best for Deadlock/Source 2)
            input.ki.wVk = step.vk;
            input.ki.wScan = step.scanCode;
            input.ki.dwFlags = KEYEVENTF_SCANCODE;
        } else { // Virtual Key fallback
            input.ki.wVk = step.vk;
            input.ki.wScan = step.scanCode;
            input.ki.dwFlags = 0;
        }

        if (step.isExtended) {
            input.ki.dwFlags |= KEYEVENTF_EXTENDEDKEY;
        }

        SendInput(1, &input, sizeof(INPUT));
    }
    TrackKey(step, true);
}

void InputManager::SendKeyUp(const MacroKeyStep& step, int inputMode) {
    if (step.isMouse) {
        INPUT input = {};
        input.type = INPUT_MOUSE;
        input.mi.dwFlags = step.mouseFlagsUp;
        if (step.mouseFlagsUp == MOUSEEVENTF_XUP) {
            input.mi.mouseData = (step.vk == VK_XBUTTON1) ? XBUTTON1 : XBUTTON2;
        }
        SendInput(1, &input, sizeof(INPUT));
    } else {
        INPUT input = {};
        input.type = INPUT_KEYBOARD;

        if (inputMode == 0 && step.scanCode > 0) { // Hardware ScanCode
            input.ki.wVk = step.vk;
            input.ki.wScan = step.scanCode;
            input.ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP;
        } else { // Virtual Key fallback
            input.ki.wVk = step.vk;
            input.ki.wScan = step.scanCode;
            input.ki.dwFlags = KEYEVENTF_KEYUP;
        }

        if (step.isExtended) {
            input.ki.dwFlags |= KEYEVENTF_EXTENDEDKEY;
        }

        SendInput(1, &input, sizeof(INPUT));
    }
    TrackKey(step, false);
}

void InputManager::ReleaseAllSimulatedKeys(int inputMode) {
    std::lock_guard<std::mutex> lock(g_trackedKeysMutex);
    for (const auto& key : g_activeDownKeys) {
        if (key.isMouse) {
            INPUT input = {};
            input.type = INPUT_MOUSE;
            input.mi.dwFlags = key.mouseFlagsUp;
            if (key.mouseFlagsUp == MOUSEEVENTF_XUP) {
                input.mi.mouseData = (key.vk == VK_XBUTTON1) ? XBUTTON1 : XBUTTON2;
            }
            SendInput(1, &input, sizeof(INPUT));
        } else {
            INPUT input = {};
            input.type = INPUT_KEYBOARD;
            if (inputMode == 0 && key.scanCode > 0) {
                input.ki.wVk = key.vk;
                input.ki.wScan = key.scanCode;
                input.ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP;
            } else {
                input.ki.wVk = key.vk;
                input.ki.wScan = key.scanCode;
                input.ki.dwFlags = KEYEVENTF_KEYUP;
            }
            if (key.isExtended) {
                input.ki.dwFlags |= KEYEVENTF_EXTENDEDKEY;
            }
            SendInput(1, &input, sizeof(INPUT));
        }
    }
    g_activeDownKeys.clear();
}

bool InputManager::IsVkDown(int vk) {
    return (GetAsyncKeyState(vk) & 0x8000) != 0;
}
