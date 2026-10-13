#pragma once

#include "imgui.h"
#include <cmath>
#include <string>
#include <vector>

namespace AtlusTheme {
    // Atlus (Persona 5 / Metaphor) Dark Purple Color Palette
    inline const ImVec4 ColMidnightAbyss       = ImVec4(0.039f, 0.027f, 0.078f, 1.0f); // #0a0714 (Deep Abyss)
    inline const ImVec4 ColObsidianCard         = ImVec4(0.078f, 0.051f, 0.141f, 1.0f); // #140d24 (Obsidian Violet Card)
    inline const ImVec4 ColObsidianCardHover    = ImVec4(0.125f, 0.082f, 0.220f, 1.0f); // #201538 (Card Hover)
    inline const ImVec4 ColObsidianInput        = ImVec4(0.071f, 0.039f, 0.133f, 1.0f); // #120a22 (Input Surface)
    inline const ImVec4 ColElectricPurple       = ImVec4(0.541f, 0.169f, 0.886f, 1.0f); // #8a2be2 (Electric Violet)
    inline const ImVec4 ColElectricPurpleBright = ImVec4(0.659f, 0.333f, 0.969f, 1.0f); // #a855f7 (Electric Lilac)
    inline const ImVec4 ColNeonMagenta          = ImVec4(0.878f, 0.141f, 0.765f, 1.0f); // #e024c3 (Signature Neon Magenta)
    inline const ImVec4 ColNeonMagentaBright    = ImVec4(0.851f, 0.275f, 0.937f, 1.0f); // #d946ef (High Magenta)
    inline const ImVec4 ColYellowHighlight      = ImVec4(1.000f, 0.902f, 0.000f, 1.0f); // #ffe600 (Atlus Vivid Yellow)
    inline const ImVec4 ColPureWhite            = ImVec4(1.000f, 1.000f, 1.000f, 1.0f); // #ffffff (Pure Comic White)
    inline const ImVec4 ColComicBlack           = ImVec4(0.000f, 0.000f, 0.000f, 1.0f); // #000000 (Ink Black)
    inline const ImVec4 ColTextMuted            = ImVec4(0.604f, 0.510f, 0.745f, 1.0f); // #9a82be (Muted Lavender)
    inline const ImVec4 ColBorderDim            = ImVec4(0.227f, 0.125f, 0.353f, 1.0f); // #3a205a (Divider)
    inline const ImVec4 ColNeonGreen            = ImVec4(0.000f, 0.902f, 0.463f, 1.0f); // #00e676 (Active Green)
    inline const ImVec4 ColNeonRed              = ImVec4(0.961f, 0.235f, 0.369f, 1.0f); // #f53c5e (Alert Crimson)

    // Backward compatibility aliases
    inline const ImVec4 ColDarkBg       = ColMidnightAbyss;
    inline const ImVec4 ColFrameBg      = ColObsidianCard;
    inline const ImVec4 ColFrameBgHover = ColObsidianCardHover;
    inline const ImVec4 ColFrameBgAct   = ImVec4(0.235f, 0.149f, 0.392f, 1.00f);
    inline const ImVec4 ColBorder       = ColElectricPurple;
    inline const ImVec4 ColBorderGlow   = ColNeonMagentaBright;
    inline const ImVec4 ColText         = ColPureWhite;
    inline const ImVec4 ColNeonViolet   = ColElectricPurpleBright;
    inline const ImVec4 ColNeonCyan     = ImVec4(0.024f, 0.776f, 0.898f, 1.00f);

    // U32 Color Constants for ImDrawList
    inline const ImU32 U32MidnightAbyss        = IM_COL32(10, 7, 20, 255);
    inline const ImU32 U32ObsidianCard          = IM_COL32(20, 13, 36, 255);
    inline const ImU32 U32ObsidianCardHover     = IM_COL32(32, 21, 56, 255);
    inline const ImU32 U32ObsidianInput         = IM_COL32(18, 10, 34, 255);
    inline const ImU32 U32ElectricPurple        = IM_COL32(138, 43, 226, 255);
    inline const ImU32 U32ElectricPurpleBright  = IM_COL32(168, 85, 247, 255);
    inline const ImU32 U32NeonMagenta           = IM_COL32(224, 36, 195, 255);
    inline const ImU32 U32NeonMagentaBright     = IM_COL32(217, 70, 239, 255);
    inline const ImU32 U32YellowHighlight       = IM_COL32(255, 230, 0, 255);
    inline const ImU32 U32PureWhite             = IM_COL32(255, 255, 255, 255);
    inline const ImU32 U32ComicBlack            = IM_COL32(0, 0, 0, 255);
    inline const ImU32 U32TextMuted             = IM_COL32(154, 130, 190, 255);
    inline const ImU32 U32BorderDim             = IM_COL32(58, 32, 90, 255);
    inline const ImU32 U32NeonGreen             = IM_COL32(0, 230, 118, 255);
    inline const ImU32 U32NeonRed               = IM_COL32(245, 60, 94, 255);

    // Font Helpers for ImGui 1.92+ Compatibility
    inline float GetFontPixelSize(ImFont* font) {
        if (font && font->LegacySize > 0.0f) return font->LegacySize;
        return ImGui::GetFontSize();
    }

    inline void DrawTextWithFont(ImDrawList* dl, ImFont* font, float fontSize, const ImVec2& pos, ImU32 col, const char* text) {
        if (!dl || !text) return;
        if (font) {
            float sz = (fontSize > 0.0f) ? fontSize : GetFontPixelSize(font);
            dl->AddText(font, sz, pos, col, text);
        } else {
            dl->AddText(pos, col, text);
        }
    }

    inline ImVec2 CalcTextSizeWithFont(ImFont* font, float fontSize, const char* text) {
        if (!text) return ImVec2(0.0f, 0.0f);
        if (font) {
            float sz = (fontSize > 0.0f) ? fontSize : GetFontPixelSize(font);
            return font->CalcTextSizeA(sz, FLT_MAX, 0.0f, text);
        }
        return ImGui::CalcTextSize(text);
    }

    // Dynamic Screen Shake System
    class ScreenShake {
    public:
        void Trigger(float duration = 0.35f, float intensity = 8.0f) {
            m_duration = duration;
            m_timeRemaining = duration;
            m_intensity = intensity;
            m_elapsed = 0.0f;
        }

        ImVec2 Update(float dt) {
            if (m_timeRemaining <= 0.0f) return ImVec2(0.0f, 0.0f);
            m_timeRemaining -= dt;
            m_elapsed += dt;
            if (m_timeRemaining < 0.0f) m_timeRemaining = 0.0f;

            float progress = m_duration > 0.0f ? (m_timeRemaining / m_duration) : 0.0f;
            float decay = progress * progress; // Quadratic decay for punchy Atlus shudder
            float dx = m_intensity * decay * sinf(m_elapsed * 55.0f);
            float dy = m_intensity * decay * cosf(m_elapsed * 42.0f) * 0.6f;
            return ImVec2(dx, dy);
        }

        bool IsShaking() const { return m_timeRemaining > 0.0f; }

    private:
        float m_duration = 0.0f;
        float m_timeRemaining = 0.0f;
        float m_intensity = 0.0f;
        float m_elapsed = 0.0f;
    };

    // Style initialization
    void ApplyTheme();

    // Canvas Background with Halftone Dot Matrix and Diagonal Comic Slashes
    void DrawHalftoneBackdrop(ImVec2 canvasPos, ImVec2 canvasSize, ImVec2 offset = ImVec2(0.0f, 0.0f));

    // Skewed Card (-4.5 degrees) with 3px solid black borders & 6px solid black drop shadows
    void DrawSkewCard(ImVec2 minPos, ImVec2 maxPos, ImU32 fillCol = U32ObsidianCard,
                      ImU32 borderCol = U32ComicBlack, float skewAngleDeg = -4.5f,
                      float borderWidth = 3.0f, float shadowOffset = 6.0f,
                      ImU32 accentCol = U32ElectricPurple);

    // Tilted Ransom-Note Badge (-3deg, +2deg, -2deg) on contrasting tiles with 3px drop shadows
    void DrawRansomBadge(const char* text, ImVec2 center, ImVec2 size, float angleDeg,
                         ImU32 bgCol, ImU32 textCol, ImFont* font = nullptr, float fontSize = 0.0f);

    // Top Calling Card Banner with [DEADLOCK] [MACRO] [EMIL] ransom titles
    void RenderCallingCardHeader(const char* title1, const char* title2, const char* title3,
                                 const char* subtitle, bool isMasterActive, bool sfxActive,
                                 bool* outSfxClicked, ImFont* fontRansom = nullptr,
                                 ImFont* fontSub = nullptr, ImFont* fontTag = nullptr);

    // Slanted Polygon Action Button
    bool AtlusButton(const char* label, const ImVec2& size, ImU32 fillCol, ImU32 textCol,
                     ImFont* font = nullptr, float slant = 10.0f, bool chamfer = true,
                     bool isSelected = false);

    // Custom Comic Slider with Diamond Thumb & Active Track Fill
    bool AtlusSliderInt(const char* id, const char* label, int* v, int vMin, int vMax,
                        const char* format, ImFont* labelFont = nullptr, ImFont* valFont = nullptr,
                        float trackHeight = 16.0f);

    // Tilted Header Tag
    void AtlusTag(const char* text, float angleDeg, ImU32 bgCol, ImU32 textCol, ImFont* font = nullptr);

    // Slanted Key Tile Visualizer Step
    void AtlusKeyStep(const char* label, int index, bool isCurrent, float width = 64.0f,
                      float height = 50.0f, ImFont* font = nullptr, ImFont* subFont = nullptr);

    // Atlus Checkbox (Chamfered comic toggle with drop shadow & magenta fill)
    bool AtlusCheckbox(const char* label, bool* v, ImFont* font = nullptr);

    // Atlus Radio Button (Slanted selector button with active indicator)
    bool AtlusRadioButton(const char* label, bool active, ImFont* font = nullptr);

    // Section Divider
    void RenderSectionDivider(const char* label, ImFont* font = nullptr);
    void RenderStatusBadge(const char* title, const char* status, bool isActive, const ImVec4& color,
                           float width = 186.0f, float height = 48.0f,
                           ImFont* titleFont = nullptr, ImFont* statusFont = nullptr);
}
