#include "fileops.hh"

#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <cerrno>
#include <cstring>

namespace vektor
{
namespace file_system
{

bool FS_create_directory(const char* path)
{
    if (FS_exists(path))
    {
        return true;
    }

#ifdef _WIN32
    if (_mkdir(path) == 0)
#else
    if (mkdir(path, 0755) == 0)
#endif
    {
        return true;
    }

    return (errno == EEXIST);
}

bool FS_exists(const char* path)
{
    struct stat buffer;
    return (stat(path, &buffer) == 0);
}

bool FS_is_dir(const char* path)
{
    struct stat buffer;
    if (stat(path, &buffer) != 0)
    {
        return false;
    }
    return S_ISDIR(buffer.st_mode);
}

bool FS_is_file(const char* path)
{
    struct stat buffer;
    if (stat(path, &buffer) != 0)
    {
        return false;
    }
    return S_ISREG(buffer.st_mode);
}

FILE* FS_fopen(const char* path,
               const char* mode)
{
    return fopen(path, mode);
}

void FS_fclose(FILE* file)
{
    if (file)
    {
        fclose(file);
    }
}

int64_t FS_file_size(const char* path)
{
    struct stat buffer;
    if (stat(path, &buffer) != 0)
    {
        return -1;
    }
    return static_cast<int64_t>(buffer.st_size);
}

std::string FS_path_join(const std::string& dir,
                         const std::string& file)
{
    if (dir.empty())
    {
        return file;
    }
    if (file.empty())
    {
        return dir;
    }

    if (dir.back() == FS_SEP || file.front() == FS_SEP)
    {
        return dir + file;
    }
    return dir + FS_SEP_STR + file;
}

std::string FS_path_dirname(const std::string& path)
{
    size_t pos = path.find_last_of("\\/");
    if (pos == std::string::npos)
    {
        return "";
    }
    return path.substr(0, pos);
}

std::string FS_path_basename(const std::string& path)
{
    size_t pos = path.find_last_of("\\/");
    if (pos == std::string::npos)
    {
        return path;
    }
    return path.substr(pos + 1);
}

} // namespace file_system
} // namespace vektor
