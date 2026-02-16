#pragma once

#include <cstdint>

#include "DNA_defs.h"
#include "DNA_listBase.h"
#include "DNA_space_types.h"
#include "DNA_vec_types.h"
#include "DNA_view2d_types.h"
#include "DNA_windowmanager_types.h"

namespace vektor
{

#define AREAGRID 1
#define AREAMINX 29
#define HEADER_PADDING_Y 6
#define HEADERY (20 + HEADER_PADDING_Y)

struct ScrArea_Runtime
{
    struct bToolRef* tool        = nullptr;
    char             is_tool_set = 0;
    char             _pad0[7]    = {};
};

enum GlobalAreaFlag
{
    GLOBAL_AREA_IS_HIDDEN = (1 << 0),
};

enum GlobalAreaAlign
{
    GLOBAL_AREA_ALIGN_TOP    = 0,
    GLOBAL_AREA_ALIGN_BOTTOM = 1,
};

struct ScrGlobalAreaData
{

    short cur_fixed_height = 0;

    short size_min = 0, size_max = 0;

    short align   = 0;

    short flag    = 0;
    char  _pad[2] = {};
};

struct ScrVert
{
    struct ScrVert *next = nullptr, *prev = nullptr, *newv = nullptr;
    vec2s           vec  = {};

    short           flag = 0, editflag = 0;
};

struct ScrEdge
{
    struct ScrEdge *next = nullptr, *prev = nullptr;
    ScrVert *       v1 = nullptr, *v2 = nullptr;

    short           border  = 0;
    short           flag    = 0;
    char            _pad[4] = {};
};

struct LayoutPanelState
{
    struct LayoutPanelState *next = nullptr, *prev = nullptr;

    char*                    idname    = nullptr;
    uint8_t                  flag      = 0;
    char                     _pad[3]   = {};

    uint32_t                 last_used = 0;
};

struct ARegion
{
    struct ARegion *next = nullptr, *prev = nullptr;

    View2D          v2d;

    rcti            winrct = {};

    short           winx = 0, winy = 0;

    int             category_scroll = 0;

    short           regiontype      = 0;

    short           alignment       = 0;

    short           flag            = 0;

    short           sizex = 0, sizey = 0;

    short           overlap        = 0;

    short           flagfullscreen = 0;

    char            _pad[2]        = {};
};

struct ScrArea
{
    struct ScrArea *                 next = nullptr, *prev = nullptr;

    ScrVert *                        v1 = nullptr, *v2 = nullptr, *v3 = nullptr, *v4 = nullptr;

    bScreen*                         full                 = nullptr;

    rcti                             totrct               = {};

    char                             spacetype            = 0;

    char                             butspacetype         = 0;
    short                            butspacetype_subtype = 0;

    short                            winx = 0, winy = 0;

    DNA_DEPRECATED char              headertype        = 0;

    char                             do_refresh        = 0;
    short                            flag              = 0;

    short                            region_active_win = 0;
    char                             _pad[2]           = {};

    struct SpaceType*                type              = nullptr;

    ScrGlobalAreaData*               global            = nullptr;

    float                            quadview_ratio[2] = {};

    ListBaseT<SpaceLink>             spacedata;

    ListBaseT<ARegion>               regionbase;
    ListBaseT<struct wmEventHandler> handlers;

    ListBaseT<struct AZone>          actionzones;

    ScrArea_Runtime                  runtime;
};

struct ScrAreaMap
{

    ListBaseT<ScrVert> vertbase;
    ListBaseT<ScrEdge> edgebase;
    ListBaseT<ScrArea> areabase;
};

}; // namespace vektor