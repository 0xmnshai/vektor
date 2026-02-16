#pragma once

#include <functional>
#include <memory>
#include <string>

#include "../dna/DNA_listBase.h"
#include "../dna/DNA_space_enums.h"

namespace vektor
{
struct vkContext;
struct wmEvent;
struct bScreen;
struct ScrArea;
struct ARegion;
struct wmWindow;
struct WindowRuntime;

using EventHandlerPoll = bool (*)(const wmWindow* win,
                                  const ScrArea*  area,
                                  const ARegion*  region,
                                  const wmEvent*  event);

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

    ListBaseT<Handler>                   handlers;

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

struct ScrArea : public ListBaseT<ScrArea>
{
    char               spacetype = eSpace_Type::SPACE_EMPTY;

    int                x, y;
    int                winx, winy;

    ListBaseT<ARegion> regionbase;

    ListBaseT<Handler> handlers;

    ScrArea(char type)
        : spacetype(type)
        , x(0)
        , y(0)
        , winx(0)
        , winy(0)
    {
    }

    void add_region(std::shared_ptr<ARegion> region)
    {
        regionbase.last  = region.get();
        regionbase.first = region.get();
    }
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

struct wmWindow
{
public:
    std::shared_ptr<bScreen> screen;

    ListBaseT<Handler>       handlers;

    std::shared_ptr<ScrArea> active_area;
    std::shared_ptr<ARegion> active_region;

    int                      posx, posy;
    int                      sizex, sizey;
    std::string              title;

    WindowRuntime*           runtime = nullptr;

    /// need to define them in wm_window_manager
    struct wmWindow *        next = nullptr, *prev = nullptr;

    struct wmWindow*         parent                           = nullptr;

    struct Scene*            scene                            = nullptr; // active

    struct Scene*            new_scene                        = nullptr; // temporary when switching

    char                     view_layer_name[/*MAX_NAME*/ 64] = "";

    bool                     is_temp_screen                   = false;

    wmWindow()
        : posx(0)
        , posy(0)
        , sizex(1280)
        , sizey(720)
        , title("Vektor")
    {
    }

    void set_screen(std::shared_ptr<bScreen> s) { screen = s; }
};

} // namespace vektor
