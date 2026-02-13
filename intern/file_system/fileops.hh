#pragma once

#include <cstdio>
#include <string>

namespace vektor
{
namespace file_system
{

class FileSystem
{
public:
    /**
     * @brief Creates a directory if it does not exist.
     *
     * @param path The path of the directory to create.
     * @return true If the directory was created or already exists.
     * @return false If the directory could not be created.
     */
    static bool        create_directory(const char* path);

    /**
     * @brief Opens a file with the given mode.
     *
     * @param path The path of the file to open.
     * @param mode The mode string (e.g., "w", "r", "a").
     * @return FILE* The file pointer, or nullptr on failure.
     */
    static FILE*       open_file(const char* path,
                                const char* mode);

    /**
     * @brief Checks if a file or directory exists at the given path.
     *
     * @param path The path to check.
     * @return true If the path exists.
     * @return false If the path does not exist.
     */
    static bool        exists(const char* path);

    /**
     * @brief Joins two paths together.
     *
     * @param info Path A.
     * @param path Path B.
     * @return std::string The joined path.
     */
    static std::string join_path(const std::string& info,
                                const std::string& path);
};

} // namespace file_system
} // namespace vektor
