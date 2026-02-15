#pragma once

#include "DNA_listBase.h"
#include "DNA_view2d_types.h"
#include "DNA_windowmanager_types.h"

namespace vektor
{

struct SpaceLink
{
    struct PanelCategoryStack
    {
        struct PanelCategoryStack *next = nullptr, *prev = nullptr;
        char                       idname[64] = "";
    };

    enum LayoutPanelStateFlag
    {

        LAYOUT_PANEL_STATE_FLAG_OPEN = (1 << 0),
    };

    struct Panel
    {
        struct Panel *                  next = nullptr, *prev = nullptr;

        struct PanelType*               type          = nullptr;

        ui::Layout*                     layout        = nullptr;

        char                            panelname[64] = "";

        char*                           drawname      = nullptr;

        int                             ofsx = 0, ofsy = 0;

        int                             sizex = 0, sizey = 0;

        int                             blocksizex = 0, blocksizey = 0;
        short                           labelofs = 0;
        short                           flag = 0, runtime_flag = 0;
        char                            _pad[6]    = {};

        int                             sortorder  = 0;

        void*                           activedata = nullptr;

        ListBaseT<Panel>                children;

        ListBaseT<LayoutPanelStateFlag> layout_panel_states;

        uint32_t                        layout_panel_states_clock = 0;
        char                            _pad2[4]                  = {};

        struct Panel_Runtime*           runtime                   = nullptr;
    };

    struct ARegion
    {
        struct ARegion *              next = nullptr, *prev = nullptr;

        View2D                        v2d;

        rcti                          winrct = {};

        short                         winx = 0, winy = 0;

        int                           category_scroll = 0;

        short                         regiontype      = 0;

        short                         alignment       = 0;

        short                         flag            = 0;

        short                         sizex = 0, sizey = 0;

        short                         overlap        = 0;

        short                         flagfullscreen = 0;

        char                          _pad[2]        = {};

        ListBaseT<Panel>              panels;

        ListBaseT<PanelCategoryStack> panels_category_active;
    };

    struct SpaceLink * next = nullptr, *prev = nullptr;

    ListBaseT<ARegion> regionbase;
    char               spacetype = 0;
    char               link_flag = 0;
    char               _pad0[6]  = {};
};

struct SpaceInfo
{
    SpaceLink *next = nullptr, *prev = nullptr;

    char       spacetype = 0;
    char       link_flag = 0;
    char       _pad0[6]  = {};

    char       rpt_mask  = 0;
    char       _pad[7]   = {};
};
} // namespace vektor