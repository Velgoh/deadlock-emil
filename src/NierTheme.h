#pragma once

#include "imgui.h"

namespace NierTheme {
    // NieR:Automata Color Palette Constants
    inline const ImVec4 ColDarkBg      = ImVec4(0.118f, 0.114f, 0.102f, 1.00f); // #1e1d1a (Warm Charcoal)
    inline const ImVec4 ColFrameBg     = ImVec4(0.145f, 0.141f, 0.125f, 1.00f); // #252420 (Card / Frame Bg)
    inline const ImVec4 ColFrameBgHover= ImVec4(0.190f, 0.184f, 0.165f, 1.00f);
    inline const ImVec4 ColFrameBgAct  = ImVec4(0.240f, 0.230f, 0.205f, 1.00f);
    inline const ImVec4 ColBorder      = ImVec4(0.360f, 0.337f, 0.294f, 1.00f); // #5c564b (Crisp Border)
    inline const ImVec4 ColBorderDim   = ImVec4(0.250f, 0.235f, 0.210f, 0.80f);
    inline const ImVec4 ColText        = ImVec4(0.855f, 0.831f, 0.784f, 1.00f); // #dad4c8 (Warm Sand)
    inline const ImVec4 ColTextMuted   = ImVec4(0.541f, 0.522f, 0.482f, 1.00f); // #8a857b (Subdued Taupe)
    inline const ImVec4 ColGold        = ImVec4(0.784f, 0.659f, 0.294f, 1.00f); // #c8a84b (YoRHa Ochre Gold)
    inline const ImVec4 ColGoldBright  = ImVec4(0.880f, 0.760f, 0.380f, 1.00f); // #dfbe5e
    inline const ImVec4 ColGreen       = ImVec4(0.420f, 0.750f, 0.450f, 1.00f); // Tactical Green
    inline const ImVec4 ColRed         = ImVec4(0.820f, 0.300f, 0.280f, 1.00f); // Tactical Red / Alert

    // Applies full YoRHa style and color palette to Dear ImGui
    void ApplyTheme();

    // Custom UI Components with YoRHa Aesthetic
    void RenderHeaderBanner(const char* title, const char* subtitle = nullptr);
    void RenderSectionDivider(const char* label);
    void RenderStatusBadge(const char* title, const char* status, bool isActive, const ImVec4& color);
    void RenderKeyStep(const char* label, int index, bool isCurrent, float width = 64.0f, float height = 48.0f);
}
