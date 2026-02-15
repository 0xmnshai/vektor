#pragma once

#include <imgui.h>

namespace vektor
{
struct ThemeUI
{
    ImVec4 wcol_regular;
    ImVec4 wcol_tool;
    ImVec4 wcol_text;
    ImVec4 wcol_text_sel;
    ImVec4 wcol_option;

    ImVec4 widget_emboss;
    ImVec4 widget_border;
    ImVec4 widget_text;
    ImVec4 widget_state;
};

struct ThemeSpace
{
    ImVec4 bg_color;
    ImVec4 fg_color;
    ImVec4 accent_color;
    ImVec4 title;
    ImVec4 text;
    ImVec4 text_hi;
    ImVec4 header;
    ImVec4 header_text;
    ImVec4 button;
    ImVec4 button_hi;
    ImVec4 button_text;
};

struct vkTheme
{
    ThemeUI    tui;
    ThemeSpace tspace;
};

vkTheme get_default_theme();

void    setup_vektor_theme(const vkTheme* theme = nullptr);

} // namespace vektor