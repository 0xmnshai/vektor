#pragma once

#include "../dna/DNA_workspace_types.h"
#include "VKE_main.hh"
#include "wm_types.h"

namespace vektor
{

#define GETTER_ATTRS ATTR_NONNULL() ATTR_WARN_UNUSED_RESULT
#define SETTER_ATTRS ATTR_NONNULL(1)

void             VKE_workspace_active_set(WorkSpaceInstanceHook* hook,
                                          WorkSpace*             workspace) SETTER_ATTRS;

WorkSpaceLayout* VKE_workspace_layout_find_global(const Main*    bmain,
                                                  const bScreen* screen,
                                                  WorkSpace**    r_workspace) ATTR_NONNULL(1,
                                                                                        2);

void             VKE_workspace_active_screen_set(WorkSpaceInstanceHook* hook,
                                                 int                    winid,
                                                 WorkSpace*             workspace,
                                                 bScreen*               screen) SETTER_ATTRS;

WorkSpaceLayout* VKE_workspace_layout_find(const WorkSpace* workspace,
                                           const bScreen*   screen) ATTR_NONNULL() ATTR_WARN_UNUSED_RESULT;

void             VKE_workspace_active_layout_set(WorkSpaceInstanceHook* hook,
                                                 int                    winid,
                                                 WorkSpace*             workspace,
                                                 WorkSpaceLayout*       layout) SETTER_ATTRS;

WorkSpaceLayout* VKE_workspace_layout_add_from_layout(Main*                  bmain,
                                                      WorkSpace&             workspace_dst,
                                                      const WorkSpaceLayout& layout_src,
                                                      const int              id_copy_flags);

bScreen*         VKE_workspace_layout_screen_get(const WorkSpaceLayout* layout) GETTER_ATTRS;

const char*      VKE_workspace_layout_name_get(const WorkSpaceLayout* layout) GETTER_ATTRS;

} // namespace vektor