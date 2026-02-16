#include "VKE_string_utf8.h"
#include "../../intern/gaurdalloc/mallocn_intern.hh"
#include "../dna/DNA_id.h"
#include "VKE_assert.h"
#include "VKE_compiler_compat.h"

namespace vektor
{

VKE_INLINE int utf8_char_compute_skip(const char c)
{
    if (UNLIKELY(c >= 192))
    {
        if ((c & 0xe0) == 0xc0)
        {
            return 2;
        }
        if ((c & 0xf0) == 0xe0)
        {
            return 3;
        }
        if ((c & 0xf8) == 0xf0)
        {
            return 4;
        }
        if ((c & 0xfc) == 0xf8)
        {
            return 5;
        }
        if ((c & 0xfe) == 0xfc)
        {
            return 6;
        }
    }
    return 1;
}

ptrdiff_t VKE_str_utf8_invalid_byte(const char* str,
                                    size_t      str_len)
{
    /* NOTE(@ideasman42): from libswish3, originally called `u8_isvalid()`,
     * modified to return the index of the bad character (byte index not UTF).
     * http://svn.swish-e.org/libswish3/trunk/src/libswish3/utf8.c r3044.
     *
     * Comment from code in: `libswish3`.
     * Based on the `valid_utf8` routine from the PCRE library by Philip Hazel
     *
     * length is in bytes, since without knowing whether the string is valid
     * it's hard to know how many characters there are! */

    const char *p, *perr, *pend = reinterpret_cast<const char*>(str) + str_len;
    char        c;
    int         ab;

    for (p = reinterpret_cast<const char*>(str); p < pend; p++, str_len--)
    {
        c    = *p;
        perr = p; /* Erroneous char is always the first of an invalid UTF8 sequence... */
        if (ELEM(c, 0xfe, 0xff, 0x00))
        {
            /* Those three values are not allowed in UTF8 string. */
            goto utf8_error;
        }
        if (c < 128)
        {
            continue;
        }
        if ((c & 0xc0) != 0xc0)
        {
            goto utf8_error;
        }

        /* Note that since we always increase p (and decrease length) by one byte in main loop,
         * we only add/subtract extra UTF8 bytes in code below
         * (ab number, aka number of bytes remaining in the UTF8 sequence after the initial one). */
        ab = utf8_char_compute_skip(c) - 1;
        if (str_len <= size_t(ab))
        {
            goto utf8_error;
        }

        /* Check top bits in the second byte */
        p++;
        str_len--;
        if ((*p & 0xc0) != 0x80)
        {
            goto utf8_error;
        }

        /* Check for overlong sequences for each different length */
        switch (ab)
        {
            case 1:
                /* Check for: `XX00 000X`. */
                if ((c & 0x3e) == 0)
                {
                    goto utf8_error;
                }
                continue; /* We know there aren't any more bytes to check */

            case 2:
                /* Check for: `1110 0000, XX0X XXXX`. */
                if (c == 0xe0 && (*p & 0x20) == 0)
                {
                    goto utf8_error;
                }
                /* Some special cases, see section 5 of UTF8 decoder stress-test by Markus Kuhn
                 * (https://www.cl.cam.ac.uk/~mgk25/ucs/examples/UTF-8-test.txt). */
                /* From section 5.1 (and 5.2) */
                if (c == 0xed)
                {
                    if (*p == 0xa0 && *(p + 1) == 0x80)
                    {
                        goto utf8_error;
                    }
                    if (*p == 0xad && *(p + 1) == 0xbf)
                    {
                        goto utf8_error;
                    }
                    if (*p == 0xae && *(p + 1) == 0x80)
                    {
                        goto utf8_error;
                    }
                    if (*p == 0xaf && *(p + 1) == 0xbf)
                    {
                        goto utf8_error;
                    }
                    if (*p == 0xb0 && *(p + 1) == 0x80)
                    {
                        goto utf8_error;
                    }
                    if (*p == 0xbe && *(p + 1) == 0x80)
                    {
                        goto utf8_error;
                    }
                    if (*p == 0xbf && *(p + 1) == 0xbf)
                    {
                        goto utf8_error;
                    }
                }
                /* From section 5.3 */
                if (c == 0xef)
                {
                    if (*p == 0xbf && *(p + 1) == 0xbe)
                    {
                        goto utf8_error;
                    }
                    if (*p == 0xbf && *(p + 1) == 0xbf)
                    {
                        goto utf8_error;
                    }
                }
                break;

            case 3:
                /* Check for: `1111 0000, XX00 XXXX`. */
                if (c == 0xf0 && (*p & 0x30) == 0)
                {
                    goto utf8_error;
                }
                break;

            case 4:
                /* Check for `1111 1000, XX00 0XXX`. */
                if (c == 0xf8 && (*p & 0x38) == 0)
                {
                    goto utf8_error;
                }
                break;

            case 5:
                /* Check for: `1111 1100, XX00 00XX`. */
                if (c == 0xfc && (*p & 0x3c) == 0)
                {
                    goto utf8_error;
                }
                break;
        }

        /* Check for valid bytes after the 2nd, if any; all must start 10. */
        while (--ab > 0)
        {
            p++;
            str_len--;
            if ((*p & 0xc0) != 0x80)
            {
                goto utf8_error;
            }
        }
    }

    return -1;

utf8_error:

    return (reinterpret_cast<const char*>(perr) - static_cast<const char*>(str));
}

int VKE_str_utf8_invalid_substitute(char*      str,
                                    size_t     str_len,
                                    const char substitute)
{
    VKE_assert(substitute);
    ptrdiff_t bad_char;
    int       tot = 0;

    VKE_assert(str[str_len] == '\0');

    while ((bad_char = VKE_str_utf8_invalid_byte(str, str_len)) != -1)
    {
        str[bad_char] = substitute;
        bad_char += 1;
        str += bad_char;
        str_len -= size_t(bad_char);
        tot++;
    }

    return tot;
}

const char* VKE_str_utf8_invalid_substitute_if_needed(const char*  str,
                                                      size_t       str_len,
                                                      const char   substitute,
                                                      char*        buf,
                                                      const size_t buf_maxncpy)
{

    VKE_assert(str[str_len] == '\0');
    VKE_assert(buf[0] != '\0');

    const ptrdiff_t bad_char = VKE_str_utf8_invalid_byte(str, str_len);

    if (LIKELY(bad_char == -1))
    {
        return str;
    }
    VKE_assert(bad_char >= 0);

    /* In the case a bad character is outside the buffer limit,
     * simply perform a truncating UTF8 copy into the buffer and return that. */
    if (UNLIKELY(size_t(bad_char) >= buf_maxncpy))
    {
        VKE_strncpy_utf8(buf, str, buf_maxncpy);
        return buf;
    }

    size_t buf_len;
    if (str_len < buf_maxncpy)
    {
        memcpy(buf, str, str_len + 1);
        buf_len = str_len;
    }
    else
    {
        buf_len = VKE_strncpy_utf8_rlen(buf, str, buf_maxncpy);
    }

    /* Skip the good characters. */
    VKE_str_utf8_invalid_substitute(buf + bad_char, buf_len - size_t(bad_char), substitute);
    return buf;
};
} // namespace vektor