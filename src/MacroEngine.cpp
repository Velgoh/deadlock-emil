#include "MacroEngine.h"
#include <random>
#include <ctime>
#include <iomanip>
#include <sstream>

static std::string GetCurrentTimestampString() {
    auto now = std::chrono::system_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;
    std::time_t tt = std::chrono::system_clock::to_time_t(now);
    std::tm tm;
    localtime_s(&tm, &tt);

    std::ostringstream ss;
    ss << std::setfill('0')
       << std::setw(2) << tm.tm_hour << ":"
       << std::setw(2) << tm.tm_min << ":"
       << std::setw(2) << tm.tm_sec << "."
       << std::setw(3) << ms.count();
    return ss.str();
}

MacroEngine::MacroEngine() {
}

MacroEngine::~MacroEngine() {
    Shutdown();
}

void MacroEngine::Initialize(const MacroSettings& initialSettings) {
    SetSettings(initialSettings);
    m_macroEnabled = initialSettings.macroEnabled;
    m_running = true;

    AddLog("EMIL SYSTEM INITIALIZED · STANDBY", 1);
    AddLog("COMBO SEQUENCE LOADED: " + initialSettings.sequenceString, 0);

    m_workerThread = std::thread(&MacroEngine::WorkerLoop, this);
}

void MacroEngine::Shutdown() {
    if (m_running) {
        m_running = false;
        if (m_workerThread.joinable()) {
            m_workerThread.join();
        }
        PanicRelease();
        AddLog("EMIL SYSTEM TERMINATED · OFFLINE", 2);
    }
}

void MacroEngine::SetSettings(const MacroSettings& settings) {
    std::lock_guard<std::mutex> lock(m_settingsMutex);
    m_settings = settings;
    m_macroEnabled = settings.macroEnabled;

    std::vector<MacroKeyStep> steps;
    std::string err;
    if (InputManager::ParseSequence(settings.sequenceString, steps, err, settings.autoDoubleTap)) {
        m_parsedSteps = steps;
        m_parseError = "";
    } else {
        m_parsedSteps.clear();
        m_parseError = err;
        AddLog("SEQUENCE PARSE ERROR: " + err, 2);
    }
}

MacroSettings MacroEngine::GetSettings() const {
    std::lock_guard<std::mutex> lock(m_settingsMutex);
    return m_settings;
}

bool MacroEngine::IsMacroEnabled() const {
    return m_macroEnabled.load();
}

void MacroEngine::SetMacroEnabled(bool enabled) {
    bool prev = m_macroEnabled.exchange(enabled);
    if (prev != enabled) {
        bool sound = false;
        {
            std::lock_guard<std::mutex> lock(m_settingsMutex);
            m_settings.macroEnabled = enabled;
            sound = m_settings.soundBeepOnToggle;
        }
        if (enabled) {
            AddLog("SYSTEM STATUS: ENGAGED [ACTIVE]", 1);
            if (sound) {
                std::thread([]() {
                    Beep(900, 60);
                    Beep(1300, 60);
                }).detach();
            }
        } else {
            PanicRelease();
            AddLog("SYSTEM STATUS: SUSPENDED [OFF]", 2);
            if (sound) {
                std::thread([]() {
                    Beep(1100, 60);
                    Beep(600, 80);
                }).detach();
            }
        }
    }
}

void MacroEngine::ToggleMacroEnabled() {
    SetMacroEnabled(!m_macroEnabled.load());
}

bool MacroEngine::IsTriggerHeld() const {
    return m_triggerHeld.load();
}

bool MacroEngine::IsSequenceRunning() const {
    return m_sequenceRunning.load();
}

int MacroEngine::GetCurrentStepIndex() const {
    return m_currentStepIndex.load();
}

int MacroEngine::GetTotalSteps() const {
    std::lock_guard<std::mutex> lock(m_settingsMutex);
    return (int)m_parsedSteps.size();
}

uint64_t MacroEngine::GetCycleCount() const {
    return m_cycleCount.load();
}

uint64_t MacroEngine::GetKeysPressedCount() const {
    return m_keysPressedCount.load();
}

std::vector<MacroKeyStep> MacroEngine::GetParsedSteps() const {
    std::lock_guard<std::mutex> lock(m_settingsMutex);
    return m_parsedSteps;
}

std::string MacroEngine::GetParseError() const {
    std::lock_guard<std::mutex> lock(m_settingsMutex);
    return m_parseError;
}

std::vector<LogEntry> MacroEngine::GetRecentLogs(size_t maxCount) {
    std::lock_guard<std::mutex> lock(m_logMutex);
    std::vector<LogEntry> result;
    size_t count = (std::min)(maxCount, m_logs.size());
    size_t start = m_logs.size() - count;
    for (size_t i = start; i < m_logs.size(); ++i) {
        result.push_back(m_logs[i]);
    }
    return result;
}

void MacroEngine::ClearLogs() {
    std::lock_guard<std::mutex> lock(m_logMutex);
    m_logs.clear();
}

void MacroEngine::AddLog(const std::string& msg, int level) {
    std::lock_guard<std::mutex> lock(m_logMutex);
    LogEntry entry;
    entry.timestamp = GetCurrentTimestampString();
    entry.message = msg;
    entry.level = level;
    m_logs.push_back(entry);
    if (m_logs.size() > 500) {
        m_logs.pop_front();
    }
}

void MacroEngine::PanicRelease() {
    int inputMode = 0;
    {
        std::lock_guard<std::mutex> lock(m_settingsMutex);
        inputMode = m_settings.inputMode;
    }
    InputManager::ReleaseAllSimulatedKeys(inputMode);
    m_sequenceRunning = false;
    m_currentStepIndex = -1;
}

int MacroEngine::CalculateJitter(int maxJitter) {
    if (maxJitter <= 0) return 0;
    static thread_local std::mt19937 rng([]() {
        try {
            return std::random_device{}();
        } catch (...) {
            return (unsigned int)GetTickCount64();
        }
    }());
    std::uniform_int_distribution<int> dist(-maxJitter, maxJitter);
    return dist(rng);
}

void MacroEngine::CheckHotkeyEdge() {
    int hotkeyVk = 0;
    {
        std::lock_guard<std::mutex> lock(m_settingsMutex);
        hotkeyVk = m_settings.toggleHotkeyVk;
    }
    bool hotkeyHeldNow = InputManager::IsVkDown(hotkeyVk);
    bool wasHeld = m_wasHotkeyHeld.exchange(hotkeyHeldNow);
    if (hotkeyHeldNow && !wasHeld) {
        ToggleMacroEnabled();
    }
}

bool MacroEngine::InterruptibleSleep(int milliseconds, int inputMode) {
    if (milliseconds <= 0) return true;
    auto start = std::chrono::steady_clock::now();
    while (true) {
        // Continuous hotkey polling during sleep slice
        CheckHotkeyEdge();

        if (!m_running || !m_macroEnabled) {
            PanicRelease();
            return false;
        }

        int triggerVk = 0;
        {
            std::lock_guard<std::mutex> lock(m_settingsMutex);
            triggerVk = m_settings.triggerKeyVk;
        }

        if (!InputManager::IsVkDown(triggerVk)) {
            m_triggerHeld = false;
            PanicRelease();
            AddLog("TRIGGER RELEASED · IMMEDIATE MID-SEQUENCE ABORT", 0);
            return false;
        }

        auto now = std::chrono::steady_clock::now();
        int elapsed = (int)std::chrono::duration_cast<std::chrono::milliseconds>(now - start).count();
        if (elapsed >= milliseconds) {
            return true;
        }

        int remaining = milliseconds - elapsed;
        int sleepChunk = (std::min)(5, remaining);
        std::this_thread::sleep_for(std::chrono::milliseconds(sleepChunk));
    }
}

bool MacroEngine::InterruptibleSleepForTest(int milliseconds) {
    return InterruptibleSleep(milliseconds, 0);
}

void MacroEngine::WorkerLoop() {
    bool wasTriggerHeldBefore = false;

    while (m_running) {
        CheckHotkeyEdge();

        int triggerVk = 0;
        int pressDelay = 500;
        int keyHold = 35;
        int doubleTapDelay = 60;
        bool fastDoubleTap = false;
        int jitter = 25;
        int loopDelay = 0;
        int inputMode = 0;
        std::vector<MacroKeyStep> steps;

        {
            std::lock_guard<std::mutex> lock(m_settingsMutex);
            triggerVk = m_settings.triggerKeyVk;
            pressDelay = m_settings.pressDelayMs;
            keyHold = m_settings.keyHoldDurationMs;
            doubleTapDelay = m_settings.doubleTapDelayMs;
            fastDoubleTap = m_settings.useFastDoubleTap;
            jitter = m_settings.jitterMs;
            loopDelay = m_settings.loopDelayMs;
            inputMode = m_settings.inputMode;
            steps = m_parsedSteps;
        }

        // Check trigger state
        bool triggerHeldNow = InputManager::IsVkDown(triggerVk);
        m_triggerHeld = triggerHeldNow;

        // Log trigger state transitions
        if (triggerHeldNow && !wasTriggerHeldBefore) {
            if (m_macroEnabled) {
                AddLog("TRIGGER ENGAGED · EXECUTING COMBAT SEQUENCE", 1);
            } else {
                AddLog("TRIGGER DETECTED · MACRO IS SUSPENDED [IGNORED]", 0);
            }
        }
        wasTriggerHeldBefore = triggerHeldNow;

        if (!m_macroEnabled || !triggerHeldNow || steps.empty()) {
            if (m_sequenceRunning) {
                PanicRelease();
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            continue;
        }

        // Active execution loop while trigger is held
        m_sequenceRunning = true;
        bool sequenceAborted = false;

        for (size_t i = 0; i < steps.size(); ++i) {
            if (!m_running || !m_macroEnabled || !InputManager::IsVkDown(triggerVk)) {
                sequenceAborted = true;
                break;
            }

            m_currentStepIndex = (int)i;
            const auto& step = steps[i];

            // 1. Press Key Down
            InputManager::SendKeyDown(step, inputMode);

            // 2. Hold Key
            int holdMs = keyHold + CalculateJitter(jitter);
            if (holdMs < 10) holdMs = 10;
            if (!InterruptibleSleep(holdMs, inputMode)) {
                sequenceAborted = true;
                break;
            }

            // 3. Release Key
            InputManager::SendKeyUp(step, inputMode);
            m_keysPressedCount++;

            // 4. Delay After Key Press
            int stepDelay = pressDelay;
            if (fastDoubleTap && (i + 1 < steps.size()) && (steps[i].vk == steps[i + 1].vk)) {
                stepDelay = doubleTapDelay;
            }

            int postMs = stepDelay + CalculateJitter(jitter);
            if (postMs < 10) postMs = 10;

            if (!InterruptibleSleep(postMs, inputMode)) {
                sequenceAborted = true;
                break;
            }
        }

        if (sequenceAborted) {
            PanicRelease();
        } else {
            m_cycleCount++;
            m_currentStepIndex = -1;

            // Optional extra loop delay between cycles if configured
            if (loopDelay > 0 && InputManager::IsVkDown(triggerVk)) {
                int cycleSleepMs = loopDelay + CalculateJitter(jitter);
                if (cycleSleepMs > 0) {
                    if (!InterruptibleSleep(cycleSleepMs, inputMode)) {
                        PanicRelease();
                    }
                }
            }
        }
    }

    PanicRelease();
}
