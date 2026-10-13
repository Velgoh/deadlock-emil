#include "AtlusTheme.h"
#include "imgui_internal.h"
#include <cstdio>
#include <cmath>
#include <algorithm>

namespace AtlusTheme {

void ApplyTheme() {
    ImGuiStyle& style = ImGui::GetStyle();

    // Atlus Razor-Sharp Angular Architecture (Zero Rounding Everywhere)
    style.WindowRounding    = 0.0f;
    style.ChildRounding     = 0.0f;
    style.FrameRounding     = 0.0f;
    style.PopupRounding     = 0.0f;
    style.ScrollbarRounding = 0.0f;
    style.GrabRounding      = 0.0f;
    style.TabRounding       = 0.0f;

    // Solid Atlus Borders
    style.WindowBorderSize  = 2.0f;
    style.ChildBorderSize   = 2.0f;
    style.FrameBorderSize   = 2.0f;
    style.PopupBorderSize   = 2.0f;
    style.TabBorderSize     = 2.0f;

    // Spacious Layout
    style.WindowPadding     = ImVec2(16.0f, 16.0f);
    style.FramePadding      = ImVec2(12.0f, 8.0f);
    style.ItemSpacing       = ImVec2(12.0f, 10.0f);
    style.ItemInnerSpacing  = ImVec2(8.0f, 6.0f);
    style.ScrollbarSize     = 14.0f;
    style.GrabMinSize       = 14.0f;

    // Atlus Dark Purple Color Mapping
    ImVec4* colors = style.Colors;
    colors[ImGuiCol_Text]                  = ColPureWhite;
    colors[ImGuiCol_TextDisabled]          = ColTextMuted;
    colors[ImGuiCol_WindowBg]              = ColMidnightAbyss;
    colors[ImGuiCol_ChildBg]               = ColObsidianCard;
    colors[ImGuiCol_PopupBg]               = ImVec4(0.08f, 0.05f, 0.15f, 0.98f);
    colors[ImGuiCol_Border]                = ColElectricPurple;
    colors[ImGuiCol_BorderShadow]          = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_FrameBg]               = ColObsidianInput;
    colors[ImGuiCol_FrameBgHovered]        = ColObsidianCardHover;
    colors[ImGuiCol_FrameBgActive]         = ImVec4(0.24f, 0.15f, 0.39f, 1.0f);
    colors[ImGuiCol_TitleBg]               = ColMidnightAbyss;
    colors[ImGuiCol_TitleBgActive]         = ColObsidianCard;
    colors[ImGuiCol_TitleBgCollapsed]      = ColMidnightAbyss;
    colors[ImGuiCol_MenuBarBg]             = ColObsidianCard;
    colors[ImGuiCol_ScrollbarBg]           = ImVec4(0.06f, 0.04f, 0.11f, 0.85f);
    colors[ImGuiCol_ScrollbarGrab]         = ColElectricPurple;
    colors[ImGuiCol_ScrollbarGrabHovered]  = ColElectricPurpleBright;
    colors[ImGuiCol_ScrollbarGrabActive]   = ColNeonMagenta;
    colors[ImGuiCol_CheckMark]             = ColNeonMagenta;
    colors[ImGuiCol_SliderGrab]            = ColYellowHighlight;
    colors[ImGuiCol_SliderGrabActive]      = ColPureWhite;
    colors[ImGuiCol_Button]                = ImVec4(0.18f, 0.12f, 0.32f, 1.00f);
    colors[ImGuiCol_ButtonHovered]         = ImVec4(0.28f, 0.18f, 0.48f, 1.00f);
    colors[ImGuiCol_ButtonActive]          = ColNeonMagenta;
    colors[ImGuiCol_Header]                = ImVec4(0.20f, 0.13f, 0.36f, 1.00f);
    colors[ImGuiCol_HeaderHovered]         = ImVec4(0.29f, 0.19f, 0.50f, 1.00f);
    colors[ImGuiCol_HeaderActive]          = ColNeonMagenta;
    colors[ImGuiCol_Separator]             = ColBorderDim;
    colors[ImGuiCol_SeparatorHovered]      = ColElectricPurple;
    colors[ImGuiCol_SeparatorActive]       = ColNeonMagenta;
    colors[ImGuiCol_ResizeGrip]            = ImVec4(0.35f, 0.20f, 0.55f, 0.50f);
    colors[ImGuiCol_ResizeGripHovered]     = ColElectricPurpleBright;
    colors[ImGuiCol_ResizeGripActive]      = ColNeonMagenta;
    colors[ImGuiCol_Tab]                   = ImVec4(0.14f, 0.09f, 0.24f, 1.00f);
    colors[ImGuiCol_TabHovered]            = ImVec4(0.26f, 0.17f, 0.44f, 1.00f);
    colors[ImGuiCol_TabActive]             = ImVec4(0.22f, 0.14f, 0.38f, 1.00f);
    colors[ImGuiCol_TabUnfocused]          = ImVec4(0.10f, 0.07f, 0.18f, 1.00f);
    colors[ImGuiCol_TabUnfocusedActive]    = ImVec4(0.16f, 0.11f, 0.28f, 1.00f);
    colors[ImGuiCol_PlotLines]             = ColNeonMagenta;
    colors[ImGuiCol_PlotLinesHovered]      = ColElectricPurpleBright;
    colors[ImGuiCol_PlotHistogram]         = ColNeonMagenta;
    colors[ImGuiCol_PlotHistogramHovered]  = ColElectricPurpleBright;
    colors[ImGuiCol_TableHeaderBg]         = ImVec4(0.16f, 0.11f, 0.28f, 1.00f);
    colors[ImGuiCol_TableBorderStrong]     = ColElectricPurple;
    colors[ImGuiCol_TableBorderLight]      = ColBorderDim;
    colors[ImGuiCol_TableRowBg]            = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_TableRowBgAlt]         = ImVec4(1.00f, 1.00f, 1.00f, 0.03f);
    colors[ImGuiCol_TextSelectedBg]        = ImVec4(0.89f, 0.18f, 0.94f, 0.35f);
    colors[ImGuiCol_DragDropTarget]        = ColNeonMagenta;
    colors[ImGuiCol_NavHighlight]          = ColNeonMagenta;
}

void DrawHalftoneBackdrop(ImVec2 canvasPos, ImVec2 canvasSize, ImVec2 offset) {
    ImDrawList* drawList = ImGui::GetBackgroundDrawList();
    if (!drawList) return;

    // Shift canvas by offset (screen shake) and expand margin so shake never exposes unrendered borders
    float pad = 24.0f;
    ImVec2 origin(canvasPos.x - pad + offset.x, canvasPos.y - pad + offset.y);
    ImVec2 extent(canvasSize.x + pad * 2.0f, canvasSize.y + pad * 2.0f);

    // 1. Deep Midnight Abyss Base Fill (#0a0714)
    drawList->AddRectFilled(origin, ImVec2(origin.x + extent.x, origin.y + extent.y), U32MidnightAbyss);

    // 2. Large dynamic diagonal comic slash ribbons
    // Big diagonal purple slash beam
    ImVec2 slashPoly1[3] = {
        ImVec2(origin.x, origin.y),
        ImVec2(origin.x + extent.x * 0.52f, origin.y),
        ImVec2(origin.x, origin.y + extent.y * 0.65f)
    };
    drawList->AddConvexPolyFilled(slashPoly1, 3, IM_COL32(138, 43, 226, 28));

    // Magenta edge slash line
    drawList->AddLine(
        ImVec2(origin.x + extent.x * 0.52f, origin.y),
        ImVec2(origin.x, origin.y + extent.y * 0.65f),
        IM_COL32(224, 36, 195, 80), 4.0f
    );

    // Secondary subtle lower-right comic slash
    ImVec2 slashPoly2[3] = {
        ImVec2(origin.x + extent.x, origin.y + extent.y * 0.30f),
        ImVec2(origin.x + extent.x, origin.y + extent.y),
        ImVec2(origin.x + extent.x * 0.45f, origin.y + extent.y)
    };
    drawList->AddConvexPolyFilled(slashPoly2, 3, IM_COL32(168, 85, 247, 20));

    // 3. Diagonal comic strip pattern (-45 degrees)
    float step = 32.0f;
    float stripeW = 12.0f;
    for (float d = -extent.y; d < extent.x + extent.y; d += step) {
        ImVec2 p0(origin.x + d, origin.y);
        ImVec2 p1(origin.x + d - extent.y, origin.y + extent.y);
        drawList->AddLine(p0, p1, IM_COL32(0, 0, 0, 75), stripeW);
    }

    // 4. Comic halftone dot matrix
    float dotSpacing = 18.0f;
    float maxDist = sqrtf(extent.x * extent.x + extent.y * extent.y);
    for (float y = origin.y + 8.0f; y < origin.y + extent.y; y += dotSpacing) {
        for (float x = origin.x + 8.0f; x < origin.x + extent.x; x += dotSpacing) {
            float dx = x - origin.x;
            float dy = y - origin.y;
            float dist = sqrtf(dx * dx + dy * dy);
            float t = 1.0f - (dist / maxDist);
            if (t < 0.0f) t = 0.0f;
            float r = 1.0f + t * 1.5f;
            int alpha = (int)(18 + t * 30);
            drawList->AddRectFilled(ImVec2(x - r, y - r), ImVec2(x + r, y + r), IM_COL32(210, 180, 255, alpha));
        }
    }
}

void DrawSkewCard(ImVec2 minPos, ImVec2 maxPos, ImU32 fillCol, ImU32 borderCol,
                  float skewAngleDeg, float borderWidth, float shadowOffset, ImU32 accentCol) {
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    if (!drawList) return;

    float height = maxPos.y - minPos.y;
    float rad = skewAngleDeg * (3.14159265f / 180.0f);
    float slant = height * fabsf(tanf(rad));

    // Strictly bound the parallelogram within [minPos, maxPos] taking shadowOffset into account
    ImVec2 poly[4] = {
        ImVec2(minPos.x + slant, minPos.y),
        ImVec2(maxPos.x - shadowOffset, minPos.y),
        ImVec2(maxPos.x - slant - shadowOffset, maxPos.y - shadowOffset),
        ImVec2(minPos.x, maxPos.y - shadowOffset)
    };

    // 1. 6px Hard Solid Black Drop Shadow
    ImVec2 shadowPoly[4] = {
        ImVec2(poly[0].x + shadowOffset, poly[0].y + shadowOffset),
        ImVec2(poly[1].x + shadowOffset, poly[1].y + shadowOffset),
        ImVec2(poly[2].x + shadowOffset, poly[2].y + shadowOffset),
        ImVec2(poly[3].x + shadowOffset, poly[3].y + shadowOffset)
    };
    drawList->AddConvexPolyFilled(shadowPoly, 4, U32ComicBlack);

    // 2. Card Body Fill (Obsidian Violet)
    drawList->AddConvexPolyFilled(poly, 4, fillCol);

    // 3. 3px Solid Black Outer Border
    drawList->AddPolyline(poly, 4, borderCol, ImDrawFlags_Closed, borderWidth);

    // 4. Atlus Accent Ribbon on Left Slanted Edge
    if (accentCol != 0) {
        float ribbonW = 7.0f;
        ImVec2 ribbonPoly[4] = {
            ImVec2(poly[0].x, poly[0].y),
            ImVec2(poly[0].x + ribbonW, poly[0].y),
            ImVec2(poly[3].x + ribbonW, poly[3].y),
            ImVec2(poly[3].x, poly[3].y)
        };
        drawList->AddConvexPolyFilled(ribbonPoly, 4, accentCol);
        drawList->AddPolyline(ribbonPoly, 4, U32ComicBlack, ImDrawFlags_Closed, 2.0f);
    }
}

void DrawRansomBadge(const char* text, ImVec2 center, ImVec2 size, float angleDeg,
                     ImU32 bgCol, ImU32 textCol, ImFont* font, float fontSize) {
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    if (!drawList) return;

    float rad = angleDeg * (3.14159265f / 180.0f);
    float cosA = cosf(rad);
    float sinA = sinf(rad);

    float hw = size.x * 0.5f;
    float hh = size.y * 0.5f;

    ImVec2 corners[4] = {
        ImVec2(-hw, -hh),
        ImVec2(hw, -hh),
        ImVec2(hw, hh),
        ImVec2(-hw, hh)
    };

    ImVec2 rot[4];
    for (int i = 0; i < 4; ++i) {
        rot[i].x = center.x + corners[i].x * cosA - corners[i].y * sinA;
        rot[i].y = center.y + corners[i].x * sinA + corners[i].y * cosA;
    }

    // 3px solid black hard drop shadow
    ImVec2 shadow[4];
    for (int i = 0; i < 4; ++i) {
        shadow[i] = ImVec2(rot[i].x + 3.0f, rot[i].y + 3.0f);
    }
    drawList->AddConvexPolyFilled(shadow, 4, U32ComicBlack);

    // Tile fill
    drawList->AddConvexPolyFilled(rot, 4, bgCol);

    // 2.5px solid black border
    drawList->AddPolyline(rot, 4, U32ComicBlack, ImDrawFlags_Closed, 2.5f);

    // Centered text
    float actualFontSize = (fontSize > 0.0f) ? fontSize : GetFontPixelSize(font);
    ImVec2 textSize = CalcTextSizeWithFont(font, actualFontSize, text);
    float textX = center.x - textSize.x * 0.5f;
    float textY = center.y - textSize.y * 0.5f;

    DrawTextWithFont(drawList, font, actualFontSize, ImVec2(textX, textY), textCol, text);
}

void RenderCallingCardHeader(const char* title1, const char* title2, const char* title3,
                             const char* subtitle, bool isMasterActive, bool sfxActive,
                             bool* outSfxClicked, ImFont* fontRansom, ImFont* fontSub,
                             ImFont* fontTag) {
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    ImVec2 pos = ImGui::GetCursorScreenPos();
    float width = ImGui::GetContentRegionAvail().x;
    float bannerHeight = 60.0f;

    // Draw the skewed calling card background
    DrawSkewCard(pos, ImVec2(pos.x + width - 16.0f, pos.y + bannerHeight),
                 U32ObsidianCard, U32ComicBlack, -4.5f, 3.0f, 6.0f, U32NeonMagenta);

    float slant = bannerHeight * 0.0787f;
    float leftX = pos.x + slant + 20.0f;
    float centerY = pos.y + bannerHeight * 0.5f;

    float fSize = GetFontPixelSize(fontRansom);

    ImVec2 s1 = CalcTextSizeWithFont(fontRansom, fSize, title1);
    ImVec2 s2 = CalcTextSizeWithFont(fontRansom, fSize, title2);
    ImVec2 s3 = (title3 && strlen(title3) > 0) ? CalcTextSizeWithFont(fontRansom, fSize, title3) : ImVec2(0.0f, 0.0f);

    float padX = 12.0f;
    float badgeH = bannerHeight - 20.0f;

    // Badge 1 -> -3.0 deg, Solid Black Tile, White Text
    float w1 = s1.x + padX * 2.0f;
    DrawRansomBadge(title1, ImVec2(leftX + w1 * 0.5f, centerY), ImVec2(w1, badgeH), -3.0f,
                    U32ComicBlack, U32PureWhite, fontRansom, fSize);

    // Badge 2 -> +2.0 deg, Neon Magenta Tile, White Text
    float left2 = leftX + w1 + 8.0f;
    float w2 = s2.x + padX * 2.0f;
    DrawRansomBadge(title2, ImVec2(left2 + w2 * 0.5f, centerY), ImVec2(w2, badgeH), 2.0f,
                    U32NeonMagenta, U32PureWhite, fontRansom, fSize);

    float nextLeft = left2 + w2 + 8.0f;
    if (title3 && strlen(title3) > 0) {
        float w3 = s3.x + padX * 2.0f;
        DrawRansomBadge(title3, ImVec2(nextLeft + w3 * 0.5f, centerY), ImVec2(w3, badgeH), -2.0f,
                        U32PureWhite, U32ComicBlack, fontRansom, fSize);
        nextLeft += w3 + 8.0f;
    }

    // Subtitle in Yellow with drop shadow
    if (subtitle) {
        float subLeft = nextLeft + 8.0f;
        float subY = centerY - 8.0f;
        DrawTextWithFont(drawList, fontSub, 0.0f, ImVec2(subLeft + 1.5f, subY + 1.5f), U32ComicBlack, subtitle);
        DrawTextWithFont(drawList, fontSub, 0.0f, ImVec2(subLeft, subY), U32YellowHighlight, subtitle);
    }

    // Right-side items: SFX toggle button & Master status badge
    float rightX = pos.x + width - 36.0f;

    // SFX Button
    float sfxW = 86.0f;
    float sfxH = 32.0f;
    float sfxX = rightX - sfxW;
    float sfxY = centerY - sfxH * 0.5f;

    ImGui::SetCursorScreenPos(ImVec2(sfxX, sfxY));
    char sfxLabel[32];
    snprintf(sfxLabel, sizeof(sfxLabel), "SFX: %s", sfxActive ? "ON" : "OFF");
    ImU32 sfxCol = sfxActive ? U32YellowHighlight : U32ObsidianInput;
    ImU32 sfxText = sfxActive ? U32ComicBlack : U32TextMuted;
    if (AtlusButton(sfxLabel, ImVec2(sfxW, sfxH), sfxCol, sfxText, fontTag, 6.0f, true)) {
        if (outSfxClicked) *outSfxClicked = true;
    }

    // Master Status Pill
    float statW = 150.0f;
    float statX = sfxX - statW - 10.0f;
    const char* statText = isMasterActive ? "MASTER: ON" : "MASTER: OFF";
    ImU32 statBg = isMasterActive ? U32NeonMagenta : U32ComicBlack;
    ImU32 statTextCol = isMasterActive ? U32PureWhite : U32NeonRed;
    DrawRansomBadge(statText, ImVec2(statX + statW * 0.5f, centerY), ImVec2(statW, sfxH), 0.0f,
                    statBg, statTextCol, fontTag, GetFontPixelSize(fontTag));

    ImGui::SetCursorScreenPos(ImVec2(pos.x, pos.y + bannerHeight + 10.0f));
}

bool AtlusButton(const char* label, const ImVec2& size, ImU32 fillCol, ImU32 textCol,
                 ImFont* font, float slant, bool chamfer, bool isSelected) {
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems) return false;

    ImGuiContext& g = *GImGui;
    const ImGuiID id = window->GetID(label);
    ImVec2 pos = window->DC.CursorPos;
    ImVec2 btnSize = size;
    if (btnSize.x <= 0.0f) btnSize.x = ImGui::GetContentRegionAvail().x;
    if (btnSize.y <= 0.0f) btnSize.y = 38.0f;

    const ImRect bb(pos, ImVec2(pos.x + btnSize.x, pos.y + btnSize.y));
    ImGui::ItemSize(bb);
    if (!ImGui::ItemAdd(bb, id)) return false;

    bool hovered, held;
    bool pressed = ImGui::ButtonBehavior(bb, id, &hovered, &held);

    ImDrawList* drawList = window->DrawList;

    float offset = held ? 2.0f : (hovered ? -2.0f : (isSelected ? -1.0f : 0.0f));
    float shadowOffset = held ? 2.0f : (hovered ? 7.0f : (isSelected ? 6.0f : 4.0f));

    float x = pos.x + offset;
    float y = pos.y + offset;
    float w = btnSize.x;
    float h = btnSize.y;

    if (chamfer) {
        float c = 9.0f;
        ImVec2 verts[6] = {
            ImVec2(x + c, y),
            ImVec2(x + w, y),
            ImVec2(x + w, y + h - c),
            ImVec2(x + w - c, y + h),
            ImVec2(x, y + h),
            ImVec2(x, y + c)
        };

        // Drop shadow
        ImVec2 sVerts[6];
        for (int i = 0; i < 6; ++i) sVerts[i] = ImVec2(verts[i].x + shadowOffset, verts[i].y + shadowOffset);
        drawList->AddConvexPolyFilled(sVerts, 6, U32ComicBlack);

        // Fill
        ImU32 activeFill = isSelected ? U32NeonMagenta : fillCol;
        if (hovered && !held) {
            activeFill = ImGui::ColorConvertFloat4ToU32(ImVec4(
                (std::min)(1.0f, ImGui::ColorConvertU32ToFloat4(activeFill).x + 0.12f),
                (std::min)(1.0f, ImGui::ColorConvertU32ToFloat4(activeFill).y + 0.12f),
                (std::min)(1.0f, ImGui::ColorConvertU32ToFloat4(activeFill).z + 0.12f),
                1.0f
            ));
        }
        drawList->AddConvexPolyFilled(verts, 6, activeFill);

        // 3px solid black border
        drawList->AddPolyline(verts, 6, U32ComicBlack, ImDrawFlags_Closed, 3.0f);

        // Yellow accent on top-left chamfer
        drawList->AddLine(verts[5], verts[0], U32YellowHighlight, 3.0f);
    } else {
        ImVec2 verts[4] = {
            ImVec2(x + slant, y),
            ImVec2(x + w, y),
            ImVec2(x + w - slant, y + h),
            ImVec2(x, y + h)
        };

        // Shadow
        ImVec2 sVerts[4];
        for (int i = 0; i < 4; ++i) sVerts[i] = ImVec2(verts[i].x + shadowOffset, verts[i].y + shadowOffset);
        drawList->AddConvexPolyFilled(sVerts, 4, U32ComicBlack);

        // Fill
        ImU32 activeFill = isSelected ? U32NeonMagenta : (hovered ? U32NeonMagentaBright : fillCol);
        drawList->AddConvexPolyFilled(verts, 4, activeFill);

        // 3px solid black border
        drawList->AddPolyline(verts, 4, U32ComicBlack, ImDrawFlags_Closed, 3.0f);

        // Bottom yellow accent underline if active tab/selected
        if (isSelected) {
            drawList->AddLine(ImVec2(verts[3].x + 4.0f, verts[3].y - 3.0f),
                              ImVec2(verts[2].x - 4.0f, verts[2].y - 3.0f),
                              U32YellowHighlight, 3.5f);
        }
    }

    // Centered label text
    float fSize = GetFontPixelSize(font);
    ImVec2 textSize = CalcTextSizeWithFont(font, fSize, label);
    float tx = x + (w - textSize.x) * 0.5f;
    float ty = y + (h - textSize.y) * 0.5f;

    DrawTextWithFont(drawList, font, fSize, ImVec2(tx + 1.5f, ty + 1.5f), U32ComicBlack, label);
    DrawTextWithFont(drawList, font, fSize, ImVec2(tx, ty), isSelected ? U32PureWhite : textCol, label);

    return pressed;
}

bool AtlusSliderInt(const char* id, const char* label, int* v, int vMin, int vMax,
                    const char* format, ImFont* labelFont, ImFont* valFont, float trackHeight) {
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems) return false;

    ImGuiContext& g = *GImGui;
    ImDrawList* drawList = window->DrawList;
    float availW = ImGui::GetContentRegionAvail().x;

    ImVec2 curPos = window->DC.CursorPos;

    // 1. Label on left
    DrawTextWithFont(drawList, labelFont, 0.0f, curPos, U32PureWhite, label);

    // Format value and draw badge on right
    char valStr[64];
    snprintf(valStr, sizeof(valStr), format ? format : "%d", *v);
    ImVec2 valTextSize = CalcTextSizeWithFont(valFont, 0.0f, valStr);

    float badgePadX = 10.0f;
    float badgePadY = 3.0f;
    float badgeW = valTextSize.x + badgePadX * 2.0f;
    float badgeH = valTextSize.y + badgePadY * 2.0f;
    ImVec2 badgePos(curPos.x + availW - badgeW - 6.0f, curPos.y - 2.0f);

    drawList->AddRectFilled(ImVec2(badgePos.x + 2.0f, badgePos.y + 2.0f),
                            ImVec2(badgePos.x + badgeW + 2.0f, badgePos.y + badgeH + 2.0f), U32ComicBlack);
    drawList->AddRectFilled(badgePos, ImVec2(badgePos.x + badgeW, badgePos.y + badgeH), U32ObsidianInput);
    drawList->AddRect(badgePos, ImVec2(badgePos.x + badgeW, badgePos.y + badgeH), U32ElectricPurple, 0.0f, 0, 1.5f);
    DrawTextWithFont(drawList, valFont, 0.0f, ImVec2(badgePos.x + badgePadX, badgePos.y + badgePadY),
                     U32YellowHighlight, valStr);

    // Track position
    float headerH = (std::max)(GetFontPixelSize(labelFont), badgeH) + 6.0f;
    ImVec2 trackPos(curPos.x, curPos.y + headerH);

    // 2. Interactive Area
    float totalH = trackHeight + 10.0f;
    const ImGuiID widgetId = window->GetID(id);
    const ImRect bb(trackPos, ImVec2(trackPos.x + availW, trackPos.y + totalH));
    ImGui::ItemSize(bb);
    if (!ImGui::ItemAdd(bb, widgetId)) return false;

    bool hovered, held;
    bool valueChanged = false;
    ImGui::ButtonBehavior(bb, widgetId, &hovered, &held);

    float thumbRadius = 9.0f;
    float trackActualY = trackPos.y + (totalH - trackHeight) * 0.5f;
    float trackInnerW = availW - thumbRadius * 2.0f;

    if (held) {
        float mouseX = g.IO.MousePos.x;
        float fraction = (mouseX - (trackPos.x + thumbRadius)) / (trackInnerW > 0.0f ? trackInnerW : 1.0f);
        if (fraction < 0.0f) fraction = 0.0f;
        if (fraction > 1.0f) fraction = 1.0f;
        int newVal = vMin + (int)std::round(fraction * (float)(vMax - vMin));
        if (newVal != *v) {
            *v = newVal;
            valueChanged = true;
        }
    }

    float currentFrac = (vMax > vMin) ? ((float)(*v - vMin) / (float)(vMax - vMin)) : 0.0f;
    if (currentFrac < 0.0f) currentFrac = 0.0f;
    if (currentFrac > 1.0f) currentFrac = 1.0f;

    float thumbCX = trackPos.x + thumbRadius + currentFrac * trackInnerW;
    float thumbCY = trackActualY + trackHeight * 0.5f;

    // 3. Draw Track
    // Track 3px Solid Black Drop Shadow
    drawList->AddRectFilled(
        ImVec2(trackPos.x + 3.0f, trackActualY + 3.0f),
        ImVec2(trackPos.x + availW + 3.0f, trackActualY + trackHeight + 3.0f),
        U32ComicBlack
    );

    // Track Background (Obsidian Input)
    drawList->AddRectFilled(
        ImVec2(trackPos.x, trackActualY),
        ImVec2(trackPos.x + availW, trackActualY + trackHeight),
        U32ObsidianInput
    );

    // Active Filled Track (Neon Magenta)
    float activeW = thumbCX - trackPos.x;
    if (activeW > 0.0f) {
        drawList->AddRectFilled(
            ImVec2(trackPos.x, trackActualY),
            ImVec2(trackPos.x + activeW, trackActualY + trackHeight),
            U32NeonMagenta
        );
    }

    // 2px Solid Black Border around Track
    drawList->AddRect(
        ImVec2(trackPos.x, trackActualY),
        ImVec2(trackPos.x + availW, trackActualY + trackHeight),
        U32ComicBlack, 0.0f, 0, 2.0f
    );

    // 4. Diamond Thumb
    ImVec2 dVerts[4] = {
        ImVec2(thumbCX, thumbCY - thumbRadius),
        ImVec2(thumbCX + thumbRadius, thumbCY),
        ImVec2(thumbCX, thumbCY + thumbRadius),
        ImVec2(thumbCX - thumbRadius, thumbCY)
    };

    // Diamond 3px Drop Shadow
    ImVec2 dShadow[4];
    for (int i = 0; i < 4; ++i) dShadow[i] = ImVec2(dVerts[i].x + 3.0f, dVerts[i].y + 3.0f);
    drawList->AddConvexPolyFilled(dShadow, 4, U32ComicBlack);

    // Diamond Fill: Bright Yellow Highlight (White when held/hovered)
    ImU32 thumbFill = (held || hovered) ? U32PureWhite : U32YellowHighlight;
    drawList->AddConvexPolyFilled(dVerts, 4, thumbFill);

    // 2.5px Solid Black Border
    drawList->AddPolyline(dVerts, 4, U32ComicBlack, ImDrawFlags_Closed, 2.5f);

    // Inner Diamond Center Dot
    drawList->AddCircleFilled(ImVec2(thumbCX, thumbCY), 2.5f, U32ComicBlack);

    ImGui::Spacing();
    return valueChanged;
}

void AtlusTag(const char* text, float angleDeg, ImU32 bgCol, ImU32 textCol, ImFont* font) {
    ImVec2 s = CalcTextSizeWithFont(font, 0.0f, text);

    float padX = 12.0f;
    float padY = 5.0f;
    float w = s.x + padX * 2.0f;
    float h = s.y + padY * 2.0f;

    ImVec2 pos = ImGui::GetCursorScreenPos();
    DrawRansomBadge(text, ImVec2(pos.x + w * 0.5f, pos.y + h * 0.5f), ImVec2(w, h), angleDeg, bgCol, textCol, font, GetFontPixelSize(font));
    ImGui::Dummy(ImVec2(w + 6.0f, h + 4.0f));
}

void AtlusKeyStep(const char* label, int index, bool isCurrent, float width, float height, ImFont* font, ImFont* subFont) {
    ImVec2 pos = ImGui::GetCursorScreenPos();
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    float slant = 8.0f;

    // Slanted parallelogram key tile
    ImVec2 poly[4] = {
        ImVec2(pos.x + slant, pos.y),
        ImVec2(pos.x + width + slant, pos.y),
        ImVec2(pos.x + width, pos.y + height),
        ImVec2(pos.x, pos.y + height)
    };

    // 4px Drop Shadow
    ImVec2 shadow[4];
    for (int i = 0; i < 4; ++i) shadow[i] = ImVec2(poly[i].x + 4.0f, poly[i].y + 4.0f);
    drawList->AddConvexPolyFilled(shadow, 4, U32ComicBlack);

    // Fill
    ImU32 bgCol = isCurrent ? U32NeonMagenta : U32ObsidianInput;
    drawList->AddConvexPolyFilled(poly, 4, bgCol);

    // 2.5px Solid Black Border
    drawList->AddPolyline(poly, 4, U32ComicBlack, ImDrawFlags_Closed, 2.5f);

    // Key label text
    ImVec2 textSize = CalcTextSizeWithFont(font, 0.0f, label);
    float tx = pos.x + (width - textSize.x) * 0.5f + slant * 0.5f;
    float ty = pos.y + 6.0f;
    ImU32 textCol = isCurrent ? U32PureWhite : U32PureWhite;
    DrawTextWithFont(drawList, font, 0.0f, ImVec2(tx, ty), textCol, label);

    // Step index (#1, #2)
    char stepIndexStr[16];
    snprintf(stepIndexStr, sizeof(stepIndexStr), "#%d", index + 1);
    ImVec2 subSize = CalcTextSizeWithFont(subFont, 0.0f, stepIndexStr);
    float sx = pos.x + (width - subSize.x) * 0.5f + slant * 0.5f;
    float sy = pos.y + height - subSize.y - 4.0f;
    ImU32 subCol = isCurrent ? U32YellowHighlight : U32TextMuted;
    DrawTextWithFont(drawList, subFont, 0.0f, ImVec2(sx, sy), subCol, stepIndexStr);

    ImGui::Dummy(ImVec2(width + slant + 6.0f, height + 4.0f));
}

void RenderSectionDivider(const char* label, ImFont* font) {
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    ImVec2 pos = ImGui::GetCursorScreenPos();
    float width = ImGui::GetContentRegionAvail().x;
    float textWidth = CalcTextSizeWithFont(font, 0.0f, label).x;
    float h = 22.0f;
    float slant = 10.0f;

    float badgeW = textWidth + 30.0f;
    ImVec2 badgePoly[4] = {
        ImVec2(pos.x, pos.y),
        ImVec2(pos.x + badgeW, pos.y),
        ImVec2(pos.x + badgeW - slant, pos.y + h),
        ImVec2(pos.x, pos.y + h)
    };
    drawList->AddConvexPolyFilled(badgePoly, 4, ImGui::ColorConvertFloat4ToU32(ImVec4(0.20f, 0.12f, 0.35f, 0.95f)));
    drawList->AddPolyline(badgePoly, 4, U32ComicBlack, ImDrawFlags_Closed, 2.0f);

    // Left neon accent mark
    drawList->AddLine(ImVec2(pos.x + 2.0f, pos.y + 2.0f), ImVec2(pos.x + 2.0f, pos.y + h - 2.0f),
                      U32NeonMagenta, 3.0f);

    // Section title
    DrawTextWithFont(drawList, font, 0.0f, ImVec2(pos.x + 10.0f, pos.y + 2.0f), U32PureWhite, label);

    // Trailing line
    float lineStartX = pos.x + badgeW + 8.0f;
    if (lineStartX < pos.x + width) {
        drawList->AddLine(ImVec2(lineStartX, pos.y + h * 0.5f), ImVec2(pos.x + width, pos.y + h * 0.5f),
                          U32BorderDim, 1.5f);
        drawList->AddCircleFilled(ImVec2(pos.x + width - 3.0f, pos.y + h * 0.5f), 2.5f, U32ElectricPurpleBright);
    }

    ImGui::Dummy(ImVec2(width, h + 4.0f));
}

bool AtlusCheckbox(const char* label, bool* v, ImFont* font) {
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems) return false;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(label);

    ImVec2 labelSize = CalcTextSizeWithFont(font, 0.0f, label);
    float squareSz = 22.0f;
    ImVec2 pos = window->DC.CursorPos;
    float totalW = squareSz + style.ItemInnerSpacing.x + labelSize.x + 8.0f;
    const ImRect bb(pos, ImVec2(pos.x + totalW, pos.y + (std::max)(squareSz, labelSize.y) + 4.0f));
    ImGui::ItemSize(bb);
    if (!ImGui::ItemAdd(bb, id)) return false;

    bool hovered, held;
    bool pressed = ImGui::ButtonBehavior(bb, id, &hovered, &held);
    if (pressed) {
        *v = !(*v);
        ImGui::MarkItemEdited(id);
    }

    ImDrawList* drawList = window->DrawList;
    float boxX = pos.x;
    float boxY = pos.y + (bb.GetHeight() - squareSz) * 0.5f;

    // Chamfered box (22x22)
    float c = 5.0f;
    ImVec2 verts[6] = {
        ImVec2(boxX + c, boxY),
        ImVec2(boxX + squareSz, boxY),
        ImVec2(boxX + squareSz, boxY + squareSz - c),
        ImVec2(boxX + squareSz - c, boxY + squareSz),
        ImVec2(boxX, boxY + squareSz),
        ImVec2(boxX, boxY + c)
    };

    // 2.5px solid black drop shadow
    ImVec2 shadow[6];
    for (int i = 0; i < 6; ++i) shadow[i] = ImVec2(verts[i].x + 3.0f, verts[i].y + 3.0f);
    drawList->AddConvexPolyFilled(shadow, 6, U32ComicBlack);

    // Box Fill
    ImU32 fill = *v ? U32NeonMagenta : (hovered ? U32ObsidianCardHover : U32ObsidianInput);
    drawList->AddConvexPolyFilled(verts, 6, fill);
    drawList->AddPolyline(verts, 6, U32ComicBlack, ImDrawFlags_Closed, 2.0f);

    // If checked: Draw Atlus Yellow / White check glyph
    if (*v) {
        float cx = boxX + squareSz * 0.5f;
        float cy = boxY + squareSz * 0.5f;
        ImVec2 pCheck[3] = {
            ImVec2(cx - 5.0f, cy),
            ImVec2(cx - 1.0f, cy + 4.5f),
            ImVec2(cx + 6.0f, cy - 5.0f)
        };
        drawList->AddPolyline(pCheck, 3, U32YellowHighlight, ImDrawFlags_None, 3.0f);
        drawList->AddPolyline(pCheck, 3, U32PureWhite, ImDrawFlags_None, 1.5f);
    }

    // Label Text
    float tx = boxX + squareSz + style.ItemInnerSpacing.x + 4.0f;
    float ty = pos.y + (bb.GetHeight() - labelSize.y) * 0.5f;
    ImU32 textCol = *v ? U32PureWhite : (hovered ? U32PureWhite : U32TextMuted);
    DrawTextWithFont(drawList, font, 0.0f, ImVec2(tx, ty), textCol, label);

    return pressed;
}

bool AtlusRadioButton(const char* label, bool active, ImFont* font) {
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems) return false;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(label);

    ImVec2 labelSize = CalcTextSizeWithFont(font, 0.0f, label);
    float dotSz = 20.0f;
    ImVec2 pos = window->DC.CursorPos;
    float totalW = dotSz + style.ItemInnerSpacing.x + labelSize.x + 8.0f;
    const ImRect bb(pos, ImVec2(pos.x + totalW, pos.y + (std::max)(dotSz, labelSize.y) + 4.0f));
    ImGui::ItemSize(bb);
    if (!ImGui::ItemAdd(bb, id)) return false;

    bool hovered, held;
    bool pressed = ImGui::ButtonBehavior(bb, id, &hovered, &held);

    ImDrawList* drawList = window->DrawList;
    float dotCX = pos.x + dotSz * 0.5f;
    float dotCY = pos.y + bb.GetHeight() * 0.5f;
    float r = dotSz * 0.45f;

    // Diamond radio button (Atlus signature)
    ImVec2 dVerts[4] = {
        ImVec2(dotCX, dotCY - r),
        ImVec2(dotCX + r, dotCY),
        ImVec2(dotCX, dotCY + r),
        ImVec2(dotCX - r, dotCY)
    };

    ImVec2 dShadow[4];
    for (int i = 0; i < 4; ++i) dShadow[i] = ImVec2(dVerts[i].x + 2.5f, dVerts[i].y + 2.5f);
    drawList->AddConvexPolyFilled(dShadow, 4, U32ComicBlack);

    drawList->AddConvexPolyFilled(dVerts, 4, active ? U32NeonMagenta : U32ObsidianInput);
    drawList->AddPolyline(dVerts, 4, U32ComicBlack, ImDrawFlags_Closed, 2.0f);

    if (active) {
        float inR = r * 0.5f;
        ImVec2 inVerts[4] = {
            ImVec2(dotCX, dotCY - inR),
            ImVec2(dotCX + inR, dotCY),
            ImVec2(dotCX, dotCY + inR),
            ImVec2(dotCX - inR, dotCY)
        };
        drawList->AddConvexPolyFilled(inVerts, 4, U32YellowHighlight);
    }

    float tx = pos.x + dotSz + style.ItemInnerSpacing.x + 4.0f;
    float ty = pos.y + (bb.GetHeight() - labelSize.y) * 0.5f;
    DrawTextWithFont(drawList, font, 0.0f, ImVec2(tx, ty), active ? U32PureWhite : (hovered ? U32PureWhite : U32TextMuted), label);

    return pressed;
}

void RenderStatusBadge(const char* title, const char* status, bool isActive, const ImVec4& color,
                       float width, float height, ImFont* titleFont, ImFont* statusFont) {
    ImVec2 pos = ImGui::GetCursorScreenPos();
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    float boxW = width > 0.0f ? width : 186.0f;
    float boxH = height > 0.0f ? height : 48.0f;
    float chamfer = 8.0f;

    ImVec2 poly[6] = {
        ImVec2(pos.x, pos.y),
        ImVec2(pos.x + boxW - chamfer, pos.y),
        ImVec2(pos.x + boxW, pos.y + chamfer),
        ImVec2(pos.x + boxW, pos.y + boxH),
        ImVec2(pos.x + chamfer, pos.y + boxH),
        ImVec2(pos.x, pos.y + boxH - chamfer)
    };

    ImU32 bgCol = isActive ? IM_COL32(36, 20, 60, 255) : U32ObsidianInput;

    // 3px solid black drop shadow
    ImVec2 shadow[6];
    for (int i = 0; i < 6; ++i) shadow[i] = ImVec2(poly[i].x + 3.0f, poly[i].y + 3.0f);
    drawList->AddConvexPolyFilled(shadow, 6, U32ComicBlack);

    drawList->AddConvexPolyFilled(poly, 6, bgCol);
    drawList->AddPolyline(poly, 6, U32ComicBlack, ImDrawFlags_Closed, 2.5f);

    float indX = pos.x + 10.0f;
    float indY = pos.y + boxH * 0.5f - 8.0f;
    if (isActive) {
        ImVec2 arrow[3] = {
            ImVec2(indX, indY),
            ImVec2(indX + 8.0f, indY + 8.0f),
            ImVec2(indX, indY + 16.0f)
        };
        drawList->AddConvexPolyFilled(arrow, 3, ImGui::ColorConvertFloat4ToU32(color));
    } else {
        drawList->AddRectFilled(ImVec2(indX, indY + 2.0f), ImVec2(indX + 4.0f, indY + 14.0f), U32BorderDim);
    }

    DrawTextWithFont(drawList, titleFont, 0.0f, ImVec2(pos.x + 24.0f, pos.y + 6.0f), U32TextMuted, title);
    DrawTextWithFont(drawList, statusFont, 0.0f, ImVec2(pos.x + 24.0f, pos.y + boxH - 24.0f),
                     isActive ? ImGui::ColorConvertFloat4ToU32(color) : U32PureWhite, status);

    ImGui::Dummy(ImVec2(boxW, boxH));
}

} // namespace AtlusTheme
