
#include "VKE_path_utils.hh"

namespace vektor
{
namespace vklib
{

bool VKE_path_is_rel(const char* path)
{
    return path[0] == '/' && path[1] == '/';
}

} // namespace vklib
} // namespace vektor