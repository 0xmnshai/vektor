#pragma once

#include <sys/types.h>
#include <cstdio>
#include <string>

/* Platform specific defines */
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
namespace file_system
{

/**
 * @brief Creates a directory if it does not exist.
 * Recursive creation is NOT supported in this base function.
 *
 * @param path The path of the directory to create.
 * @return true If the directory was created or already exists.
 * @return false If the directory could not be created.
 */
bool        FS_create_directory(const char* path) FS_ATTR_WARN_UNUSED_RESULT;

/**
 * @brief Checks if a path exists (file or directory).
 */
bool        FS_exists(const char* path) FS_ATTR_WARN_UNUSED_RESULT;

/**
 * @brief Checks if a path is a directory.
 */
bool        FS_is_dir(const char* path) FS_ATTR_WARN_UNUSED_RESULT;

/**
 * @brief Checks if a path is a regular file.
 */
bool        FS_is_file(const char* path) FS_ATTR_WARN_UNUSED_RESULT;

/* -------------------------------------------------------------------- */
/** \name File I/O
 * \{ */

/**
 * @brief Opens a file with the given mode. Wrapper around fopen.
 */
FILE*       FS_fopen(const char* path,
                     const char* mode) FS_ATTR_WARN_UNUSED_RESULT;

/**
 * @brief Closes a file. Wrapper around fclose.
 */
void        FS_fclose(FILE* file);

/**
 * @brief Returns the size of the file in bytes. Returns -1 on error.
 */
int64_t     FS_file_size(const char* path);

/** \} */

/* -------------------------------------------------------------------- */
/** \name Path Manipulation
 * \{ */

/**
 * @brief Joins two paths dealing with separators.
 */
std::string FS_path_join(const std::string& dir,
                         const std::string& file);

/**
 * @brief Returns the directory name of a path.
 */
std::string FS_path_dirname(const std::string& path);

/**
 * @brief Returns the basename of a path (filename).
 */
std::string FS_path_basename(const std::string& path);

/** \} */

} // namespace file_system
} // namespace vektor
