#pragma once

#include <functional>
#include <list>
#include <memory>
#include <string>

#include "wm_event.h"

namespace vektor
{
struct bScreen;
struct ScrArea;
struct ARegion;
struct wmWindow;
struct EventHandler;

enum class SpaceType
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

class Handler
{
public:
    using PollFn   = std::function<bool(ARegion*, const wmEvent&)>;
    using HandleFn = std::function<bool(vkContext*, ARegion*, const wmEvent&)>;

    Handler(PollFn   poll,
            HandleFn handle)
        : poll_(poll)
        , handle_(handle)
    {
    }

    bool poll(ARegion*       region,
              const wmEvent& event)
    {
        if (poll_)
            return poll_(region, event);
        return true;
    }

    bool handle(vkContext*     ctx,
                ARegion*       region,
                const wmEvent& event)
    {
        if (handle_)
            return handle_(ctx, region, event);
        return false;
    }

private:
    PollFn   poll_;
    HandleFn handle_;
};
struct ARegion
{
    RegionType                           type;
    RegionAlignment                      alignment;

    int                                  winx, winy; // Size
    int                                  x, y;       // Position relative to Area

    std::list<std::shared_ptr<Handler>>  handlers;

    void*                                custom_data = nullptr;

    std::function<void(ARegion*)>        on_draw; // ImGui render
    std::function<void(ARegion*, float)> on_update;

    ARegion(RegionType      t,
            RegionAlignment align)
        : type(t)
        , alignment(align)
        , winx(0)
        , winy(0)
        , x(0)
        , y(0)
    {
    }
};

struct ScrArea
{
    SpaceType                           spacetype;

    int                                 x, y;
    int                                 winx, winy;

    std::list<std::shared_ptr<ARegion>> regionbase;

    std::list<std::shared_ptr<Handler>> handlers;

    ScrArea(SpaceType type)
        : spacetype(type)
        , x(0)
        , y(0)
        , winx(0)
        , winy(0)
    {
    }

    void add_region(std::shared_ptr<ARegion> region) { regionbase.push_back(region); }
};

struct bScreen
{
    std::string                         name;
    std::list<std::shared_ptr<ScrArea>> areabase;

    bScreen(const std::string& n)
        : name(n)
    {
    }

    void add_area(std::shared_ptr<ScrArea> area) { areabase.push_back(area); }
};

struct wmWindow
{
    std::shared_ptr<bScreen>            screen;

    std::list<std::shared_ptr<Handler>> handlers;

    std::shared_ptr<ScrArea>            active_area;
    std::shared_ptr<ARegion>            active_region;

    wmWindow() {}

    void SetScreen(std::shared_ptr<bScreen> s) { screen = s; }
};

} // namespace vektor
