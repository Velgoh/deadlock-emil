#pragma once

#include "Config.h"
#include "InputManager.h"
#include <thread>
#include <atomic>
#include <mutex>
#include <vector>
#include <deque>
#include <string>
#include <chrono>

struct LogEntry {
    std::string timestamp;
    std::string message;
    int level; // 0 = Info, 1 = Success/Gold, 2 = Warning/Alert
};

class MacroEngine {
public:
    MacroEngine();
    ~MacroEngine();

    void Initialize(const MacroSettings& initialSettings);
    void Shutdown();

    void SetSettings(const MacroSettings& settings);
    MacroSettings GetSettings() const;

    // Hotkey & Master state
    bool IsMacroEnabled() const;
    void SetMacroEnabled(bool enabled);
    void ToggleMacroEnabled();

    // Telemetry
    bool IsTriggerHeld() const;
    bool IsSequenceRunning() const;
    int GetCurrentStepIndex() const;
    int GetTotalSteps() const;
    uint64_t GetCycleCount() const;
    uint64_t GetKeysPressedCount() const;
    std::vector<MacroKeyStep> GetParsedSteps() const;
    std::string GetParseError() const;

    // Logging
    std::vector<LogEntry> GetRecentLogs(size_t maxCount = 100);
    void ClearLogs();
    void AddLog(const std::string& msg, int level = 0);

    // Emergency release & Test helpers
    void PanicRelease();
    bool InterruptibleSleepForTest(int milliseconds);

private:
    void WorkerLoop();
    void CheckHotkeyEdge();
    bool InterruptibleSleep(int milliseconds, int inputMode);
    int CalculateJitter(int maxJitter);

    mutable std::mutex m_settingsMutex;
    MacroSettings m_settings;

    std::vector<MacroKeyStep> m_parsedSteps;
    std::string m_parseError;

    std::atomic<bool> m_running{ false };
    std::atomic<bool> m_macroEnabled{ true };
    std::atomic<bool> m_triggerHeld{ false };
    std::atomic<bool> m_sequenceRunning{ false };
    std::atomic<bool> m_wasHotkeyHeld{ false };
    std::atomic<int> m_currentStepIndex{ -1 };
    std::atomic<uint64_t> m_cycleCount{ 0 };
    std::atomic<uint64_t> m_keysPressedCount{ 0 };

    std::thread m_workerThread;

    mutable std::mutex m_logMutex;
    std::deque<LogEntry> m_logs;
};
