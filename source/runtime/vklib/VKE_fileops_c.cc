
#include <cstdlib>
#include <cstring>

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <cerrno>

#include <CoreFoundation/CoreFoundation.h>
#include <objc/message.h>
#include <objc/runtime.h>
#include <zlib.h>

#include "../vklib//VKE_assert.h"
#include "../vklib//VKE_path_utils.hh"
#include "../vklib//VKE_utildefines.h"

namespace vektor
{
namespace vklib
{
#define FILE_MAX_STATIC_BUF 256

FILE* VKE_fopen(const char* filepath,
                const char* mode)
{
    VKE_assert(!VKE_path_is_rel(filepath));

    return fopen(filepath, mode);
}

int64_t VKE_read(int    fd,
                 void*  buf,
                 size_t nbytes)
{

    int64_t nbytes_read_total = 0;
    while (true)
    {
        int64_t nbytes_read = read(fd, buf, nbytes);

        if (nbytes_read == nbytes)
        {
            return nbytes_read_total + nbytes_read;
        }
        if (nbytes_read == 0)
        {
            return nbytes_read_total;
        }
        if (nbytes_read < 0)
        {
            return nbytes_read;
        }

        if (UNLIKELY(nbytes_read > nbytes))
        {
            VKE_assert_unreachable();
            if (errno == 0)
            {
                errno = EIO;
            }
            return -1;
        }

        buf = static_cast<void*>((static_cast<char*>(buf)) + nbytes_read);
        nbytes_read_total += nbytes_read;
        nbytes -= nbytes_read;
    }
}
} // namespace vklib
} // namespace vektor