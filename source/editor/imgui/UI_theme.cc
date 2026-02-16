#include "UI_theme.hh"

namespace vektor
{

vkTheme get_default_theme()
{
    vkTheme theme;

    theme.tui.wcol_regular    = ImVec4(0.18f, 0.18f, 0.18f, 1.00f);
    theme.tui.wcol_tool       = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
    theme.tui.wcol_text       = ImVec4(0.90f, 0.90f, 0.90f, 1.00f);
    theme.tui.wcol_text_sel   = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    theme.tui.wcol_option     = ImVec4(0.90f, 0.90f, 0.90f, 1.00f);

    theme.tui.widget_emboss   = ImVec4(0.00f, 0.00f, 0.00f, 0.50f);
    theme.tui.widget_border   = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
    theme.tui.widget_text     = ImVec4(0.90f, 0.90f, 0.90f, 1.00f);
    theme.tui.widget_state    = ImVec4(0.28f, 0.28f, 0.28f, 1.00f);

    theme.tspace.bg_color     = ImVec4(0.18f, 0.18f, 0.18f, 1.00f);
    theme.tspace.accent_color = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
    theme.tspace.fg_color     = ImVec4(0.90f, 0.90f, 0.90f, 1.00f);
    theme.tspace.title        = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
    theme.tspace.text         = ImVec4(0.90f, 0.90f, 0.90f, 1.00f);
    theme.tspace.text_hi      = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    theme.tspace.header       = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
    theme.tspace.header_text  = ImVec4(0.90f, 0.90f, 0.90f, 1.00f);
    theme.tspace.button       = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
    theme.tspace.button_hi    = ImVec4(0.28f, 0.28f, 0.28f, 1.00f);
    theme.tspace.button_text  = ImVec4(0.90f, 0.90f, 0.90f, 1.00f);

    return theme;
}

void setup_vektor_theme(const vkTheme* theme_chk)
{
    vkTheme theme;

    if (theme_chk)
    {
        theme = *theme_chk;
    }
    else
    {
        theme                           = get_default_theme();

        ImGuiStyle& style               = ImGui::GetStyle();
        ImVec4*     colors              = style.Colors;

        colors[ImGuiCol_Text]           = theme.tui.wcol_text;
        colors[ImGuiCol_TextDisabled]   = ImVec4(0.60f, 0.60f, 0.60f, 1.00f); // Keep hardcoded for now or add to struct
        colors[ImGuiCol_WindowBg]       = theme.tui.wcol_regular;
        colors[ImGuiCol_ChildBg]        = ImVec4(theme.tui.wcol_regular.x * 0.9f, theme.tui.wcol_regular.y * 0.9f,
                                                 theme.tui.wcol_regular.z * 0.9f, 1.0f);
        colors[ImGuiCol_PopupBg]        = theme.tui.wcol_tool;
        colors[ImGuiCol_Border]         = theme.tui.widget_border;
        colors[ImGuiCol_BorderShadow]   = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
        colors[ImGuiCol_FrameBg]        = ImVec4(0.10f, 0.10f, 0.10f, 1.00f); // Inputs
        colors[ImGuiCol_FrameBgHovered] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
        colors[ImGuiCol_FrameBgActive]  = theme.tui.widget_state;
        colors[ImGuiCol_TitleBg]        = theme.tspace.title;
        colors[ImGuiCol_TitleBgActive]  = theme.tspace.title;
        colors[ImGuiCol_TitleBgCollapsed]     = theme.tspace.title;
        colors[ImGuiCol_MenuBarBg]            = theme.tui.wcol_tool;
        colors[ImGuiCol_ScrollbarBg]          = ImVec4(0.16f, 0.16f, 0.16f, 1.00f);
        colors[ImGuiCol_ScrollbarGrab]        = ImVec4(0.30f, 0.30f, 0.30f, 1.00f);
        colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.40f, 0.40f, 0.40f, 1.00f);
        colors[ImGuiCol_ScrollbarGrabActive]  = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
        colors[ImGuiCol_CheckMark]            = theme.tui.wcol_option;
        colors[ImGuiCol_SliderGrab]           = ImVec4(0.35f, 0.35f, 0.35f, 1.00f);
        colors[ImGuiCol_SliderGrabActive]     = ImVec4(0.38f, 0.38f, 0.38f, 1.00f);

        // Vektor Selection / Buttons
        colors[ImGuiCol_Button]               = theme.tspace.button;
        colors[ImGuiCol_ButtonHovered]        = theme.tspace.button_hi;
        colors[ImGuiCol_ButtonActive]      = ImVec4(0.33f, 0.60f, 0.86f, 1.00f); // Blue-ish for active in some contexts

        colors[ImGuiCol_Header]            = theme.tspace.header;
        colors[ImGuiCol_HeaderHovered]     = ImVec4(0.28f, 0.28f, 0.28f, 1.00f);
        colors[ImGuiCol_HeaderActive]      = ImVec4(0.93f, 0.54f, 0.08f, 1.00f); // Vektor Orange

        colors[ImGuiCol_Separator]         = theme.tui.widget_border;
        colors[ImGuiCol_SeparatorHovered]  = theme.tui.widget_border;
        colors[ImGuiCol_SeparatorActive]   = theme.tui.widget_border;
        colors[ImGuiCol_ResizeGrip]        = ImVec4(0.26f, 0.59f, 0.98f, 0.25f);
        colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
        colors[ImGuiCol_ResizeGripActive]  = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
        colors[ImGuiCol_Tab]               = theme.tspace.title;
        colors[ImGuiCol_TabHovered]        = theme.tspace.button_hi;
        colors[ImGuiCol_TabActive]         = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
        colors[ImGuiCol_TabUnfocused]      = theme.tspace.title;
        colors[ImGuiCol_TabUnfocusedActive]    = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
        colors[ImGuiCol_PlotLines]             = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
        colors[ImGuiCol_PlotLinesHovered]      = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
        colors[ImGuiCol_PlotHistogram]         = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
        colors[ImGuiCol_PlotHistogramHovered]  = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
        colors[ImGuiCol_TableHeaderBg]         = ImVec4(0.19f, 0.19f, 0.20f, 1.00f);
        colors[ImGuiCol_TableBorderStrong]     = ImVec4(0.31f, 0.31f, 0.35f, 1.00f);
        colors[ImGuiCol_TableBorderLight]      = ImVec4(0.23f, 0.23f, 0.25f, 1.00f);
        colors[ImGuiCol_TableRowBg]            = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
        colors[ImGuiCol_TableRowBgAlt]         = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
        colors[ImGuiCol_TextSelectedBg]        = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
        colors[ImGuiCol_DragDropTarget]        = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
        colors[ImGuiCol_NavHighlight]          = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
        colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
        colors[ImGuiCol_NavWindowingDimBg]     = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
        colors[ImGuiCol_ModalWindowDimBg]      = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);

        // Styling
        style.WindowPadding                    = ImVec2(8.00f, 8.00f);
        style.FramePadding                     = ImVec2(5.00f, 5.00f);
        style.CellPadding                      = ImVec2(6.00f, 6.00f);
        style.ItemSpacing                      = ImVec2(6.00f, 6.00f);
        style.ItemInnerSpacing                 = ImVec2(6.00f, 6.00f);
        style.TouchExtraPadding                = ImVec2(0.00f, 0.00f);
        style.IndentSpacing                    = 25.00f;
        style.ScrollbarSize                    = 12.00f;
        style.GrabMinSize                      = 10.00f;
        style.WindowBorderSize                 = 1.00f;
        style.ChildBorderSize                  = 1.00f;
        style.PopupBorderSize                  = 1.00f;
        style.FrameBorderSize                  = 0.00f;
        style.TabBorderSize                    = 0.00f;
        style.WindowRounding                   = 4.00f;
        style.ChildRounding                    = 4.00f;
        style.FrameRounding                    = 3.00f;
        style.PopupRounding                    = 4.00f;
        style.ScrollbarRounding                = 9.00f;
        style.GrabRounding                     = 3.00f;
        style.LogSliderDeadzone                = 4.00f;
        style.TabRounding                      = 4.00f;
    }
}
} // namespace vektor