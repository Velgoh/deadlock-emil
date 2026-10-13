#include "AtlusTheme.h"
#include <cstdio>
#include <algorithm>

namespace AtlusTheme {

void ApplyTheme() {
    ImGuiStyle& style = ImGui::GetStyle();

    // Atlus Razor-Sharp Angular Architecture (Zero Rounding)
    style.WindowRounding    = 0.0f;
    style.ChildRounding     = 0.0f;
    style.FrameRounding     = 0.0f;
    style.PopupRounding     = 0.0f;
    style.ScrollbarRounding = 0.0f;
    style.GrabRounding      = 0.0f;
    style.TabRounding       = 0.0f;

    // Crisp Atlus Borders
    style.WindowBorderSize  = 1.0f;
    style.ChildBorderSize   = 1.0f;
    style.FrameBorderSize   = 1.0f;
    style.PopupBorderSize   = 1.0f;
    style.TabBorderSize     = 1.0f;

    // Modern Spacious Layout
    style.WindowPadding     = ImVec2(16.0f, 16.0f);
    style.FramePadding      = ImVec2(12.0f, 8.0f);
    style.ItemSpacing       = ImVec2(12.0f, 10.0f);
    style.ItemInnerSpacing  = ImVec2(8.0f, 6.0f);
    style.ScrollbarSize     = 14.0f;
    style.GrabMinSize       = 14.0f;

    // Atlus Dark Purple Color Mapping
    ImVec4* colors = style.Colors;
    colors[ImGuiCol_Text]                  = ColText;
    colors[ImGuiCol_TextDisabled]          = ColTextMuted;
    colors[ImGuiCol_WindowBg]              = ColDarkBg;
    colors[ImGuiCol_ChildBg]               = ColFrameBg;
    colors[ImGuiCol_PopupBg]               = ImVec4(0.08f, 0.05f, 0.15f, 0.98f);
    colors[ImGuiCol_Border]                = ColBorder;
    colors[ImGuiCol_BorderShadow]          = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_FrameBg]               = ColFrameBg;
    colors[ImGuiCol_FrameBgHovered]        = ColFrameBgHover;
    colors[ImGuiCol_FrameBgActive]         = ColFrameBgAct;
    colors[ImGuiCol_TitleBg]               = ColDarkBg;
    colors[ImGuiCol_TitleBgActive]         = ColFrameBg;
    colors[ImGuiCol_TitleBgCollapsed]      = ColDarkBg;
    colors[ImGuiCol_MenuBarBg]             = ColFrameBg;
    colors[ImGuiCol_ScrollbarBg]           = ImVec4(0.06f, 0.04f, 0.11f, 0.80f);
    colors[ImGuiCol_ScrollbarGrab]         = ColBorder;
    colors[ImGuiCol_ScrollbarGrabHovered]  = ColNeonViolet;
    colors[ImGuiCol_ScrollbarGrabActive]   = ColNeonMagenta;
    colors[ImGuiCol_CheckMark]             = ColNeonMagenta;
    colors[ImGuiCol_SliderGrab]            = ColNeonMagenta;
    colors[ImGuiCol_SliderGrabActive]      = ColNeonViolet;
    colors[ImGuiCol_Button]                = ImVec4(0.18f, 0.12f, 0.32f, 1.00f);
    colors[ImGuiCol_ButtonHovered]         = ImVec4(0.28f, 0.18f, 0.48f, 1.00f);
    colors[ImGuiCol_ButtonActive]          = ColNeonMagenta;
    colors[ImGuiCol_Header]                = ImVec4(0.20f, 0.13f, 0.36f, 1.00f);
    colors[ImGuiCol_HeaderHovered]         = ImVec4(0.29f, 0.19f, 0.50f, 1.00f);
    colors[ImGuiCol_HeaderActive]          = ColNeonMagenta;
    colors[ImGuiCol_Separator]             = ColBorderDim;
    colors[ImGuiCol_SeparatorHovered]      = ColBorder;
    colors[ImGuiCol_SeparatorActive]       = ColNeonMagenta;
    colors[ImGuiCol_ResizeGrip]            = ImVec4(0.35f, 0.20f, 0.55f, 0.50f);
    colors[ImGuiCol_ResizeGripHovered]     = ColNeonViolet;
    colors[ImGuiCol_ResizeGripActive]      = ColNeonMagenta;
    colors[ImGuiCol_Tab]                   = ImVec4(0.14f, 0.09f, 0.24f, 1.00f);
    colors[ImGuiCol_TabHovered]            = ImVec4(0.26f, 0.17f, 0.44f, 1.00f);
    colors[ImGuiCol_TabActive]             = ImVec4(0.22f, 0.14f, 0.38f, 1.00f);
    colors[ImGuiCol_TabUnfocused]          = ImVec4(0.10f, 0.07f, 0.18f, 1.00f);
    colors[ImGuiCol_TabUnfocusedActive]    = ImVec4(0.16f, 0.11f, 0.28f, 1.00f);
    colors[ImGuiCol_PlotLines]             = ColNeonMagenta;
    colors[ImGuiCol_PlotLinesHovered]      = ColNeonViolet;
    colors[ImGuiCol_PlotHistogram]         = ColNeonMagenta;
    colors[ImGuiCol_PlotHistogramHovered]  = ColNeonViolet;
    colors[ImGuiCol_TableHeaderBg]         = ImVec4(0.16f, 0.11f, 0.28f, 1.00f);
    colors[ImGuiCol_TableBorderStrong]     = ColBorder;
    colors[ImGuiCol_TableBorderLight]      = ColBorderDim;
    colors[ImGuiCol_TableRowBg]            = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_TableRowBgAlt]         = ImVec4(1.00f, 1.00f, 1.00f, 0.03f);
    colors[ImGuiCol_TextSelectedBg]        = ImVec4(0.89f, 0.18f, 0.94f, 0.35f);
    colors[ImGuiCol_DragDropTarget]        = ColNeonMagenta;
    colors[ImGuiCol_NavHighlight]          = ColNeonMagenta;
}

void RenderHeaderBanner(const char* title, const char* subtitle) {
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    ImVec2 pos = ImGui::GetCursorScreenPos();
    float width = ImGui::GetContentRegionAvail().x;
    float bannerHeight = subtitle ? 54.0f : 38.0f;
    float slant = 14.0f;

    // Slanted parallelogram banner background
    ImVec2 bgPoly[4] = {
        ImVec2(pos.x, pos.y),
        ImVec2(pos.x + width, pos.y),
        ImVec2(pos.x + width - slant, pos.y + bannerHeight),
        ImVec2(pos.x, pos.y + bannerHeight)
    };
    drawList->AddConvexPolyFilled(bgPoly, 4, ImGui::ColorConvertFloat4ToU32(ImVec4(0.12f, 0.08f, 0.21f, 0.98f)));

    // Atlus neon magenta slash ribbon on left edge
    ImVec2 ribbonPoly[4] = {
        ImVec2(pos.x, pos.y),
        ImVec2(pos.x + 8.0f, pos.y),
        ImVec2(pos.x + 8.0f + slant * 0.4f, pos.y + bannerHeight),
        ImVec2(pos.x, pos.y + bannerHeight)
    };
    drawList->AddConvexPolyFilled(ribbonPoly, 4, ImGui::ColorConvertFloat4ToU32(ColNeonMagenta));

    // Outer slanted border
    drawList->AddPolyline(bgPoly, 4, ImGui::ColorConvertFloat4ToU32(ColBorder), ImDrawFlags_Closed, 1.2f);

    // Decorative Atlus diamond accent glyph
    drawList->AddText(ImVec2(pos.x + 18.0f, pos.y + 6.0f), ImGui::ColorConvertFloat4ToU32(ColNeonViolet), "◆");
    drawList->AddText(ImVec2(pos.x + 36.0f, pos.y + 6.0f), ImGui::ColorConvertFloat4ToU32(ColText), title);

    if (subtitle) {
        drawList->AddText(ImVec2(pos.x + 36.0f, pos.y + 30.0f), ImGui::ColorConvertFloat4ToU32(ColTextMuted), subtitle);
    }

    ImGui::Dummy(ImVec2(width, bannerHeight));
    ImGui::Spacing();
}

void RenderSectionDivider(const char* label) {
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    ImVec2 pos = ImGui::GetCursorScreenPos();
    float width = ImGui::GetContentRegionAvail().x;
    float textWidth = ImGui::CalcTextSize(label).x;
    float h = 22.0f;
    float slant = 10.0f;

    // Stylized slanted badge behind section title
    float badgeW = textWidth + 34.0f;
    ImVec2 badgePoly[4] = {
        ImVec2(pos.x, pos.y),
        ImVec2(pos.x + badgeW, pos.y),
        ImVec2(pos.x + badgeW - slant, pos.y + h),
        ImVec2(pos.x, pos.y + h)
    };
    drawList->AddConvexPolyFilled(badgePoly, 4, ImGui::ColorConvertFloat4ToU32(ImVec4(0.20f, 0.12f, 0.35f, 0.95f)));
    drawList->AddPolyline(badgePoly, 4, ImGui::ColorConvertFloat4ToU32(ColNeonViolet), ImDrawFlags_Closed, 1.0f);

    // Left neon accent mark
    drawList->AddLine(ImVec2(pos.x + 2.0f, pos.y + 2.0f), ImVec2(pos.x + 2.0f, pos.y + h - 2.0f),
        ImGui::ColorConvertFloat4ToU32(ColNeonMagenta), 3.0f);

    // Section title
    drawList->AddText(ImVec2(pos.x + 12.0f, pos.y + 2.0f), ImGui::ColorConvertFloat4ToU32(ColText), label);

    // Slanted trailing horizontal rule
    float lineStartX = pos.x + badgeW + 8.0f;
    if (lineStartX < pos.x + width) {
        drawList->AddLine(ImVec2(lineStartX, pos.y + h * 0.5f), ImVec2(pos.x + width, pos.y + h * 0.5f),
            ImGui::ColorConvertFloat4ToU32(ColBorderDim), 1.0f);
        // Small decorative diamond at the end
        drawList->AddCircleFilled(ImVec2(pos.x + width - 3.0f, pos.y + h * 0.5f), 2.5f,
            ImGui::ColorConvertFloat4ToU32(ColNeonViolet));
    }

    ImGui::Dummy(ImVec2(width, h + 4.0f));
}

void RenderStatusBadge(const char* title, const char* status, bool isActive, const ImVec4& color) {
    ImVec2 pos = ImGui::GetCursorScreenPos();
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    float boxW = 186.0f;
    float boxH = 50.0f;
    float chamfer = 8.0f;

    // Chamfered polygon (top-right and bottom-left clipped)
    ImVec2 poly[6] = {
        ImVec2(pos.x, pos.y),
        ImVec2(pos.x + boxW - chamfer, pos.y),
        ImVec2(pos.x + boxW, pos.y + chamfer),
        ImVec2(pos.x + boxW, pos.y + boxH),
        ImVec2(pos.x + chamfer, pos.y + boxH),
        ImVec2(pos.x, pos.y + boxH - chamfer)
    };

    ImVec4 bgCol = isActive ? ImVec4(0.20f, 0.12f, 0.35f, 1.0f) : ImVec4(0.09f, 0.06f, 0.16f, 1.0f);
    ImVec4 borderCol = isActive ? color : ColBorderDim;

    drawList->AddConvexPolyFilled(poly, 6, ImGui::ColorConvertFloat4ToU32(bgCol));
    drawList->AddPolyline(poly, 6, ImGui::ColorConvertFloat4ToU32(borderCol), ImDrawFlags_Closed, isActive ? 1.6f : 1.0f);

    // Stylized chevron indicator
    float indX = pos.x + 10.0f;
    float indY = pos.y + 16.0f;
    if (isActive) {
        ImVec2 arrow[3] = {
            ImVec2(indX, indY),
            ImVec2(indX + 8.0f, indY + 9.0f),
            ImVec2(indX, indY + 18.0f)
        };
        drawList->AddConvexPolyFilled(arrow, 3, ImGui::ColorConvertFloat4ToU32(color));
    } else {
        drawList->AddRectFilled(ImVec2(indX, indY + 3.0f), ImVec2(indX + 4.0f, indY + 15.0f),
            ImGui::ColorConvertFloat4ToU32(ColBorderDim));
    }

    // Text labels
    drawList->AddText(ImVec2(pos.x + 24.0f, pos.y + 6.0f), ImGui::ColorConvertFloat4ToU32(ColTextMuted), title);
    drawList->AddText(ImVec2(pos.x + 24.0f, pos.y + 24.0f), ImGui::ColorConvertFloat4ToU32(isActive ? color : ColText), status);

    ImGui::Dummy(ImVec2(boxW, boxH));
}

void RenderKeyStep(const char* label, int index, bool isCurrent, float width, float height) {
    ImVec2 pos = ImGui::GetCursorScreenPos();
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    float slant = 6.0f;

    // Slanted parallelogram key tile
    ImVec2 poly[4] = {
        ImVec2(pos.x + slant, pos.y),
        ImVec2(pos.x + width + slant, pos.y),
        ImVec2(pos.x + width, pos.y + height),
        ImVec2(pos.x, pos.y + height)
    };

    ImVec4 bgCol = isCurrent ? ColNeonMagenta : ImVec4(0.12f, 0.08f, 0.21f, 1.0f);
    ImVec4 textCol = isCurrent ? ImVec4(1.0f, 1.0f, 1.0f, 1.0f) : ColText;
    ImVec4 subCol = isCurrent ? ImVec4(0.95f, 0.85f, 1.0f, 1.0f) : ColTextMuted;
    ImVec4 borderCol = isCurrent ? ImVec4(1.0f, 1.0f, 1.0f, 1.0f) : ColBorder;

    drawList->AddConvexPolyFilled(poly, 4, ImGui::ColorConvertFloat4ToU32(bgCol));
    drawList->AddPolyline(poly, 4, ImGui::ColorConvertFloat4ToU32(borderCol), ImDrawFlags_Closed, isCurrent ? 2.0f : 1.0f);

    // Step index text (#1, #2...)
    char stepIndexStr[16];
    snprintf(stepIndexStr, sizeof(stepIndexStr), "#%d", index + 1);

    ImVec2 textSize = ImGui::CalcTextSize(label);
    ImVec2 subSize = ImGui::CalcTextSize(stepIndexStr);

    float textX = pos.x + (width - textSize.x) * 0.5f + slant * 0.5f;
    float textY = pos.y + 6.0f;
    drawList->AddText(ImVec2(textX, textY), ImGui::ColorConvertFloat4ToU32(textCol), label);

    float subX = pos.x + (width - subSize.x) * 0.5f + slant * 0.5f;
    float subY = pos.y + height - subSize.y - 4.0f;
    drawList->AddText(ImVec2(subX, subY), ImGui::ColorConvertFloat4ToU32(subCol), stepIndexStr);

    ImGui::Dummy(ImVec2(width + slant, height));
}

} // namespace AtlusTheme
