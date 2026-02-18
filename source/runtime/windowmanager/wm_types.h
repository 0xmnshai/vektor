#pragma once

#include <memory>
#include <string>

#include "../dna/DNA_listBase.h"
#include "../dna/DNA_screen_types.h"

namespace vektor
{
struct vkContext;
struct wmEvent;
struct bScreen;
struct wmWindow;
struct WindowRuntime;

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
    std::string        name;

    ListBaseT<ScrArea> areabase = {nullptr, nullptr};

    bScreen(const std::string& n)
        : name(n)
    {
    }

    void add_area(std::shared_ptr<ScrArea> area)
    {
        areabase.last  = area.get();
        areabase.first = area.get();
    }
};
} // namespace vektor
