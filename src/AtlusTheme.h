#pragma once

#include "imgui.h"

namespace AtlusTheme {
    // Atlus (Persona / Metaphor: ReFantazio) Dark Purple Color Palette
    inline const ImVec4 ColDarkBg       = ImVec4(0.047f, 0.035f, 0.086f, 1.00f); // #0c0916 (Midnight Abyss)
    inline const ImVec4 ColFrameBg      = ImVec4(0.098f, 0.071f, 0.176f, 1.00f); // #19122d (Obsidian Violet Card)
    inline const ImVec4 ColFrameBgHover = ImVec4(0.157f, 0.110f, 0.275f, 1.00f); // #281c46 (Elevated Card Hover)
    inline const ImVec4 ColFrameBgAct   = ImVec4(0.235f, 0.149f, 0.392f, 1.00f); // #3c2664 (Active Accent Surface)
    inline const ImVec4 ColBorder       = ImVec4(0.420f, 0.239f, 0.690f, 1.00f); // #6b3db0 (Crisp Electric Violet Border)
    inline const ImVec4 ColBorderDim    = ImVec4(0.247f, 0.165f, 0.408f, 0.85f); // #3f2a68 (Subtle Violet Divider)
    inline const ImVec4 ColBorderGlow   = ImVec4(0.851f, 0.271f, 0.941f, 1.00f); // #d945f0 (Neon Magenta Glow)
    inline const ImVec4 ColText         = ImVec4(1.000f, 1.000f, 1.000f, 1.00f); // #ffffff (Pure White)
    inline const ImVec4 ColTextMuted    = ImVec4(0.608f, 0.529f, 0.745f, 1.00f); // #9b87be (Muted Lavender)
    inline const ImVec4 ColNeonViolet   = ImVec4(0.733f, 0.345f, 0.984f, 1.00f); // #bb58fb (Electric Lilac/Violet)
    inline const ImVec4 ColNeonMagenta  = ImVec4(0.898f, 0.176f, 0.941f, 1.00f); // #e52df0 (Signature Atlus Neon Magenta)
    inline const ImVec4 ColNeonCyan     = ImVec4(0.024f, 0.776f, 0.898f, 1.00f); // #06c6e5 (Electric Cyan Indicator)
    inline const ImVec4 ColNeonGreen    = ImVec4(0.188f, 0.867f, 0.529f, 1.00f); // #30dd87 (Active Green)
    inline const ImVec4 ColNeonRed      = ImVec4(0.961f, 0.235f, 0.369f, 1.00f); // #f53c5e (Alert Crimson)

    // Applies full Atlus Dark Purple theme and angular geometry to Dear ImGui
    void ApplyTheme();

    // Custom Atlus stylized UI components
    void RenderHeaderBanner(const char* title, const char* subtitle = nullptr);
    void RenderSectionDivider(const char* label);
    void RenderStatusBadge(const char* title, const char* status, bool isActive, const ImVec4& color);
    void RenderKeyStep(const char* label, int index, bool isCurrent, float width = 68.0f, float height = 52.0f);
}
