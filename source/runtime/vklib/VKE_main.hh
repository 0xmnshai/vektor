#pragma once

#include <memory>
#include "VKE_utility_mixins.hh"
#include "VKE_vector_set.hh"

#include "VKE_compiler_attrs.h"

namespace vektor
{

struct Main
    : vklib::NonCopyable
    , vklib::NonMovable
{
    std::shared_ptr<VectorSet<Main*>> split_mains                 = {};

    char                              filepath[/*FILE_MAX*/ 1024] = "";
    short                             versionfile                 = 0;
    short                             subversionfile              = 0;
    short                             minversionfile              = 0;
    short                             minsubversionfile           = 0;

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
    ~Main();
};

const char* VKE_main_vktfile_path(const Main* bmain) ATTR_NONNULL();

const char* VKE_main_vktfile_path_from_global() ATTR_NONNULL();
} // namespace vektor