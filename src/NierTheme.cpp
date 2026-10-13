#include "NierTheme.h"
#include <cstdio>

namespace NierTheme {

void ApplyTheme() {
    ImGuiStyle& style = ImGui::GetStyle();

    // YoRHa Crisp Angular Architecture (Zero Rounding)
    style.WindowRounding    = 0.0f;
    style.ChildRounding     = 0.0f;
    style.FrameRounding     = 0.0f;
    style.PopupRounding     = 0.0f;
    style.ScrollbarRounding = 0.0f;
    style.GrabRounding      = 0.0f;
    style.TabRounding       = 0.0f;

    // Crisp Borders
    style.WindowBorderSize  = 1.0f;
    style.ChildBorderSize   = 1.0f;
    style.FrameBorderSize   = 1.0f;
    style.PopupBorderSize   = 1.0f;
    style.TabBorderSize     = 1.0f;

    // Spacious, modern layout
    style.WindowPadding     = ImVec2(16.0f, 16.0f);
    style.FramePadding      = ImVec2(12.0f, 8.0f);
    style.ItemSpacing       = ImVec2(12.0f, 10.0f);
    style.ItemInnerSpacing  = ImVec2(8.0f, 6.0f);
    style.ScrollbarSize     = 14.0f;
    style.GrabMinSize       = 14.0f;

    // Colors
    ImVec4* colors = style.Colors;
    colors[ImGuiCol_Text]                  = ColText;
    colors[ImGuiCol_TextDisabled]          = ColTextMuted;
    colors[ImGuiCol_WindowBg]              = ColDarkBg;
    colors[ImGuiCol_ChildBg]               = ColFrameBg;
    colors[ImGuiCol_PopupBg]               = ImVec4(0.12f, 0.11f, 0.10f, 0.98f);
    colors[ImGuiCol_Border]                = ColBorder;
    colors[ImGuiCol_BorderShadow]          = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_FrameBg]               = ColFrameBg;
    colors[ImGuiCol_FrameBgHovered]        = ColFrameBgHover;
    colors[ImGuiCol_FrameBgActive]         = ColFrameBgAct;
    colors[ImGuiCol_TitleBg]               = ColDarkBg;
    colors[ImGuiCol_TitleBgActive]         = ColDarkBg;
    colors[ImGuiCol_TitleBgCollapsed]      = ColDarkBg;
    colors[ImGuiCol_MenuBarBg]             = ColFrameBg;
    colors[ImGuiCol_ScrollbarBg]           = ImVec4(0.10f, 0.09f, 0.08f, 0.60f);
    colors[ImGuiCol_ScrollbarGrab]         = ColBorder;
    colors[ImGuiCol_ScrollbarGrabHovered]  = ColGold;
    colors[ImGuiCol_ScrollbarGrabActive]   = ColGoldBright;
    colors[ImGuiCol_CheckMark]             = ColGold;
    colors[ImGuiCol_SliderGrab]            = ColGold;
    colors[ImGuiCol_SliderGrabActive]      = ColGoldBright;
    colors[ImGuiCol_Button]                = ImVec4(0.19f, 0.18f, 0.16f, 1.00f);
    colors[ImGuiCol_ButtonHovered]         = ImVec4(0.27f, 0.25f, 0.21f, 1.00f);
    colors[ImGuiCol_ButtonActive]          = ColGold;
    colors[ImGuiCol_Header]                = ImVec4(0.22f, 0.21f, 0.18f, 1.00f);
    colors[ImGuiCol_HeaderHovered]         = ImVec4(0.30f, 0.28f, 0.24f, 1.00f);
    colors[ImGuiCol_HeaderActive]          = ColGold;
    colors[ImGuiCol_Separator]             = ColBorderDim;
    colors[ImGuiCol_SeparatorHovered]      = ColBorder;
    colors[ImGuiCol_SeparatorActive]       = ColGold;
    colors[ImGuiCol_ResizeGrip]            = ImVec4(0.30f, 0.28f, 0.24f, 0.50f);
    colors[ImGuiCol_ResizeGripHovered]     = ColGold;
    colors[ImGuiCol_ResizeGripActive]      = ColGoldBright;
    colors[ImGuiCol_Tab]                   = ImVec4(0.16f, 0.15f, 0.13f, 1.00f);
    colors[ImGuiCol_TabHovered]            = ImVec4(0.28f, 0.26f, 0.22f, 1.00f);
    colors[ImGuiCol_TabActive]             = ImVec4(0.24f, 0.22f, 0.19f, 1.00f);
    colors[ImGuiCol_TabUnfocused]          = ImVec4(0.14f, 0.13f, 0.11f, 1.00f);
    colors[ImGuiCol_TabUnfocusedActive]    = ImVec4(0.18f, 0.17f, 0.15f, 1.00f);
    colors[ImGuiCol_PlotLines]             = ColGold;
    colors[ImGuiCol_PlotLinesHovered]      = ColGoldBright;
    colors[ImGuiCol_PlotHistogram]         = ColGold;
    colors[ImGuiCol_PlotHistogramHovered]  = ColGoldBright;
    colors[ImGuiCol_TableHeaderBg]         = ImVec4(0.18f, 0.17f, 0.15f, 1.00f);
    colors[ImGuiCol_TableBorderStrong]     = ColBorder;
    colors[ImGuiCol_TableBorderLight]      = ColBorderDim;
    colors[ImGuiCol_TableRowBg]            = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_TableRowBgAlt]         = ImVec4(1.00f, 1.00f, 1.00f, 0.03f);
    colors[ImGuiCol_TextSelectedBg]        = ImVec4(0.78f, 0.66f, 0.29f, 0.35f);
    colors[ImGuiCol_DragDropTarget]        = ColGoldBright;
    colors[ImGuiCol_NavHighlight]          = ColGold;
}

void RenderHeaderBanner(const char* title, const char* subtitle) {
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    ImVec2 pos = ImGui::GetCursorScreenPos();
    float width = ImGui::GetContentRegionAvail().x;
    float bannerHeight = subtitle ? 52.0f : 36.0f;

    // Draw background bar
    drawList->AddRectFilled(pos, ImVec2(pos.x + width, pos.y + bannerHeight),
        ImGui::ColorConvertFloat4ToU32(ImVec4(0.15f, 0.14f, 0.12f, 0.95f)));

    // Accent line on left (Gold ochre mark)
    drawList->AddRectFilled(pos, ImVec2(pos.x + 4.0f, pos.y + bannerHeight),
        ImGui::ColorConvertFloat4ToU32(ColGold));

    // Outer border
    drawList->AddRect(pos, ImVec2(pos.x + width, pos.y + bannerHeight),
        ImGui::ColorConvertFloat4ToU32(ColBorderDim));

    drawList->AddText(ImVec2(pos.x + 14.0f, pos.y + 6.0f), ImGui::ColorConvertFloat4ToU32(ColGoldBright), title);

    if (subtitle) {
        drawList->AddText(ImVec2(pos.x + 14.0f, pos.y + 28.0f), ImGui::ColorConvertFloat4ToU32(ColTextMuted), subtitle);
    }

    ImGui::Dummy(ImVec2(width, bannerHeight));
    ImGui::Spacing();
}

void RenderSectionDivider(const char* label) {
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    ImVec2 pos = ImGui::GetCursorScreenPos();
    float width = ImGui::GetContentRegionAvail().x;

    float textWidth = ImGui::CalcTextSize(label).x;
    float lineY = pos.y + 10.0f;

    // Draw left decorative slash mark
    drawList->AddLine(pos, ImVec2(pos.x + 20.0f, lineY), ImGui::ColorConvertFloat4ToU32(ColGold), 1.5f);

    // Text position
    drawList->AddText(ImVec2(pos.x + 26.0f, pos.y), ImGui::ColorConvertFloat4ToU32(ColGold), label);

    // Right decorative horizontal line
    float rightStartX = pos.x + 32.0f + textWidth;
    if (rightStartX < pos.x + width) {
        drawList->AddLine(ImVec2(rightStartX, lineY), ImVec2(pos.x + width, lineY),
            ImGui::ColorConvertFloat4ToU32(ColBorderDim), 1.0f);
    }

    ImGui::Dummy(ImVec2(width, 20.0f));
}

void RenderStatusBadge(const char* title, const char* status, bool isActive, const ImVec4& color) {
    ImVec2 pos = ImGui::GetCursorScreenPos();
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    float boxW = 180.0f;
    float boxH = 50.0f;

    ImVec4 bgCol = isActive ? ImVec4(0.20f, 0.18f, 0.14f, 1.0f) : ImVec4(0.13f, 0.12f, 0.11f, 1.0f);
    ImVec4 borderCol = isActive ? color : ColBorderDim;

    drawList->AddRectFilled(pos, ImVec2(pos.x + boxW, pos.y + boxH), ImGui::ColorConvertFloat4ToU32(bgCol));
    drawList->AddRect(pos, ImVec2(pos.x + boxW, pos.y + boxH), ImGui::ColorConvertFloat4ToU32(borderCol), 0.0f, 0, isActive ? 1.5f : 1.0f);

    // Status indicator square
    float indX = pos.x + 10.0f;
    float indY = pos.y + 12.0f;
    drawList->AddRectFilled(ImVec2(indX, indY), ImVec2(indX + 6.0f, indY + 26.0f), ImGui::ColorConvertFloat4ToU32(color));

    // Text labels
    drawList->AddText(ImVec2(pos.x + 24.0f, pos.y + 6.0f), ImGui::ColorConvertFloat4ToU32(ColTextMuted), title);
    drawList->AddText(ImVec2(pos.x + 24.0f, pos.y + 24.0f), ImGui::ColorConvertFloat4ToU32(isActive ? color : ColText), status);

    ImGui::Dummy(ImVec2(boxW, boxH));
}

void RenderKeyStep(const char* label, int index, bool isCurrent, float width, float height) {
    ImVec2 pos = ImGui::GetCursorScreenPos();
    ImDrawList* drawList = ImGui::GetWindowDrawList();

    ImVec4 bgCol = isCurrent ? ColGold : ImVec4(0.16f, 0.15f, 0.13f, 1.0f);
    ImVec4 textCol = isCurrent ? ImVec4(0.12f, 0.11f, 0.10f, 1.0f) : ColText;
    ImVec4 subCol = isCurrent ? ImVec4(0.25f, 0.22f, 0.15f, 1.0f) : ColTextMuted;
    ImVec4 borderCol = isCurrent ? ColGoldBright : ColBorder;

    // Background & Border
    drawList->AddRectFilled(pos, ImVec2(pos.x + width, pos.y + height), ImGui::ColorConvertFloat4ToU32(bgCol));
    drawList->AddRect(pos, ImVec2(pos.x + width, pos.y + height), ImGui::ColorConvertFloat4ToU32(borderCol), 0.0f, 0, isCurrent ? 2.0f : 1.0f);

    // Step index text (#1, #2...)
    char stepIndexStr[16];
    snprintf(stepIndexStr, sizeof(stepIndexStr), "#%d", index + 1);

    ImVec2 textSize = ImGui::CalcTextSize(label);
    ImVec2 subSize = ImGui::CalcTextSize(stepIndexStr);

    float textX = pos.x + (width - textSize.x) * 0.5f;
    float textY = pos.y + 6.0f;
    drawList->AddText(ImVec2(textX, textY), ImGui::ColorConvertFloat4ToU32(textCol), label);

    float subX = pos.x + (width - subSize.x) * 0.5f;
    float subY = pos.y + height - subSize.y - 4.0f;
    drawList->AddText(ImVec2(subX, subY), ImGui::ColorConvertFloat4ToU32(subCol), stepIndexStr);

    ImGui::Dummy(ImVec2(width, height));
}

} // namespace NierTheme
