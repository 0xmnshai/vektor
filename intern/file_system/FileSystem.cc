#include "FileSystem.hh"

#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <cerrno>
#include <cstring>

namespace vektor
{
namespace file_system
{

bool FileSystem::CreateDirectory(const char* path)
{
    if (Exists(path))
    {
        return true;
    }

    // specific to POSIX/Unix/Mac
    if (mkdir(path, 0755) == 0)
    {
        return true;
    }

    return (errno == EEXIST);
}

FILE* FileSystem::OpenFile(const char* path,
                           const char* mode)
{
    return fopen(path, mode);
}

bool FileSystem::Exists(const char* path)
{
    struct stat buffer;
    return (stat(path, &buffer) == 0);
}

std::string FileSystem::JoinPath(const std::string& info,
                                 const std::string& path)
{
    if (info.empty())
    {
        return path;
    }
    if (path.empty())
    {
        return info;
    }

    if (info.back() == '/' || path.front() == '/')
    {
        return info + path;
    }
    return info + "/" + path;
}

} // namespace file_system
} // namespace vektor
