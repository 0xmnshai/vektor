#pragma once

#include <sys/types.h>
#include <cstdio>
#include <string>

#ifdef _WIN32
#define FS_SEP '\\'
#define FS_SEP_STR "\\"
#else
#define FS_SEP '/'
#define FS_SEP_STR "/"
#endif

#define FS_ATTR_WARN_UNUSED_RESULT __attribute__((warn_unused_result))

namespace vektor
{
const char* FS_dir_home(void);

namespace file_system
{

bool        FS_create_directory(const char* path) FS_ATTR_WARN_UNUSED_RESULT;

bool        FS_exists(const char* path) FS_ATTR_WARN_UNUSED_RESULT;

bool        FS_is_dir(const char* path) FS_ATTR_WARN_UNUSED_RESULT;

bool        FS_is_file(const char* path) FS_ATTR_WARN_UNUSED_RESULT;

FILE*       FS_fopen(const char* path,
                     const char* mode) FS_ATTR_WARN_UNUSED_RESULT;

void        FS_fclose(FILE* file);

int64_t     FS_file_size(const char* path);

std::string FS_path_join(const std::string& dir,
                         const std::string& file);

std::string FS_path_dirname(const std::string& path);

std::string FS_path_basename(const std::string& path);

} // namespace file_system
} // namespace vektor
