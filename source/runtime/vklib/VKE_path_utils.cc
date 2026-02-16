#include "VKE_path_util.hh"

namespace vektor
{
const char* VKE_path_slash_rfind(const char* path)
{
    const char* const lfslash = strrchr(path, '/');
    const char* const lbslash = strrchr(path, '\\');

    if (!lfslash)
    {
        return lbslash;
    }
    if (!lbslash)
    {
        return lfslash;
    }

    return (lfslash > lbslash) ? lfslash : lbslash;
}

const char* VKE_path_basename(const char* path)
{
    const char* const filename = VKE_path_slash_rfind(path);
    return filename ? filename + 1 : path;
}

const char* VKE_path_extension_or_end(const char* filepath)
{
    const char* const dot = strrchr(filepath, '.');
    if (!dot)
    {
        return filepath + strlen(filepath);
    }
    const char* const last_slash = VKE_path_slash_rfind(filepath);
    if (last_slash && (dot < last_slash))
    {
        return filepath + strlen(filepath);
    }
    return dot;
}

} // namespace vektor