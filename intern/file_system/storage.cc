#include <pwd.h>
#include <unistd.h>
#include <cstdlib>

#include "fileops.hh"

namespace vektor
{
const char* FS_dir_home()
{
    const char* home_dir;

    home_dir = getenv("HOME");
    if (home_dir == nullptr)
    {
        if (const passwd* pwuser = getpwuid(getuid()))
        {
            home_dir = pwuser->pw_dir;
        }
    }

    return home_dir;
}
} // namespace vektor
