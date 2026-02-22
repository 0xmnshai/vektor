#pragma once

#include <cstdint>
#include <functional>
#include <map>

#include "../vklib/VKE_enum_flags.hh"
#include "../vklib/VKE_string_ref.hh"
#include "../windowmanager/wm_event_system.h"
#include "DNA_defs.h"
#include "DNA_listBase.h"
#include "DNA_vec_types.h"
#include "DNA_view2d_types.h"

namespace vektor
{

using vklib::StringRef;

struct bScreen;
struct SpaceLink;
struct vkContext;
struct wmEvent;
struct wmKeyConfig;
struct wmWindowManager;
struct Scene;
struct bContextDataResult;
struct bContextStore;
using bContextDataCallback = int (*)(const vkContext*    C,
                                     const char*         member,
                                     bContextDataResult* result);

#define AREAGRID 1
#define AREAMINX 29
#define HEADER_PADDING_Y 6
#define HEADERY (20 + HEADER_PADDING_Y)

enum
{
    SCREENNORMAL    = 0,
    /** One editor taking over the screen. */
    SCREENMAXIMIZED = 1,
    /**
     * One editor taking over the screen with no bare-minimum UI elements.
     *
     * Besides making the area full-screen this disables navigation & statistics because
     * this is part of a stereo 3D pipeline where these elements would interfere, see: !142418.
     */
    SCREENFULL      = 2,
};

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

struct ARegion; // Forward declaration

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

// interface inten ..h

// node draw /h
enum class Type
{
    Socket,
    Separator,
    Layout,
    PanelHeader,
    PanelContentBegin,
    PanelContentEnd,
};

struct PanelDeclaration;

class ItemDeclaration
{
public:
    const PanelDeclaration* parent = nullptr;

    virtual ~ItemDeclaration()     = default;
};

// nod node declaration. h

enum eNodeSocketInOut
{
    SOCK_IN  = 1 << 0,
    SOCK_OUT = 1 << 1,
};
ENUM_OPERATORS(eNodeSocketInOut);

struct NodeDeclarationBuilder;

// will define when needed ....
class BaseSocketDeclarationBuilder
{
protected:
    bool                    reference_pass_all_ = false;
    bool                    field_on_all_       = false;
    bool                    propagate_from_all_ = false;
    NodeDeclarationBuilder* node_decl_builder_  = nullptr;
    // SocketDeclaration*      decl_base_          = nullptr;

    friend class NodeDeclarationBuilder;
    friend class DeclarationListBuilder;

public:
    virtual ~BaseSocketDeclarationBuilder() = default;
};

class DeclarationListBuilder
{
public:
    NodeDeclarationBuilder&        node_decl_builder;
    std::vector<ItemDeclaration*>& items;
    PanelDeclaration*              parent_panel_decl = nullptr;

    DeclarationListBuilder(NodeDeclarationBuilder&        node_decl_builder,
                           std::vector<ItemDeclaration*>& items)
        : node_decl_builder(node_decl_builder)
        , items(items)
    {
    }

    template <typename DeclType>
    typename DeclType::Builder& add_socket(StringRef        name,
                                           StringRef        identifier,
                                           eNodeSocketInOut in_out);

    template <typename DeclType>
    typename DeclType::Builder& add_input(StringRef name,
                                          StringRef identifier = "");
    template <typename DeclType>
    typename DeclType::Builder& add_output(StringRef name,
                                           StringRef identifier = "");

    // BaseSocketDeclarationBuilder& add_input(eNodeSocketDatatype socket_type,
    //                                         StringRef           name,
    //                                         StringRef           identifier = "");
    // BaseSocketDeclarationBuilder& add_input(eCustomDataType data_type,
    //                                         StringRef       name,
    //                                         StringRef       identifier = "");
    // BaseSocketDeclarationBuilder& add_output(eNodeSocketDatatype socket_type,
    //                                          StringRef           name,
    //                                          StringRef           identifier = "");
    // BaseSocketDeclarationBuilder& add_output(eCustomDataType data_type,
    //                                          StringRef       name,
    //                                          StringRef       identifier = "");

    // PanelDeclarationBuilder&      add_panel(StringRef name,
    //                                         int       identifier = -1);

    // void                          add_separator();
    // void                          add_default_layout();
    // void                          add_layout(std::function<void(ui::Layout&,
    //                                    vkContext*,
    //                                    PointerRNA*)> draw);
};

class NodeDeclarationBuilder : public DeclarationListBuilder
{
private:
    // const bNodeType&                                      typeinfo_;
    // NodeDeclaration&                                      declaration_;
    // const bNodeTree*                                      ntree_ = nullptr;
    // const bNode*                                          node_  = nullptr;
    // Vector<std::unique_ptr<BaseSocketDeclarationBuilder>> socket_builders_;
    // Vector<BaseSocketDeclarationBuilder*>                 input_socket_builders_;
    // Vector<BaseSocketDeclarationBuilder*>                 output_socket_builders_;
    // Vector<std::unique_ptr<PanelDeclarationBuilder>>      panel_builders_;
    // bool                                                  is_function_node_ = false;

    friend DeclarationListBuilder;

public:
    // NodeDeclarationBuilder(const bke::bNodeType& typeinfo,
    //                        NodeDeclaration&      declaration,
    //                        const bNodeTree*      ntree = nullptr,
    //                        const bNode*          node  = nullptr);

    // const bNode* node_or_null() const
    // {
    //     declaration_.is_context_dependent = true;
    //     return node_;
    // }

    // const bNodeTree* tree_or_null() const
    // {
    //     declaration_.is_context_dependent = true;
    //     return ntree_;
    // }

    /**
     * All inputs support fields, and all outputs are fields if any of the inputs is a field.
     * Calling field status definitions on each socket is unnecessary.
     */
    // void                  is_function_node() { is_function_node_ = true; }

    // void                  finalize();

    // void                  use_custom_socket_order(bool enable = true);
    // void                  allow_any_socket_order(bool enable = true);

    // aal::RelationsInNode& get_anonymous_attribute_relations()
    // {
    //     if (!declaration_.anonymous_attribute_relations_)
    //     {
    //         declaration_.anonymous_attribute_relations_ = std::make_unique<aal::RelationsInNode>();
    //     }
    //     return *declaration_.anonymous_attribute_relations_;
    // }

    // NodeDeclaration& declaration() { return declaration_; }

private:
    void build_remaining_anonymous_attribute_relations();
};

class PanelDeclaration : public ItemDeclaration
{
public:
    int                           identifier;
    std::string                   name;
    std::string                   description;
    std::optional<std::string>    translation_context;
    bool                          default_collapsed = false;
    std::vector<ItemDeclaration*> items;
    /** Index in the list of panels on the node. */
    int                           index        = -1;
    PanelDeclaration*             parent_panel = nullptr;

private:
    friend NodeDeclarationBuilder;
    friend class PanelDeclarationBuilder;

public:
    ~PanelDeclaration() override = default;

    // void                     build(bNodePanelState& panel) const;
    // bool                     matches(const bNodePanelState& panel) const;
    // void                     update_or_build(const bNodePanelState& old_panel,
    //                                          bNodePanelState&       new_panel) const;

    int depth() const;

    /** Get the declaration for a child item that should be drawn as part of the panel header. */
    // const SocketDeclaration* panel_input_decl() const;
};

class LayoutDeclaration : public ItemDeclaration
{
public:
    // std::function<DrawNodeLayoutFn> draw;
    /**
     * Sometimes the default layout has special handling (e.g. choose between #draw_buttons and
     * #draw_buttons_ex).
     */
    bool is_default = false;
};

struct Layout
{
    static constexpr Type type = Type::Layout;
    // const nodes::LayoutDeclaration* decl;
};

struct Block;

struct Button
{

    /** Pointer back to the layout item holding this button. */
    Layout*                                    layout   = nullptr;
    int                                        flag     = 0;
    int                                        drawflag = 0;
    char                                       flag2    = 0;

    short                                      retval = 0, strwidth = 0, alignnr = 0;
    short                                      ofs = 0, pos = 0, selsta = 0, selend = 0;

    std::string                                str;

    std::string                                drawstr;

    char*                                      placeholder = nullptr;

    char*                                      poin        = nullptr;
    float                                      hardmin = 0, hardmax = 0, softmin = 0, softmax = 0;

    std::function<void(vkContext&)>            apply_func;

    void*                                      func_argN        = nullptr;

    const bContextStore*                       context          = nullptr;

    /**
     * When defined, and the button edits a string RNA property,
     * the new name is _not_ set at all, instead this function is called with the new name.
     */
    std::function<void(std::string& new_name)> rename_full_func = nullptr;
    std::string                                rename_full_new;

    const char*                                disabled_info = nullptr;

    /** could be made into a single flag */
    bool                                       changed       = false;

    Block*                                     block         = nullptr;

    Button()                                                 = default;
    /** Performs a mostly shallow copy for now. Only contained C++ types are deep copied. */
    Button(const Button& other)                              = default;
    /** Mostly shallow copy, just like copy constructor above. */
    Button& operator=(const Button& other)                   = default;

    virtual ~Button()                                        = default;
};

struct Block
{
    Block *                                     next = nullptr, *prev = nullptr;

    std::vector<std::unique_ptr<Button>>        buttons;
    // Panel*                                      panel    = nullptr;
    Block*                                      oldblock = nullptr;

    // ListBaseT<ButStore>                         butstore = {nullptr, nullptr};

    std::vector<std::unique_ptr<bContextStore>> contexts;
};

struct ARegionType
{
    ARegionType *next, *prev;
    /** Unique identifier within this space, defines `RGN_TYPE_xxxx`. */
    int          regionid;

    /** Add handlers, stuff you only do once or on area/region type/size changes. */
    void (*init)(wmWindowManager* wm,
                 ARegion*         region);
    /** Exit is called when the region is hidden or removed. */
    void (*exit)(wmWindowManager* wm,
                 ARegion*         region);

    //  bool (*poll)(const RegionPollParams *params);
    /** Draw entirely, view changes should be handled here. */
    void (*draw)(const vkContext* C,
                 ARegion*         region);

    void (*draw_overlay)(const vkContext* C,
                         ARegion*         region);
    /** Optional, compute button layout before drawing for dynamic size. */
    void (*layout)(const vkContext* C,
                   ARegion*         region);

    void (*free)(ARegion*);

    void (*operatortypes)();
    /** Add items to keymap. */
    void (*keymap)(wmKeyConfig* keyconf);
    /** Return context data. */

    bContextDataCallback context;
};

struct ARegionRuntime
{
public:
    /** Callbacks for this region type. */
    struct ARegionType*           type;

    /** Runtime for partial redraw, same or smaller than #ARegion::winrct. */
    rcti                          drawrct      = {};

    /**
     * The visible part of the region, use with region overlap not to draw
     * on top of the overlapping regions.
     *
     * Lazy initialize, zeroed when unset, relative to #ARegion.winrct x/y min.
     */
    rcti                          visible_rect = {};

    /**
     * The offset needed to not overlap with window scroll-bars.
     * Only used by HUD regions for now.
     */
    int                           offset_x     = 0;
    int                           offset_y     = 0;

    /** Panel category to use between 'layout' and 'draw'. */
    const char*                   category     = nullptr;

    /** Maps #ui::Block::name to ui::Block for faster lookups. */
    std::map<std::string, Block*> block_name_map;
    ListBaseT<Block>              uiblocks    = {};

    ListBaseT<wmEventHandler>     handlers    = {};

    /** Use this string to draw info. */
    char*                         headerstr   = nullptr;

    /** Blend in/out. */
    wmTimer*                      regiontimer = nullptr;

    // wmDrawBuffer*                 draw_buffer     = nullptr;

    /** Panel categories runtime. */
    // ListBaseT<PanelCategoryDyn>   panels_category = {};

    /** Region is currently visible on screen. */
    short                         visible     = 0;

    /** Private, cached notifier events. */
    short                         do_draw     = 0;

    /** Private, cached notifier events. */
    short                         do_draw_paintcursor;
};

struct ARegion
{
    struct ARegion *                     next = nullptr, *prev = nullptr;

    View2D                               v2d;

    rcti                                 winrct = {};

    short                                winx = 0, winy = 0;

    int                                  category_scroll = 0;

    short                                regiontype      = 0;

    short                                alignment       = 0;

    short                                flag            = 0;

    short                                sizex = 0, sizey = 0;

    short                                overlap        = 0;

    short                                flagfullscreen = 0;

    char                                 _pad[2]        = {};

    ListBaseT<Handler>                   handlers;

    void*                                custom_data = nullptr;

    std::function<void(ARegion*)>        on_draw; // ImGui render
    std::function<void(ARegion*, float)> on_update;

    ARegionRuntime*                      runtime = nullptr;
};

#define VKE_ST_MAXNAME 64

struct ScrArea;

struct SpaceType
{
    /** For menus. */
    char name[VKE_ST_MAXNAME];
    /** Unique space identifier. */
    int  spaceid;
    /** Icon lookup for menus. */
    int  iconid;

    SpaceLink* (*create)(const ScrArea* area,
                         const Scene*   scene);

    void (*free)(SpaceLink* sl);

    /** Init is to cope with file load, screen (size) changes, check handlers. */
    void (*init)(wmWindowManager* wm,
                 ScrArea*         area);
    /** Exit is called when the area is hidden or removed. */
    void (*exit)(wmWindowManager* wm,
                 ScrArea*         area);
    /** Listeners can react to bContext changes. */
    // void (*listener)(const wmSpaceTypeListenerParams* params);

    /** Called when the mouse moves out of the area. */
    void (*deactivate)(ScrArea* area);

    /** Refresh context, called after file-reads, #ED_area_tag_refresh(). */
    void (*refresh)(const vkContext* C,
                    ScrArea*         area);

    bContextDataCallback context;
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
    ListBaseT<Handler>               handlers;
    ListBaseT<struct wmEventHandler> event_handlers;

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