#pragma once

#include <stddef.h>

void _VKE_assert_print_pos(const char* file,
                           int         line,
                           const char* function,
                           const char* id);
void _VKE_assert_print_extra(const char* str);
void _VKE_assert_print_backtrace(void);
void _VKE_assert_abort(void);

#define VKE_assert_unreachable()                                                                                       \
    do                                                                                                                 \
    {                                                                                                                  \
        _VKE_assert_print_pos(__FILE__, __LINE__, __func__, "UNREACHABLE");                                            \
        _VKE_assert_print_extra("This should never happen");                                                           \
        _VKE_assert_print_backtrace();                                                                                 \
        _VKE_assert_abort();                                                                                           \
    } while (0)

#define VKE_assert(expr)                                                                                               \
    do                                                                                                                 \
    {                                                                                                                  \
        if (!(expr))                                                                                                   \
        {                                                                                                              \
            _VKE_assert_print_pos(__FILE__, __LINE__, __func__, "ASSERT");                                             \
            _VKE_assert_print_extra("Assertion failed");                                                               \
            _VKE_assert_print_backtrace();                                                                             \
            _VKE_assert_abort();                                                                                       \
        }                                                                                                              \
    } while (0)

#define VKE_assert_msg(expr, msg)                                                                                      \
    do                                                                                                                 \
    {                                                                                                                  \
        if (!(expr))                                                                                                   \
        {                                                                                                              \
            _VKE_assert_print_pos(__FILE__, __LINE__, __func__, "ASSERT");                                             \
            _VKE_assert_print_extra(msg);                                                                              \
            _VKE_assert_print_backtrace();                                                                             \
            _VKE_assert_abort();                                                                                       \
        }                                                                                                              \
    } while (0)

#define VKE_assert_msg_fmt(expr, fmt, ...)                                                                             \
    do                                                                                                                 \
    {                                                                                                                  \
        if (!(expr))                                                                                                   \
        {                                                                                                              \
            _VKE_assert_print_pos(__FILE__, __LINE__, __func__, "ASSERT");                                             \
            _VKE_assert_print_extra(fmt);                                                                              \
            _VKE_assert_print_backtrace();                                                                             \
            _VKE_assert_abort();                                                                                       \
        }                                                                                                              \
    } while (0)

#define BLI_assert_unreachable()                                                                                       \
    {                                                                                                                  \
        _VKE_assert_print_pos(__FILE__, __LINE__, __func__, "UNREACHABLE");                                            \
        _VKE_assert_print_extra("This should never happen");                                                           \
        _VKE_assert_print_backtrace();                                                                                 \
        _VKE_assert_abort();                                                                                           \
    }                                                                                                                  \
    ((void)0)
