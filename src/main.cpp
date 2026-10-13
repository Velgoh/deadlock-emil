#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <d3d11.h>
#include <dxgi.h>
#include <dwmapi.h>
#include <tchar.h>
#include <string>
#include <vector>
#include <iostream>

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"

#include "Config.h"
#include "InputManager.h"
#include "MacroEngine.h"
#include "AtlusTheme.h"

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// DirectX 11 global objects
static ID3D11Device*            g_pd3dDevice = nullptr;
static ID3D11DeviceContext*     g_pd3dDeviceContext = nullptr;
static IDXGISwapChain*          g_pSwapChain = nullptr;
static ID3D11RenderTargetView*  g_mainRenderTargetView = nullptr;
static UINT                     g_ResizeWidth = 0, g_ResizeHeight = 0;

static ImFont*                  g_pMainFont = nullptr;
static ImFont*                  g_pLogFont = nullptr;
static ImFont*                  g_pTitleFont = nullptr;

bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void CreateRenderTarget();
void CleanupRenderTarget();
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

static std::string GetExeDirectory() {
    wchar_t path[MAX_PATH];
    GetModuleFileNameW(nullptr, path, MAX_PATH);
    std::wstring ws(path);
    size_t lastSlash = ws.find_last_of(L"\\/");
    if (lastSlash != std::wstring::npos) {
        ws = ws.substr(0, lastSlash);
    }
    int len = WideCharToMultiByte(CP_UTF8, 0, ws.c_str(), -1, nullptr, 0, nullptr, nullptr);
    if (len <= 0) return ".";
    std::string str(len - 1, 0);
    WideCharToMultiByte(CP_UTF8, 0, ws.c_str(), -1, &str[0], len, nullptr, nullptr);
    return str;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    // Enable DPI awareness
    SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

    // Register Win32 Window Class
    WNDCLASSEXW wc = {
        sizeof(wc),
        CS_CLASSDC,
        WndProc,
        0L, 0L,
        GetModuleHandle(nullptr),
        ::LoadIcon(nullptr, IDI_APPLICATION),
        ::LoadCursor(nullptr, IDC_ARROW),
        nullptr, nullptr,
        L"AtlusDeadlockMacroWindowClass",
        nullptr
    };
    ::RegisterClassExW(&wc);

    // Calculate window position and size (1040 x 800)
    int windowWidth = 1040;
    int windowHeight = 800;
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);
    int posX = (std::max)(50, (screenWidth - windowWidth) / 2);
    int posY = (std::max)(50, (screenHeight - windowHeight) / 2);

    HWND hWnd = ::CreateWindowW(
        wc.lpszClassName,
        L"ATLUS // DEADLOCK MACRO [EMIL PROTOCOL]",
        WS_OVERLAPPEDWINDOW,
        posX, posY, windowWidth, windowHeight,
        nullptr, nullptr, wc.hInstance, nullptr
    );

    // Enable dark titlebar in Windows 10/11
    BOOL darkMode = TRUE;
    DwmSetWindowAttribute(hWnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &darkMode, sizeof(darkMode));

    // Initialize Direct3D 11
    if (!CreateDeviceD3D(hWnd)) {
        CleanupDeviceD3D();
        ::UnregisterClassW(wc.lpszClassName, wc.hInstance);
        return 1;
    }

    ::ShowWindow(hWnd, SW_SHOWDEFAULT);
    ::UpdateWindow(hWnd);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    // Load fonts
    io.Fonts->AddFontDefault();
    // Try loading crisp Windows system fonts for high readability
    g_pMainFont = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\segoeui.ttf", 17.5f);
    if (!g_pMainFont) g_pMainFont = io.Fonts->Fonts[0];

    g_pTitleFont = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\segoeuib.ttf", 22.0f);
    if (!g_pTitleFont) g_pTitleFont = g_pMainFont;

    g_pLogFont = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\consola.ttf", 15.0f);
    if (!g_pLogFont) g_pLogFont = g_pMainFont;

    // Apply Atlus Dark Purple theme
    AtlusTheme::ApplyTheme();

    // Setup Platform/Renderer backends
    ImGui_ImplWin32_Init(hWnd);
    ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);

    // Load configuration & initialize Macro Engine
    std::string configPath = GetExeDirectory() + "\\settings.ini";
    MacroSettings settings;
    if (!settings.LoadFromFile(configPath)) {
        // If settings.ini doesn't exist yet, save default
        settings.SaveToFile(configPath);
    }

    MacroEngine engine;
    engine.Initialize(settings);

    // Working copy of UI settings
    char sequenceBuffer[256];
    strncpy_s(sequenceBuffer, sizeof(sequenceBuffer), settings.sequenceString.c_str(), _TRUNCATE);

    int selectedTriggerIndex = 0;
    const auto& triggerOptions = GetTriggerKeyOptions();
    for (size_t i = 0; i < triggerOptions.size(); ++i) {
        if (triggerOptions[i].vk == settings.triggerKeyVk) {
            selectedTriggerIndex = (int)i;
            break;
        }
    }

    int selectedHotkeyIndex = 0;
    const auto& hotkeyOptions = GetHotkeyOptions();
    for (size_t i = 0; i < hotkeyOptions.size(); ++i) {
        if (hotkeyOptions[i].vk == settings.toggleHotkeyVk) {
            selectedHotkeyIndex = (int)i;
            break;
        }
    }

    bool settingsLiveChanged = false;
    bool needDiskSave = false;
    bool autoScrollLog = true;

    // Main loop
    bool done = false;
    while (!done) {
        MSG msg;
        while (::PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE)) {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
            if (msg.message == WM_QUIT) {
                done = true;
            }
        }
        if (done) break;

        // Handle window resize
        if (g_ResizeWidth != 0 && g_ResizeHeight != 0) {
            CleanupRenderTarget();
            g_pSwapChain->ResizeBuffers(0, g_ResizeWidth, g_ResizeHeight, DXGI_FORMAT_UNKNOWN, 0);
            g_ResizeWidth = g_ResizeHeight = 0;
            CreateRenderTarget();
        }

        // Start Dear ImGui frame
        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        ImGui::PushFont(g_pMainFont);

        // Render main full-window docking container
        ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize(viewport->WorkSize);
        ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove |
                                       ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoBringToFrontOnFocus;

        ImGui::Begin("AtlusMainWindow", nullptr, windowFlags);

        // 1. Header Banner (Atlus Angular Stylized Header)
        ImGui::PushFont(g_pTitleFont);
        AtlusTheme::RenderHeaderBanner(
            "DEADLOCK // EMIL MACRO",
            "ATLUS REVOLUTION  ◆  PERSONA / METAPHOR ARCHITECTURE  ◆  STATUS: ACTIVE"
        );
        ImGui::PopFont();

        // 2. Tactical HUD Status Row
        bool isEnabled = engine.IsMacroEnabled();
        bool isTriggerHeld = engine.IsTriggerHeld();
        bool isRunning = engine.IsSequenceRunning();
        int currentStep = engine.GetCurrentStepIndex();
        uint64_t cycles = engine.GetCycleCount();
        uint64_t keysPressed = engine.GetKeysPressedCount();

        ImGui::Spacing();
        AtlusTheme::RenderSectionDivider("STATUS & TELEMETRY");

        ImGui::BeginGroup();
        {
            // Status 1: Master State
            if (isEnabled) {
                AtlusTheme::RenderStatusBadge("SYSTEM MASTER", "ENGAGED [ON]", true, AtlusTheme::ColNeonMagenta);
            } else {
                AtlusTheme::RenderStatusBadge("SYSTEM MASTER", "SUSPENDED [OFF]", false, AtlusTheme::ColNeonRed);
            }
            ImGui::SameLine();

            // Status 2: Trigger Key
            const char* trigName = GetKeyNameByVk(settings.triggerKeyVk, triggerOptions, "Custom");
            if (isTriggerHeld) {
                AtlusTheme::RenderStatusBadge("TRIGGER INPUT", "HELD [ACTIVE]", true, AtlusTheme::ColNeonCyan);
            } else {
                AtlusTheme::RenderStatusBadge("TRIGGER INPUT", "RELEASED", false, AtlusTheme::ColTextMuted);
            }
            ImGui::SameLine();

            // Status 3: Sequence State
            if (isRunning) {
                char stepStr[64];
                snprintf(stepStr, sizeof(stepStr), "STEP #%d FIRING", currentStep + 1);
                AtlusTheme::RenderStatusBadge("SEQUENCE RUNNER", stepStr, true, AtlusTheme::ColNeonViolet);
            } else {
                AtlusTheme::RenderStatusBadge("SEQUENCE RUNNER", "IDLE / STANDBY", false, AtlusTheme::ColTextMuted);
            }
            ImGui::SameLine();

            // Status 4: Iterations & Key count
            char statsStr[64];
            snprintf(statsStr, sizeof(statsStr), "CYC: %llu | KEYS: %llu", cycles, keysPressed);
            AtlusTheme::RenderStatusBadge("CYCLE STATS", statsStr, cycles > 0, AtlusTheme::ColNeonGreen);
        }
        ImGui::EndGroup();

        ImGui::Spacing();
        ImGui::Spacing();

        // 3. Quick Action Controls Bar
        {
            float availW = ImGui::GetContentRegionAvail().x;
            float btnW = (availW - 16.0f) * 0.5f;

            if (isEnabled) {
                ImGui::PushStyleColor(ImGuiCol_Button, AtlusTheme::ColNeonMagenta);
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, AtlusTheme::ColBorderGlow);
                if (ImGui::Button("[ MASTER ENGAGED : PRESS F6 TO SUSPEND ]", ImVec2(btnW, 44.0f))) {
                    engine.ToggleMacroEnabled();
                }
                ImGui::PopStyleColor(3);
            } else {
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.24f, 0.10f, 0.18f, 1.0f));
                ImGui::PushStyleColor(ImGuiCol_Text, AtlusTheme::ColNeonRed);
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.36f, 0.12f, 0.24f, 1.0f));
                if (ImGui::Button("[ MASTER SUSPENDED : PRESS F6 TO ENGAGE ]", ImVec2(btnW, 44.0f))) {
                    engine.ToggleMacroEnabled();
                }
                ImGui::PopStyleColor(3);
            }

            ImGui::SameLine();
            if (ImGui::Button("[ EMERGENCY ABORT & KEY RELEASE ]", ImVec2(btnW, 44.0f))) {
                engine.PanicRelease();
                engine.AddLog("EMERGENCY ABORT COMMAND DISPATCHED", 2);
            }
        }

        ImGui::Spacing();
        ImGui::Spacing();

        // 4. Tabbed Configuration & Monitoring Interface
        if (ImGui::BeginTabBar("AtlusTabBar", ImGuiTabBarFlags_None)) {

            // =====================================================================
            // TAB 1: SEQUENCE CONFIGURATION
            // =====================================================================
            if (ImGui::BeginTabItem("  [ 1. COMBAT SEQUENCE ]  ")) {
                ImGui::Spacing();
                AtlusTheme::RenderSectionDivider("KEY COMBO SPECIFICATION");

                ImGui::TextColored(AtlusTheme::ColNeonViolet, "Sequence Input (Supports shorthand like 123zc or comma separated):");
                ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - 140.0f);
                if (ImGui::InputText("##SequenceInput", sequenceBuffer, sizeof(sequenceBuffer))) {
                    settings.sequenceString = sequenceBuffer;
                    settingsLiveChanged = true;
                }
                if (ImGui::IsItemDeactivatedAfterEdit()) {
                    needDiskSave = true;
                }
                ImGui::SameLine();
                if (ImGui::Button("Reset Default", ImVec2(130.0f, 0.0f))) {
                    strncpy_s(sequenceBuffer, sizeof(sequenceBuffer), "2, 2, z, z, 2, 2, x, x, 3, 3", _TRUNCATE);
                    settings.sequenceString = sequenceBuffer;
                    settingsLiveChanged = true;
                    needDiskSave = true;
                }

                // Auto Double-Tap Option
                ImGui::Spacing();
                if (ImGui::Checkbox("Auto Double-Tap Shorthand", &settings.autoDoubleTap)) {
                    settingsLiveChanged = true;
                    needDiskSave = true;
                }
                ImGui::SameLine();
                ImGui::TextColored(AtlusTheme::ColTextMuted, "(Expands shorthand like 123zc into 1,1,2,2,3,3,z,z,c,c)");

                // Sequence validation status
                std::string parseErr = engine.GetParseError();
                auto parsedSteps = engine.GetParsedSteps();

                if (!parseErr.empty()) {
                    ImGui::TextColored(AtlusTheme::ColNeonRed, "▲ [PARSE ERROR] %s", parseErr.c_str());
                } else {
                    ImGui::TextColored(AtlusTheme::ColNeonGreen, "◆ [READY] %d steps configured.", (int)parsedSteps.size());
                }

                ImGui::Spacing();
                AtlusTheme::RenderSectionDivider("STEP VISUALIZER");
                ImGui::Spacing();

                // Render dynamic visual key steps
                if (!parsedSteps.empty()) {
                    ImGui::BeginChild("StepsContainer", ImVec2(0.0f, 76.0f), true, ImGuiWindowFlags_HorizontalScrollbar);
                    for (int i = 0; i < (int)parsedSteps.size(); ++i) {
                        bool isCurrent = isRunning && (currentStep == i);
                        AtlusTheme::RenderKeyStep(parsedSteps[i].label.c_str(), i, isCurrent, 68.0f, 52.0f);
                        if (i + 1 < (int)parsedSteps.size()) {
                            ImGui::SameLine();
                        }
                    }
                    ImGui::EndChild();
                }

                ImGui::Spacing();
                AtlusTheme::RenderSectionDivider("QUICK PRESETS");
                if (ImGui::Button("Preset A: Standard Double-Tap (2, 2, z, z, 2, 2, x, x, 3, 3)", ImVec2(520.0f, 32.0f))) {
                    strncpy_s(sequenceBuffer, sizeof(sequenceBuffer), "2, 2, z, z, 2, 2, x, x, 3, 3", _TRUNCATE);
                    settings.sequenceString = sequenceBuffer;
                    settingsLiveChanged = true;
                    needDiskSave = true;
                }
                ImGui::SameLine();
                if (ImGui::Button("Preset B: Shorthand Notation (123zc)", ImVec2(340.0f, 32.0f))) {
                    strncpy_s(sequenceBuffer, sizeof(sequenceBuffer), "123zc", _TRUNCATE);
                    settings.sequenceString = sequenceBuffer;
                    settingsLiveChanged = true;
                    needDiskSave = true;
                }

                ImGui::EndTabItem();
            }

            // =====================================================================
            // TAB 2: TIMING & HUMANIZATION
            // =====================================================================
            if (ImGui::BeginTabItem("  [ 2. TIMINGS & DYNAMICS ]  ")) {
                ImGui::Spacing();
                AtlusTheme::RenderSectionDivider("EXECUTION TIMINGS & DELAYS");

                // Press Delay Slider
                ImGui::TextColored(AtlusTheme::ColNeonViolet, "Inter-Press Delay (ms):");
                ImGui::SameLine();
                ImGui::TextDisabled("(Default: 500 ms)");
                ImGui::SetNextItemWidth(550.0f);
                if (ImGui::SliderInt("##PressDelay", &settings.pressDelayMs, 30, 2000, "%d ms")) {
                    settingsLiveChanged = true;
                }
                if (ImGui::IsItemDeactivatedAfterEdit()) needDiskSave = true;

                // Key Hold Duration Slider
                ImGui::Spacing();
                ImGui::TextColored(AtlusTheme::ColNeonViolet, "Key Hold Duration (ms):");
                ImGui::SameLine();
                ImGui::TextDisabled("(Time key is held down)");
                ImGui::SetNextItemWidth(550.0f);
                if (ImGui::SliderInt("##KeyHold", &settings.keyHoldDurationMs, 10, 200, "%d ms")) {
                    settingsLiveChanged = true;
                }
                if (ImGui::IsItemDeactivatedAfterEdit()) needDiskSave = true;

                // Jitter Slider
                ImGui::Spacing();
                AtlusTheme::RenderSectionDivider("HUMANIZATION JITTER");
                ImGui::TextColored(AtlusTheme::ColNeonViolet, "Random Timing Jitter (± ms):");
                ImGui::SameLine();
                ImGui::TextDisabled("(Natural variance added to timings)");
                ImGui::SetNextItemWidth(550.0f);
                if (ImGui::SliderInt("##Jitter", &settings.jitterMs, 0, 100, "± %d ms")) {
                    settingsLiveChanged = true;
                }
                if (ImGui::IsItemDeactivatedAfterEdit()) needDiskSave = true;

                // Loop Delay Slider
                ImGui::Spacing();
                ImGui::TextColored(AtlusTheme::ColNeonViolet, "Loop Restart Delay (ms):");
                ImGui::SameLine();
                ImGui::TextDisabled("(Delay before repeating cycle if trigger remains held)");
                ImGui::SetNextItemWidth(550.0f);
                if (ImGui::SliderInt("##LoopDelay", &settings.loopDelayMs, 0, 3000, "%d ms")) {
                    settingsLiveChanged = true;
                }
                if (ImGui::IsItemDeactivatedAfterEdit()) needDiskSave = true;

                // Fast Double-Tap Mode
                ImGui::Spacing();
                AtlusTheme::RenderSectionDivider("DOUBLE-TAP ACCELERATION");
                if (ImGui::Checkbox("Enable Fast Double-Tap Mode", &settings.useFastDoubleTap)) {
                    settingsLiveChanged = true;
                    needDiskSave = true;
                }
                ImGui::SameLine();
                ImGui::TextDisabled("(Accelerates identical double-taps e.g. 2->2)");

                if (settings.useFastDoubleTap) {
                    ImGui::SetNextItemWidth(550.0f);
                    if (ImGui::SliderInt("Confirmation Tap Delay##DoubleTap", &settings.doubleTapDelayMs, 15, 300, "%d ms")) {
                        settingsLiveChanged = true;
                    }
                    if (ImGui::IsItemDeactivatedAfterEdit()) needDiskSave = true;
                }

                ImGui::EndTabItem();
            }

            // =====================================================================
            // TAB 3: INPUT & TRIGGER CONFIGURATION
            // =====================================================================
            if (ImGui::BeginTabItem("  [ 3. TRIGGER & CONTROLS ]  ")) {
                ImGui::Spacing();
                AtlusTheme::RenderSectionDivider("TRIGGER BINDING");

                ImGui::TextColored(AtlusTheme::ColNeonViolet, "Trigger Activation Key:");
                ImGui::SetNextItemWidth(450.0f);
                if (ImGui::BeginCombo("##TriggerCombo", triggerOptions[selectedTriggerIndex].name)) {
                    for (int n = 0; n < (int)triggerOptions.size(); n++) {
                        const bool isSelected = (selectedTriggerIndex == n);
                        if (ImGui::Selectable(triggerOptions[n].name, isSelected)) {
                            selectedTriggerIndex = n;
                            settings.triggerKeyVk = triggerOptions[n].vk;
                            settingsLiveChanged = true;
                            needDiskSave = true;
                        }
                        if (isSelected) {
                            ImGui::SetItemDefaultFocus();
                        }
                    }
                    ImGui::EndCombo();
                }

                ImGui::Spacing();
                AtlusTheme::RenderSectionDivider("MASTER HOTKEY");

                ImGui::TextColored(AtlusTheme::ColNeonViolet, "Global Master Toggle Hotkey:");
                ImGui::SetNextItemWidth(450.0f);
                if (ImGui::BeginCombo("##HotkeyCombo", hotkeyOptions[selectedHotkeyIndex].name)) {
                    for (int n = 0; n < (int)hotkeyOptions.size(); n++) {
                        const bool isSelected = (selectedHotkeyIndex == n);
                        if (ImGui::Selectable(hotkeyOptions[n].name, isSelected)) {
                            selectedHotkeyIndex = n;
                            settings.toggleHotkeyVk = hotkeyOptions[n].vk;
                            settingsLiveChanged = true;
                            needDiskSave = true;
                        }
                        if (isSelected) {
                            ImGui::SetItemDefaultFocus();
                        }
                    }
                    ImGui::EndCombo();
                }

                ImGui::Spacing();
                if (ImGui::Checkbox("Audio Chime on Hotkey Toggle", &settings.soundBeepOnToggle)) {
                    settingsLiveChanged = true;
                    needDiskSave = true;
                }

                ImGui::Spacing();
                AtlusTheme::RenderSectionDivider("INJECTION DRIVER");
                if (ImGui::RadioButton("DirectInput Hardware ScanCode (Deadlock / Source 2)", settings.inputMode == 0)) {
                    settings.inputMode = 0;
                    settingsLiveChanged = true;
                    needDiskSave = true;
                }
                if (ImGui::RadioButton("Standard Virtual Key (Win32)", settings.inputMode == 1)) {
                    settings.inputMode = 1;
                    settingsLiveChanged = true;
                    needDiskSave = true;
                }

                ImGui::EndTabItem();
            }

            // =====================================================================
            // TAB 4: SYSTEM EVENT LOG
            // =====================================================================
            if (ImGui::BeginTabItem("  [ 4. TELEMETRY LOG ]  ")) {
                ImGui::Spacing();
                AtlusTheme::RenderSectionDivider("EVENT MONITOR");

                if (ImGui::Button("Clear Console", ImVec2(160.0f, 28.0f))) {
                    engine.ClearLogs();
                }
                ImGui::SameLine();
                ImGui::Checkbox("Auto-Scroll", &autoScrollLog);

                ImGui::Spacing();
                ImGui::PushFont(g_pLogFont);
                ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.06f, 0.04f, 0.11f, 1.0f));
                ImGui::BeginChild("LogTerminal", ImVec2(0.0f, -8.0f), true, ImGuiWindowFlags_AlwaysVerticalScrollbar);

                auto logs = engine.GetRecentLogs(300);
                for (const auto& entry : logs) {
                    ImVec4 col = AtlusTheme::ColText;
                    if (entry.level == 1) col = AtlusTheme::ColNeonCyan;
                    else if (entry.level == 2) col = AtlusTheme::ColNeonRed;

                    ImGui::TextColored(AtlusTheme::ColTextMuted, "[%s]", entry.timestamp.c_str());
                    ImGui::SameLine();
                    ImGui::TextColored(col, "%s", entry.message.c_str());
                }

                if (autoScrollLog && ImGui::GetScrollY() >= ImGui::GetScrollMaxY() - 20.0f) {
                    ImGui::SetScrollHereY(1.0f);
                }

                ImGui::EndChild();
                ImGui::PopStyleColor();
                ImGui::PopFont();

                ImGui::EndTabItem();
            }

            // =====================================================================
            // TAB 5: SYSTEM SPECIFICATION & PROFILE
            // =====================================================================
            if (ImGui::BeginTabItem("  [ 5. PROFILES & INFO ]  ")) {
                ImGui::Spacing();
                AtlusTheme::RenderSectionDivider("PROFILE MANAGEMENT");

                ImGui::Text("Active Profile: settings.ini");
                ImGui::Spacing();

                if (ImGui::Button("Save Configuration", ImVec2(220.0f, 32.0f))) {
                    settings.SaveToFile(configPath);
                    engine.AddLog("CONFIG MANUALLY SAVED TO SETTINGS.INI", 1);
                }
                ImGui::SameLine();
                if (ImGui::Button("Reload from File", ImVec2(200.0f, 32.0f))) {
                    if (settings.LoadFromFile(configPath)) {
                        engine.SetSettings(settings);
                        strncpy_s(sequenceBuffer, sizeof(sequenceBuffer), settings.sequenceString.c_str(), _TRUNCATE);
                        for (size_t i = 0; i < triggerOptions.size(); ++i) {
                            if (triggerOptions[i].vk == settings.triggerKeyVk) selectedTriggerIndex = (int)i;
                        }
                        for (size_t i = 0; i < hotkeyOptions.size(); ++i) {
                            if (hotkeyOptions[i].vk == settings.toggleHotkeyVk) selectedHotkeyIndex = (int)i;
                        }
                        engine.AddLog("CONFIG RELOADED FROM DISK", 1);
                    }
                }
                ImGui::SameLine();
                if (ImGui::Button("Reset to Defaults", ImVec2(220.0f, 32.0f))) {
                    settings.ResetToDefaults();
                    settingsLiveChanged = true;
                    needDiskSave = true;
                    strncpy_s(sequenceBuffer, sizeof(sequenceBuffer), settings.sequenceString.c_str(), _TRUNCATE);
                    for (size_t i = 0; i < triggerOptions.size(); ++i) {
                        if (triggerOptions[i].vk == settings.triggerKeyVk) selectedTriggerIndex = (int)i;
                    }
                    for (size_t i = 0; i < hotkeyOptions.size(); ++i) {
                        if (hotkeyOptions[i].vk == settings.toggleHotkeyVk) selectedHotkeyIndex = (int)i;
                    }
                    engine.AddLog("CONFIG RESET TO FACTORY SPECIFICATIONS", 2);
                }

                ImGui::Spacing();
                AtlusTheme::RenderSectionDivider("OPERATION INSTRUCTIONS");
                ImGui::BulletText("Hold Mouse Back (XBUTTON1 / M4) to execute combo repetition.");
                ImGui::BulletText("Releasing trigger key aborts execution instantly.");
                ImGui::BulletText("Press F6 globally to toggle macro enabled state.");
                ImGui::BulletText("DirectInput scan codes ensure full compatibility with Deadlock.");

                ImGui::EndTabItem();
            }

            ImGui::EndTabBar();
        }

        // Apply live settings changes if modified
        if (settingsLiveChanged) {
            settings.ValidateAndClamp();
            engine.SetSettings(settings);
            settingsLiveChanged = false;
        }

        // Persist to disk when user finishes editing or triggers an action
        if (needDiskSave) {
            settings.ValidateAndClamp();
            settings.SaveToFile(configPath);
            needDiskSave = false;
        }

        ImGui::End();
        ImGui::PopFont();

        // Render Dear ImGui draw data
        ImGui::Render();
        const float clear_color_with_alpha[4] = { 0.118f, 0.114f, 0.102f, 1.0f };
        g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, nullptr);
        g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, clear_color_with_alpha);
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

        // Present with vsync (1 = 60/144 fps, 0% idle CPU)
        g_pSwapChain->Present(1, 0);
    }

    // Ensure latest settings are persisted upon exit
    settings.SaveToFile(configPath);

    // Cleanup and release
    engine.Shutdown();

    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    CleanupDeviceD3D();
    ::DestroyWindow(hWnd);
    ::UnregisterClassW(wc.lpszClassName, wc.hInstance);

    return 0;
}

// Helper functions for Direct3D 11
bool CreateDeviceD3D(HWND hWnd) {
    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferCount = 2;
    sd.BufferDesc.Width = 0;
    sd.BufferDesc.Height = 0;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hWnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    UINT createDeviceFlags = 0;
    D3D_FEATURE_LEVEL featureLevel;
    const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
    HRESULT res = D3D11CreateDeviceAndSwapChain(
        nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, createDeviceFlags,
        featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain,
        &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext
    );
    if (res == DXGI_ERROR_UNSUPPORTED) {
        res = D3D11CreateDeviceAndSwapChain(
            nullptr, D3D_DRIVER_TYPE_WARP, nullptr, createDeviceFlags,
            featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain,
            &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext
        );
    }
    if (res != S_OK) return false;

    CreateRenderTarget();
    return true;
}

void CleanupDeviceD3D() {
    CleanupRenderTarget();
    if (g_pSwapChain) { g_pSwapChain->Release(); g_pSwapChain = nullptr; }
    if (g_pd3dDeviceContext) { g_pd3dDeviceContext->Release(); g_pd3dDeviceContext = nullptr; }
    if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = nullptr; }
}

void CreateRenderTarget() {
    ID3D11Texture2D* pBackBuffer = nullptr;
    g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
    if (pBackBuffer) {
        g_pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &g_mainRenderTargetView);
        pBackBuffer->Release();
    }
}

void CleanupRenderTarget() {
    if (g_mainRenderTargetView) {
        g_mainRenderTargetView->Release();
        g_mainRenderTargetView = nullptr;
    }
}

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg) {
    case WM_SIZE:
        if (wParam == SIZE_MINIMIZED)
            return 0;
        g_ResizeWidth = (UINT)LOWORD(lParam);
        g_ResizeHeight = (UINT)HIWORD(lParam);
        return 0;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
            return 0;
        break;
    case WM_DESTROY:
        ::PostQuitMessage(0);
        return 0;
    }
    return ::DefWindowProcW(hWnd, msg, wParam, lParam);
}
