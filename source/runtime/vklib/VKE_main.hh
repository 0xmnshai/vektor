#pragma once

#include <memory>
#include "../../intern/gaurdalloc/MEM_gaurdalloc.hh"
#include "VKE_utility_mixins.hh"
#include "VKE_vector_set.hh"

#include "../dna/DNA_listBase.h"
#include "VKE_compiler_attrs.h"

#include "../dna/DNA_workspace_types.h"

namespace vektor
{
struct wmWindowManager;

struct Main
    : vklib::NonCopyable
    , vklib::NonMovable
{
    std::shared_ptr<VectorSet<Main*>> split_mains                 = {};

    ListBaseT<wmWindowManager>        wm                          = {};

    char                              filepath[/*FILE_MAX*/ 1024] = "";
    short                             versionfile                 = 0;
    short                             subversionfile              = 0;
    short                             minversionfile              = 0;
    short                             minsubversionfile           = 0;

    bool                              is_read_invalid             = false;

    ListBaseT<WorkSpace>              workspaces                  = {};

    // We will define them and include which are already defined

    // ListBaseT<Scene>                  scenes                      = {};
    // ListBaseT<Library>                libraries                   = {};
    // ListBaseT<Object>                 objects                     = {};
    // ListBaseT<Mesh>                   meshes                      = {};
    // ListBaseT<Tex>                    textures                    = {};
    // ListBaseT<Image>                  images                      = {};
    // ListBaseT<Light>                  lights                      = {};
    // ListBaseT<Camera>                 cameras                     = {};
    // ListBaseT<World>                  worlds                      = {};
    // ListBaseT<bScreen>                screens                     = {};

    Main();
    ~Main() = default;
};

const char* VKE_main_vktfile_path(const Main* bmain) ATTR_NONNULL();

const char* VKE_main_vktfile_path_from_global() ATTR_NONNULL();

// void        VKE_main_free(Main* bmain);

inline void VKE_main_free(Main* bmain)
{
    MEM_delete(bmain);
}

} // namespace vektor