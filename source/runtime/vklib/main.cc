#include "../kernel/VK_global.hh"
#include "VKE_main.hh"

namespace vektor
{

const char* VKE_main_vktfile_path(const Main* gmain)
{
    return gmain->filepath;
}

const char* VKE_main_vktfile_path_from_global()
{
    return VKE_main_vktfile_path(G_MAIN);
}

} // namespace vektor