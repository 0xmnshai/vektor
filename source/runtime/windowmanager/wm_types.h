#pragma once

#include <memory>
#include <string>

#include "../dna/DNA_id.h"
#include "../dna/DNA_listBase.h"

namespace vektor
{
struct vkContext;
struct wmEvent;
struct bScreen;
struct wmWindow;
struct WindowRuntime;
struct ScrArea;
struct ARegion;

using EventHandlerPoll = bool (*)(const wmWindow* win,
                                  const ScrArea*  area,
                                  const ARegion*  region,
                                  const wmEvent*  event);

enum class eSpaceType
{
    EMPTY = 0,
    VIEW_3D,
    GRAPH_EDITOR,
    PROPERTIES,
    CONSOLE,
    SCENE_HIERARCHY,
};

enum class RegionType
{
    WINDOW = 0, // Main content
    HEADER,     // Top bar of an area
    TOOLS,      // Left side toolbar
    UI,         // Right side properties/UI
    FOOTER,     // Bottom bar
};

enum class RegionAlignment
{
    NONE = 0,
    TOP,
    BOTTOM,
    LEFT,
    RIGHT,
    HORIZONTAL_SPLIT,
    VERTICAL_SPLIT,
    FLOAT // Overlays
};

struct bScreen : public ListBaseT<bScreen>
{
    static constexpr ID_Type id_type = ID_SCR;

    ID                       id;

    std::string              name;

    char                     state    = 0;

    ListBaseT<ScrArea>       areabase = {nullptr, nullptr};

    bScreen(const std::string& n)
        : name(n)
    {
    }

    void add_area(std::shared_ptr<ScrArea> area)
    {
        areabase.last  = area.get();
        areabase.first = area.get();
    }

    void /*bContextDataCallback*/* context = nullptr;

    /** Window-ID from WM, starts with 1. */
    short                          winid   = 0;
};
} // namespace vektor
