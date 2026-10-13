#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <d3d11.h>
#include <dxgi.h>
#include <dwmapi.h>
#include <tchar.h>
#include <string>
#include <vector>
#include <iostream>
#include <algorithm>

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

// Atlus Font Handles
static ImFont*                  g_pFontSegoeBold = nullptr;
static ImFont*                  g_pFontImpactHuge = nullptr;
static ImFont*                  g_pFontImpactHeader = nullptr;
static ImFont*                  g_pFontImpactButton = nullptr;
static ImFont*                  g_pFontImpactTag = nullptr;
static ImFont*                  g_pFontMonoInput = nullptr;
static ImFont*                  g_pFontMonoLog = nullptr;

// Dynamic Screen Shake System
static AtlusTheme::ScreenShake  g_ScreenShake;
static bool                     g_lastMacroEnabled = true;
static bool                     g_lastSequenceRunning = false;

bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void CreateRenderTarget();
void CleanupRenderTarget();
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

static std::wstring GetNierEmilTitle() {
    static const wchar_t* const titles[] = {
        L"[E]mil macro",
        L"e[M]il macro",
        L"em[I]l macro",
        L"emi[L] macro",
        L"emil [M]acro",
        L"emil m[A]cro",
        L"emil ma[C]ro",
        L"emil mac[R]o",
        L"emil macr[O]",
    };
    static const size_t count = sizeof(titles) / sizeof(titles[0]);
    static size_t lastIdx = 5; // Start with 'emil m[A]cro'
    static ULONGLONG lastTick = 0;
    ULONGLONG now = GetTickCount64();
    if (lastTick == 0) {
        srand((unsigned int)now);
        lastTick = now;
    } else if (now - lastTick > 3500) {
        lastIdx = (lastIdx + 1 + (rand() % (count - 1))) % count;
        lastTick = now;
    }
    return titles[lastIdx];
}

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
    (void)hInstance; (void)hPrevInstance; (void)lpCmdLine; (void)nCmdShow;

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
        L"EmilMacroWindowClass",
        nullptr
    };
    ::RegisterClassExW(&wc);

    // Calculate window position and size (1140 x 860)
    int windowWidth = 1140;
    int windowHeight = 860;
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);
    int posX = (std::max)(40, (screenWidth - windowWidth) / 2);
    int posY = (std::max)(40, (screenHeight - windowHeight) / 2);

    std::wstring initialTitle = GetNierEmilTitle();
    HWND hWnd = ::CreateWindowW(
        wc.lpszClassName,
        initialTitle.c_str(),
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
    ImFontConfig cfg;
    cfg.OversampleH = 2;
    cfg.OversampleV = 2;

    g_pFontSegoeBold = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\segoeuib.ttf", 16.0f, &cfg);
    if (!g_pFontSegoeBold) g_pFontSegoeBold = io.Fonts->Fonts[0];

    g_pFontImpactHuge = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\impact.ttf", 30.0f, &cfg);
    if (!g_pFontImpactHuge) g_pFontImpactHuge = g_pFontSegoeBold;

    g_pFontImpactHeader = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\impact.ttf", 22.0f, &cfg);
    if (!g_pFontImpactHeader) g_pFontImpactHeader = g_pFontSegoeBold;

    g_pFontImpactButton = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\impact.ttf", 17.5f, &cfg);
    if (!g_pFontImpactButton) g_pFontImpactButton = g_pFontSegoeBold;

    g_pFontImpactTag = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\impact.ttf", 14.5f, &cfg);
    if (!g_pFontImpactTag) g_pFontImpactTag = g_pFontSegoeBold;

    g_pFontMonoInput = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\consola.ttf", 16.0f, &cfg);
    if (!g_pFontMonoInput) g_pFontMonoInput = g_pFontSegoeBold;

    g_pFontMonoLog = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\consola.ttf", 14.0f, &cfg);
    if (!g_pFontMonoLog) g_pFontMonoLog = g_pFontSegoeBold;

    // Apply Atlus Dark Purple theme
    AtlusTheme::ApplyTheme();

    // Setup Platform/Renderer backends
    ImGui_ImplWin32_Init(hWnd);
    ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);

    // Load configuration & initialize Macro Engine
    std::string configPath = GetExeDirectory() + "\\settings.ini";
    MacroSettings settings;
    if (!settings.LoadFromFile(configPath)) {
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
    int activeTab = 0;

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

        // Update dynamic NieR ending bracket window title
        std::wstring curTitle = GetNierEmilTitle();
        static std::wstring lastSetTitle = L"";
        if (curTitle != lastSetTitle) {
            ::SetWindowTextW(hWnd, curTitle.c_str());
            lastSetTitle = curTitle;
        }

        // Start Dear ImGui frame
        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        // 1. Screen Shake & Viewport Offset
        ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImVec2 shakeOffset = g_ScreenShake.Update(io.DeltaTime);
        ImGui::SetNextWindowPos(ImVec2(viewport->WorkPos.x + shakeOffset.x, viewport->WorkPos.y + shakeOffset.y));
        ImGui::SetNextWindowSize(viewport->WorkSize);

        // 2. Draw Atlus Halftone Dot Matrix & Comic Slash Backdrop (Shakes synchronously with viewport)
        AtlusTheme::DrawHalftoneBackdrop(viewport->WorkPos, viewport->WorkSize, shakeOffset);

        // Check engine state transitions to trigger screen shake dynamically
        bool isEnabled = engine.IsMacroEnabled();
        bool isRunning = engine.IsSequenceRunning();
        bool isTriggerHeld = engine.IsTriggerHeld();
        int currentStep = engine.GetCurrentStepIndex();
        uint64_t cycles = engine.GetCycleCount();
        uint64_t keysPressed = engine.GetKeysPressedCount();

        if (isEnabled != g_lastMacroEnabled) {
            g_ScreenShake.Trigger(0.35f, 9.0f);
            g_lastMacroEnabled = isEnabled;
        }
        if (isRunning && !g_lastSequenceRunning) {
            g_ScreenShake.Trigger(0.25f, 6.0f);
        }
        g_lastSequenceRunning = isRunning;

        // Begin Main Fullscreen Window Container
        ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove |
                                       ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoBringToFrontOnFocus |
                                       ImGuiWindowFlags_NoBackground;

        ImGui::PushFont(g_pFontSegoeBold);
        ImGui::Begin("AtlusMainWindow", nullptr, windowFlags);

        // 3. Top Calling Card Header Banner
        bool toggleSound = false;
        AtlusTheme::RenderCallingCardHeader("EMIL", "MACRO", nullptr,
                                            "ALL-OUT DIRECTINPUT ENGINE",
                                            isEnabled, settings.soundBeepOnToggle, &toggleSound,
                                            g_pFontImpactHuge, g_pFontImpactButton, g_pFontImpactTag);
        if (toggleSound) {
            settings.soundBeepOnToggle = !settings.soundBeepOnToggle;
            settingsLiveChanged = true;
            needDiskSave = true;
        }

        // 4. Master Combat Action Controls Bar
        {
            float availW = ImGui::GetContentRegionAvail().x;
            float btnW = (availW - 14.0f) * 0.5f;

            if (isEnabled) {
                if (AtlusTheme::AtlusButton("[ MASTER ENGAGED · PRESS F6 TO SUSPEND ]",
                                            ImVec2(btnW, 46.0f),
                                            AtlusTheme::U32NeonMagenta,
                                            AtlusTheme::U32PureWhite,
                                            g_pFontImpactButton, 10.0f, true)) {
                    engine.ToggleMacroEnabled();
                    g_ScreenShake.Trigger(0.35f, 9.0f);
                }
            } else {
                if (AtlusTheme::AtlusButton("[ ENGAGE MASTER PROTOCOL · F6 ]",
                                            ImVec2(btnW, 46.0f),
                                            AtlusTheme::U32ObsidianInput,
                                            AtlusTheme::U32NeonRed,
                                            g_pFontImpactButton, 10.0f, true)) {
                    engine.ToggleMacroEnabled();
                    g_ScreenShake.Trigger(0.35f, 9.0f);
                }
            }

            ImGui::SameLine();
            if (AtlusTheme::AtlusButton("[ EMERGENCY ABORT & RELEASE ]",
                                        ImVec2(btnW, 46.0f),
                                        AtlusTheme::U32ObsidianInput,
                                        AtlusTheme::U32YellowHighlight,
                                        g_pFontImpactButton, 10.0f, true)) {
                engine.PanicRelease();
                engine.AddLog("EMERGENCY ABORT COMMAND DISPATCHED", 2);
                g_ScreenShake.Trigger(0.40f, 11.0f);
            }
        }

        ImGui::Spacing();

        // 5. Tactical HUD Status Row (4 symmetrically spanned badges with Impact typography)
        {
            float availW = ImGui::GetContentRegionAvail().x;
            float badgeW = (availW - 36.0f) / 4.0f;

            // Badge 1: System Master
            AtlusTheme::RenderStatusBadge("SYSTEM MASTER", isEnabled ? "ENGAGED [ON]" : "SUSPENDED [OFF]",
                                          isEnabled, isEnabled ? AtlusTheme::ColNeonMagenta : AtlusTheme::ColNeonRed,
                                          badgeW, 52.0f, g_pFontImpactTag, g_pFontImpactButton);
            ImGui::SameLine();

            // Badge 2: Trigger State
            AtlusTheme::RenderStatusBadge("TRIGGER INPUT", isTriggerHeld ? "HELD [ACTIVE]" : "STANDBY",
                                          isTriggerHeld, isTriggerHeld ? AtlusTheme::ColNeonCyan : AtlusTheme::ColTextMuted,
                                          badgeW, 52.0f, g_pFontImpactTag, g_pFontImpactButton);
            ImGui::SameLine();

            // Badge 3: Sequence Firing
            char stepStr[64];
            if (isRunning) {
                snprintf(stepStr, sizeof(stepStr), "STEP #%d FIRING", currentStep + 1);
            } else {
                snprintf(stepStr, sizeof(stepStr), "IDLE / STANDBY");
            }
            AtlusTheme::RenderStatusBadge("SEQUENCE RUNNER", stepStr, isRunning,
                                          isRunning ? AtlusTheme::ColNeonViolet : AtlusTheme::ColTextMuted,
                                          badgeW, 52.0f, g_pFontImpactTag, g_pFontImpactButton);
            ImGui::SameLine();

            // Badge 4: Cycle Stats
            char statsStr[64];
            snprintf(statsStr, sizeof(statsStr), "CYC: %llu | KEYS: %llu", cycles, keysPressed);
            AtlusTheme::RenderStatusBadge("CYCLE TELEMETRY", statsStr, cycles > 0,
                                          cycles > 0 ? AtlusTheme::ColNeonGreen : AtlusTheme::ColTextMuted,
                                          badgeW, 52.0f, g_pFontImpactTag, g_pFontImpactButton);
        }

        ImGui::Spacing();

        // 6. Atlus Navigation Bar (5 Slanted Action Tabs - Punchy Minimalist Labels)
        {
            float availW = ImGui::GetContentRegionAvail().x;
            const char* tabNames[5] = {
                "[ 1. SEQUENCE ]",
                "[ 2. TIMINGS ]",
                "[ 3. HARDWARE ]",
                "[ 4. TELEMETRY ]",
                "[ 5. PROFILES ]"
            };
            float tabW = (availW - 32.0f) / 5.0f;

            for (int i = 0; i < 5; ++i) {
                bool isSel = (activeTab == i);
                ImU32 fill = isSel ? AtlusTheme::U32NeonMagenta : AtlusTheme::U32ObsidianInput;
                ImU32 text = isSel ? AtlusTheme::U32PureWhite : AtlusTheme::U32TextMuted;

                if (AtlusTheme::AtlusButton(tabNames[i], ImVec2(tabW, 36.0f), fill, text, g_pFontImpactTag, 8.0f, false, isSel)) {
                    activeTab = i;
                }
                if (i < 4) ImGui::SameLine();
            }
        }

        ImGui::Spacing();

        // 7. Skewed Card (-4.5 degrees) Main Content Area
        ImVec2 cardMin = ImGui::GetCursorScreenPos();
        float cardW = ImGui::GetContentRegionAvail().x;
        float cardH = ImGui::GetContentRegionAvail().y - 34.0f;

        AtlusTheme::DrawSkewCard(cardMin, ImVec2(cardMin.x + cardW - 14.0f, cardMin.y + cardH),
                                 AtlusTheme::U32ObsidianCard, AtlusTheme::U32ComicBlack,
                                 -4.5f, 3.0f, 6.0f, AtlusTheme::U32ElectricPurple);

        float cardSlant = cardH * 0.0787f;
        ImGui::SetCursorScreenPos(ImVec2(cardMin.x + cardSlant + 20.0f, cardMin.y + 12.0f));
        ImGui::BeginChild("CardInnerContent", ImVec2(cardW - cardSlant - 44.0f, cardH - 24.0f), false, ImGuiWindowFlags_None);

        // =====================================================================
        // TAB 0: COMBAT SEQUENCE CONFIGURATION
        // =====================================================================
        if (activeTab == 0) {
            AtlusTheme::AtlusTag("[ 1. COMBAT SEQUENCE CONFIGURATION ]", -1.5f,
                                 AtlusTheme::U32PureWhite, AtlusTheme::U32ComicBlack, g_pFontImpactTag);

            ImGui::Spacing();
            ImGui::TextColored(AtlusTheme::ColElectricPurpleBright, "KEY SEQUENCE INPUT:");
            ImGui::PushFont(g_pFontMonoInput);
            ImGui::PushStyleColor(ImGuiCol_FrameBg, AtlusTheme::ColObsidianInput);
            ImGui::PushStyleColor(ImGuiCol_Border, AtlusTheme::ColElectricPurple);
            ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 2.0f);
            ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - 146.0f);

            if (ImGui::InputText("##SequenceInput", sequenceBuffer, sizeof(sequenceBuffer))) {
                settings.sequenceString = sequenceBuffer;
                settingsLiveChanged = true;
            }
            if (ImGui::IsItemDeactivatedAfterEdit()) {
                needDiskSave = true;
            }
            ImGui::PopStyleVar();
            ImGui::PopStyleColor(2);
            ImGui::PopFont();

            ImGui::SameLine();
            if (AtlusTheme::AtlusButton("RESET DEFAULT", ImVec2(136.0f, 34.0f),
                                        AtlusTheme::U32ObsidianInput, AtlusTheme::U32YellowHighlight,
                                        g_pFontImpactTag, 6.0f, true)) {
                strncpy_s(sequenceBuffer, sizeof(sequenceBuffer), "2, 2, z, z, 2, 2, x, x, 3, 3", _TRUNCATE);
                settings.sequenceString = sequenceBuffer;
                settingsLiveChanged = true;
                needDiskSave = true;
            }

            ImGui::Spacing();
            if (AtlusTheme::AtlusCheckbox("AUTO DOUBLE-TAP SHORTHAND (123ZC -> 1,1,2,2,3,3,Z,Z,C,C)", &settings.autoDoubleTap, g_pFontImpactTag)) {
                settingsLiveChanged = true;
                needDiskSave = true;
            }

            // Sequence validation status badge
            std::string parseErr = engine.GetParseError();
            auto parsedSteps = engine.GetParsedSteps();

            ImGui::Spacing();
            if (!parseErr.empty()) {
                char errBuf[256];
                snprintf(errBuf, sizeof(errBuf), "PARSE ERROR: %s", parseErr.c_str());
                AtlusTheme::AtlusTag(errBuf, 0.0f, AtlusTheme::U32NeonRed, AtlusTheme::U32PureWhite, g_pFontImpactTag);
            } else {
                char okBuf[128];
                snprintf(okBuf, sizeof(okBuf), "STATUS: READY · %d STEPS CONFIGURED", (int)parsedSteps.size());
                AtlusTheme::AtlusTag(okBuf, 0.0f, AtlusTheme::U32NeonGreen, AtlusTheme::U32ComicBlack, g_pFontImpactTag);
            }

            ImGui::Spacing();
            AtlusTheme::AtlusTag("[ STEP VISUALIZER ]", 1.2f, AtlusTheme::U32ElectricPurple, AtlusTheme::U32PureWhite, g_pFontImpactTag);
            ImGui::Spacing();

            if (!parsedSteps.empty()) {
                ImGui::BeginChild("StepsContainer", ImVec2(0.0f, 68.0f), false, ImGuiWindowFlags_HorizontalScrollbar);
                for (int i = 0; i < (int)parsedSteps.size(); ++i) {
                    bool isCurrent = isRunning && (currentStep == i);
                    AtlusTheme::AtlusKeyStep(parsedSteps[i].label.c_str(), i, isCurrent, 66.0f, 50.0f, g_pFontImpactButton, g_pFontMonoLog);
                    if (i + 1 < (int)parsedSteps.size()) {
                        ImGui::SameLine();
                    }
                }
                ImGui::EndChild();
            }

            ImGui::Spacing();
            AtlusTheme::AtlusTag("[ QUICK COMBAT PRESETS ]", -1.2f, AtlusTheme::U32YellowHighlight, AtlusTheme::U32ComicBlack, g_pFontImpactTag);
            ImGui::Spacing();

            if (AtlusTheme::AtlusButton("PRESET 1: 2, 2, Z, Z, 2, 2, X, X, 3, 3", ImVec2(370.0f, 34.0f),
                                        AtlusTheme::U32ObsidianInput, AtlusTheme::U32PureWhite, g_pFontImpactTag, 6.0f, true)) {
                strncpy_s(sequenceBuffer, sizeof(sequenceBuffer), "2, 2, z, z, 2, 2, x, x, 3, 3", _TRUNCATE);
                settings.sequenceString = sequenceBuffer;
                settingsLiveChanged = true;
                needDiskSave = true;
            }
            ImGui::SameLine();
            if (AtlusTheme::AtlusButton("PRESET 2: 123ZC (SHORTHAND)", ImVec2(270.0f, 34.0f),
                                        AtlusTheme::U32ObsidianInput, AtlusTheme::U32PureWhite, g_pFontImpactTag, 6.0f, true)) {
                strncpy_s(sequenceBuffer, sizeof(sequenceBuffer), "123zc", _TRUNCATE);
                settings.sequenceString = sequenceBuffer;
                settingsLiveChanged = true;
                needDiskSave = true;
            }
            ImGui::SameLine();
            if (AtlusTheme::AtlusButton("PRESET 3: 1, 1, 2, 2, 3, 3, 4, 4", ImVec2(290.0f, 34.0f),
                                        AtlusTheme::U32ObsidianInput, AtlusTheme::U32PureWhite, g_pFontImpactTag, 6.0f, true)) {
                strncpy_s(sequenceBuffer, sizeof(sequenceBuffer), "1, 1, 2, 2, 3, 3, 4, 4", _TRUNCATE);
                settings.sequenceString = sequenceBuffer;
                settingsLiveChanged = true;
                needDiskSave = true;
            }
        }

        // =====================================================================
        // TAB 1: TIMINGS & DYNAMICS MATRIX
        // =====================================================================
        else if (activeTab == 1) {
            AtlusTheme::AtlusTag("[ 2. TIMING MATRIX & HUMANIZATION ]", -1.5f,
                                 AtlusTheme::U32NeonMagenta, AtlusTheme::U32PureWhite, g_pFontImpactTag);
            ImGui::Spacing();

            // Slider 1: Press Delay
            if (AtlusTheme::AtlusSliderInt("PressDelay", "INTER-PRESS DELAY", &settings.pressDelayMs,
                                          30, 2000, "%d MS", g_pFontImpactButton, g_pFontImpactTag)) {
                settingsLiveChanged = true;
                needDiskSave = true;
            }
            ImGui::Spacing();

            // Slider 2: Key Hold Duration
            if (AtlusTheme::AtlusSliderInt("KeyHold", "KEY HOLD DURATION", &settings.keyHoldDurationMs,
                                          10, 200, "%d MS", g_pFontImpactButton, g_pFontImpactTag)) {
                settingsLiveChanged = true;
                needDiskSave = true;
            }

            ImGui::Spacing();
            AtlusTheme::AtlusTag("[ HUMAN JITTER & LOOP DELAYS ]", 1.2f,
                                 AtlusTheme::U32ElectricPurple, AtlusTheme::U32PureWhite, g_pFontImpactTag);
            ImGui::Spacing();

            // Slider 3: Jitter Variance
            if (AtlusTheme::AtlusSliderInt("Jitter", "TIMING JITTER VARIANCE", &settings.jitterMs,
                                          0, 100, "+/- %d MS", g_pFontImpactButton, g_pFontImpactTag)) {
                settingsLiveChanged = true;
                needDiskSave = true;
            }
            ImGui::Spacing();

            // Slider 4: Cycle Repeat Delay
            if (AtlusTheme::AtlusSliderInt("LoopDelay", "CYCLE REPEAT DELAY", &settings.loopDelayMs,
                                          0, 3000, "%d MS", g_pFontImpactButton, g_pFontImpactTag)) {
                settingsLiveChanged = true;
                needDiskSave = true;
            }

            ImGui::Spacing();
            AtlusTheme::AtlusTag("[ DOUBLE-TAP ACCELERATION ]", -1.2f,
                                 AtlusTheme::U32YellowHighlight, AtlusTheme::U32ComicBlack, g_pFontImpactTag);
            ImGui::Spacing();

            if (AtlusTheme::AtlusCheckbox("FAST DOUBLE-TAP MODE", &settings.useFastDoubleTap, g_pFontImpactTag)) {
                settingsLiveChanged = true;
                needDiskSave = true;
            }

            if (settings.useFastDoubleTap) {
                ImGui::Spacing();
                if (AtlusTheme::AtlusSliderInt("DoubleTap", "CONFIRMATION TAP DELAY", &settings.doubleTapDelayMs,
                                              15, 300, "%d MS", g_pFontImpactButton, g_pFontImpactTag)) {
                    settingsLiveChanged = true;
                    needDiskSave = true;
                }
            }
        }

        // =====================================================================
        // TAB 2: TRIGGER & HARDWARE INJECTION
        // =====================================================================
        else if (activeTab == 2) {
            AtlusTheme::AtlusTag("[ 3. INPUT BINDINGS & DRIVER ]", -1.5f,
                                 AtlusTheme::U32YellowHighlight, AtlusTheme::U32ComicBlack, g_pFontImpactTag);
            ImGui::Spacing();

            ImGui::TextColored(AtlusTheme::ColElectricPurpleBright, "TRIGGER ACTIVATION KEY:");
            ImGui::PushStyleColor(ImGuiCol_FrameBg, AtlusTheme::ColObsidianInput);
            ImGui::PushStyleColor(ImGuiCol_Border, AtlusTheme::ColElectricPurple);
            ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 2.0f);
            ImGui::SetNextItemWidth(480.0f);
            if (ImGui::BeginCombo("##TriggerCombo", triggerOptions[selectedTriggerIndex].name)) {
                for (int n = 0; n < (int)triggerOptions.size(); n++) {
                    const bool isSelected = (selectedTriggerIndex == n);
                    if (ImGui::Selectable(triggerOptions[n].name, isSelected)) {
                        selectedTriggerIndex = n;
                        settings.triggerKeyVk = triggerOptions[n].vk;
                        settingsLiveChanged = true;
                        needDiskSave = true;
                    }
                    if (isSelected) ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }

            ImGui::Spacing();
            ImGui::TextColored(AtlusTheme::ColElectricPurpleBright, "GLOBAL MASTER TOGGLE HOTKEY:");
            ImGui::SetNextItemWidth(480.0f);
            if (ImGui::BeginCombo("##HotkeyCombo", hotkeyOptions[selectedHotkeyIndex].name)) {
                for (int n = 0; n < (int)hotkeyOptions.size(); n++) {
                    const bool isSelected = (selectedHotkeyIndex == n);
                    if (ImGui::Selectable(hotkeyOptions[n].name, isSelected)) {
                        selectedHotkeyIndex = n;
                        settings.toggleHotkeyVk = hotkeyOptions[n].vk;
                        settingsLiveChanged = true;
                        needDiskSave = true;
                    }
                    if (isSelected) ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }
            ImGui::PopStyleVar();
            ImGui::PopStyleColor(2);

            ImGui::Spacing();
            if (AtlusTheme::AtlusCheckbox("AUDIO CHIME ON HOTKEY TOGGLE", &settings.soundBeepOnToggle, g_pFontImpactTag)) {
                settingsLiveChanged = true;
                needDiskSave = true;
            }

            ImGui::Spacing();
            AtlusTheme::AtlusTag("[ INJECTION DRIVER PROTOCOL ]", 1.2f,
                                 AtlusTheme::U32ElectricPurple, AtlusTheme::U32PureWhite, g_pFontImpactTag);
            ImGui::Spacing();

            if (AtlusTheme::AtlusRadioButton("DIRECTINPUT HARDWARE SCANCODE", settings.inputMode == 0, g_pFontImpactTag)) {
                settings.inputMode = 0;
                settingsLiveChanged = true;
                needDiskSave = true;
            }
            ImGui::Spacing();
            if (AtlusTheme::AtlusRadioButton("STANDARD VIRTUAL KEY", settings.inputMode == 1, g_pFontImpactTag)) {
                settings.inputMode = 1;
                settingsLiveChanged = true;
                needDiskSave = true;
            }

            ImGui::Spacing();
            AtlusTheme::AtlusTag("DRIVER SPEC: DIRECT HARDWARE SCANCODES · < 5MS ABORT LATENCY ACTIVE",
                                 0.0f, AtlusTheme::U32ObsidianInput, AtlusTheme::U32YellowHighlight, g_pFontImpactTag);
        }

        // =====================================================================
        // TAB 3: TELEMETRY & EVENT STREAM
        // =====================================================================
        else if (activeTab == 3) {
            AtlusTheme::AtlusTag("[ 4. REAL-TIME EVENT STREAM ]", -1.5f,
                                 AtlusTheme::U32PureWhite, AtlusTheme::U32ComicBlack, g_pFontImpactTag);

            ImGui::Spacing();
            if (AtlusTheme::AtlusButton("CLEAR CONSOLE", ImVec2(150.0f, 32.0f),
                                        AtlusTheme::U32ObsidianInput, AtlusTheme::U32PureWhite, g_pFontImpactTag, 6.0f, true)) {
                engine.ClearLogs();
            }
            ImGui::SameLine();
            AtlusTheme::AtlusCheckbox("AUTO-SCROLL", &autoScrollLog, g_pFontImpactTag);

            ImGui::Spacing();
            ImGui::PushFont(g_pFontMonoLog);
            ImGui::PushStyleColor(ImGuiCol_ChildBg, AtlusTheme::ColObsidianInput);
            ImGui::PushStyleColor(ImGuiCol_Border, AtlusTheme::ColElectricPurple);
            ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 2.0f);
            ImGui::BeginChild("LogTerminal", ImVec2(0.0f, -8.0f), true, ImGuiWindowFlags_AlwaysVerticalScrollbar);

            auto logs = engine.GetRecentLogs(300);
            for (const auto& entry : logs) {
                ImVec4 col = AtlusTheme::ColPureWhite;
                if (entry.level == 1) col = AtlusTheme::ColNeonGreen;
                else if (entry.level == 2) col = AtlusTheme::ColNeonRed;

                ImGui::TextColored(AtlusTheme::ColTextMuted, "[%s]", entry.timestamp.c_str());
                ImGui::SameLine();
                ImGui::TextColored(col, "%s", entry.message.c_str());
            }

            if (autoScrollLog && ImGui::GetScrollY() >= ImGui::GetScrollMaxY() - 20.0f) {
                ImGui::SetScrollHereY(1.0f);
            }

            ImGui::EndChild();
            ImGui::PopStyleVar();
            ImGui::PopStyleColor(2);
            ImGui::PopFont();
        }

        // =====================================================================
        // TAB 4: PROFILES & COMBAT SPECS
        // =====================================================================
        else if (activeTab == 4) {
            AtlusTheme::AtlusTag("[ 5. CONFIGURATION PERSISTENCE ]", -1.5f,
                                 AtlusTheme::U32NeonMagenta, AtlusTheme::U32PureWhite, g_pFontImpactTag);

            ImGui::Spacing();
            ImGui::TextColored(AtlusTheme::ColYellowHighlight, "ACTIVE PROFILE: settings.ini");
            ImGui::Spacing();

            if (AtlusTheme::AtlusButton("SAVE CONFIGURATION", ImVec2(220.0f, 36.0f),
                                        AtlusTheme::U32NeonMagenta, AtlusTheme::U32PureWhite, g_pFontImpactTag, 6.0f, true)) {
                settings.SaveToFile(configPath);
                engine.AddLog("CONFIG MANUALLY PERSISTED TO SETTINGS.INI", 1);
            }
            ImGui::SameLine();
            if (AtlusTheme::AtlusButton("RELOAD FROM FILE", ImVec2(200.0f, 36.0f),
                                        AtlusTheme::U32ObsidianInput, AtlusTheme::U32PureWhite, g_pFontImpactTag, 6.0f, true)) {
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
            if (AtlusTheme::AtlusButton("RESET FACTORY DEFAULTS", ImVec2(240.0f, 36.0f),
                                        AtlusTheme::U32ObsidianInput, AtlusTheme::U32NeonRed, g_pFontImpactTag, 6.0f, true)) {
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
            AtlusTheme::AtlusTag("[ COMBAT PROTOCOL SPECS ]", 1.2f,
                                 AtlusTheme::U32ElectricPurple, AtlusTheme::U32PureWhite, g_pFontImpactTag);
            ImGui::Spacing();

            ImGui::TextColored(AtlusTheme::ColYellowHighlight, "◆");
            ImGui::SameLine();
            ImGui::Text("HOLD TRIGGER (M4) TO DISPATCH SEQUENCE CYCLES CONTINUOUSLY.");

            ImGui::TextColored(AtlusTheme::ColYellowHighlight, "◆");
            ImGui::SameLine();
            ImGui::Text("RELEASE TRIGGER FOR IMMEDIATE COMBO ABORT (< 5MS LATENCY).");

            ImGui::TextColored(AtlusTheme::ColYellowHighlight, "◆");
            ImGui::SameLine();
            ImGui::Text("GLOBAL HOTKEY (F6) TOGGLES SYSTEM MASTER STATE SYSTEM-WIDE.");

            ImGui::TextColored(AtlusTheme::ColYellowHighlight, "◆");
            ImGui::SameLine();
            ImGui::Text("HARDWARE SCANCODES INJECTED DIRECTLY VIA DIRECTINPUT DRIVER.");

            ImGui::TextColored(AtlusTheme::ColYellowHighlight, "◆");
            ImGui::SameLine();
            ImGui::Text("SHORTHAND NOTATION EXPANDS STRINGS (123ZC -> 1,1,2,2,3,3,Z,Z,C,C).");
        }

        ImGui::EndChild(); // CardInnerContent

        // 8. Bottom Comic Status Bar
        ImGui::SetCursorScreenPos(ImVec2(cardMin.x, cardMin.y + cardH + 4.0f));
        float barW = ImGui::GetContentRegionAvail().x;
        float barH = 26.0f;
        ImVec2 statusPoly[4] = {
            ImVec2(cardMin.x + 8.0f, cardMin.y + cardH + 4.0f),
            ImVec2(cardMin.x + barW, cardMin.y + cardH + 4.0f),
            ImVec2(cardMin.x + barW - 8.0f, cardMin.y + cardH + 4.0f + barH),
            ImVec2(cardMin.x, cardMin.y + cardH + 4.0f + barH)
        };
        ImDrawList* drawList = ImGui::GetWindowDrawList();
        drawList->AddConvexPolyFilled(statusPoly, 4, AtlusTheme::U32ObsidianInput);
        drawList->AddPolyline(statusPoly, 4, AtlusTheme::U32ComicBlack, ImDrawFlags_Closed, 2.0f);

        AtlusTheme::DrawTextWithFont(drawList, g_pFontImpactTag, 0.0f,
                                     ImVec2(cardMin.x + 16.0f, cardMin.y + cardH + 8.0f),
                                     AtlusTheme::U32YellowHighlight,
                                     "EMIL COMBAT MACRO · ATLUS METAVERSE DIRECTINPUT");

        char rightStatus[128];
        snprintf(rightStatus, sizeof(rightStatus), "STATE: [%s] | CYCLES: %llu | LATENCY: <5MS",
                 isEnabled ? "ENGAGED" : "SUSPENDED", cycles);
        ImVec2 rightSize = AtlusTheme::CalcTextSizeWithFont(g_pFontImpactTag, 0.0f, rightStatus);
        AtlusTheme::DrawTextWithFont(drawList, g_pFontImpactTag, 0.0f,
                                     ImVec2(cardMin.x + barW - rightSize.x - 20.0f, cardMin.y + cardH + 8.0f),
                                     isEnabled ? AtlusTheme::U32PureWhite : AtlusTheme::U32TextMuted,
                                     rightStatus);

        ImGui::Dummy(ImVec2(barW, barH + 6.0f));

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
        const float clear_color_with_alpha[4] = { 0.039f, 0.027f, 0.078f, 1.0f }; // Midnight Abyss
        g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, nullptr);
        g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, clear_color_with_alpha);
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

        // Present with vsync (1 = 60/144 fps)
        g_pSwapChain->Present(1, 0);
    }

    // Persist settings on exit
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
    const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0 };
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
