#pragma once

#define _CLOG_ATTR_NONNULL(...)
#define _CLOG_ATTR_PRINTF_FORMAT(format_param, dots_param)

#define STRINGIFY_ARG(x) "" #x
#define STRINGIFY_APPEND(a, b) "" a #b
#define STRINGIFY(x) STRINGIFY_APPEND("", x)

#include <stdio.h>

#define SNPRINTF_UTF8(buff, fmt, ...) snprintf(buff, sizeof(buff), fmt, ##__VA_ARGS__)

#define STREQ(a, b) (strcmp(a, b) == 0)
#define STREQLEN(a, b, n) (strncmp(a, b, n) == 0)

#define _CLOG_ATTR_ALWAYS_INLINE

#include <stdint.h>
#include <stdio.h>
#include <sys/types.h>

enum CLG_Level
{
    CLG_LEVEL_FATAL = 0,
    CLG_LEVEL_ERROR = 1,
    CLG_LEVEL_WARN  = 2,
    CLG_LEVEL_INFO  = 3,
    CLG_LEVEL_DEBUG = 4,
    CLG_LEVEL_TRACE = 5,
};
#define CLG_LEVEL_LEN (CLG_LEVEL_TRACE + 1)

struct CLG_IDFilter
{
    struct CLG_IDFilter* next;
    char                 match;
};

#define WITH_CLOG_PTHREADS

struct CLogContext
{
    struct CLG_LogType* types;
#ifdef WITH_CLOG_PTHREADS
    pthread_mutex_t types_lock;
#endif

    struct CLG_IDFilter* filters[2];
    int                  use_color;
    int                  use_source;
    int                  use_basename;
    int                  use_timestamp;
    int                  use_memory;

    int                  output;
    FILE*                output_file;
    FILE*                log_file;

    uint64_t             timestamp_tick_start;

    struct
    {
        enum CLG_Level level;
    } default_type;

    struct
    {
        void (*error_fn)(void* file_handle);
        void (*fatal_fn)(void* file_handle);
        void (*backtrace_fn)(void* file_handle);
    } callbacks;
};

struct CLG_LogType
{
    struct CLG_LogType* next;
    char                identifier[64];
    struct CLogContext* ctx;
    enum CLG_Level      level;
};

struct CLG_LogRef
{
    CLG_LogRef(const char* identifier);

    const char*         identifier;
    struct CLG_LogType* type;
    struct CLG_LogRef*  next;
};

void CLG_level_set(CLG_Level level);

void CLG_logref_init(CLG_LogRef* clg_ref);

void CLG_logref_register(CLG_LogRef* clg_ref);
void CLG_logref_list_all(void (*callback)(const char* identifier,
                                          void*       user_data),
                         void* user_data);

int  CLG_color_support_get(CLG_LogRef* clg_ref);

void CLG_quiet_set(int quiet);
bool CLG_quiet_get();

inline CLG_LogRef::CLG_LogRef(const char* identifier)
    : identifier(identifier)
    , type(nullptr)
    , next(nullptr)
{
    CLG_logref_register(this);
}

void CLG_output_set(void* file_handle);
void CLG_output_use_source_set(int value);
void CLG_output_use_basename_set(int value);
void CLG_output_use_timestamp_set(int value);
void CLG_output_use_memory_set(int value);
void CLG_error_fn_set(void (*error_fn)(void* file_handle));
void CLG_fatal_fn_set(void (*fatal_fn)(void* file_handle));
void CLG_backtrace_fn_set(void (*fatal_fn)(void* file_handle));

void CLG_type_filter_include(const char* type_match,
                             int         type_match_len);
void CLG_type_filter_exclude(const char* type_match,
                             int         type_match_len);

void CLG_log_str(const struct CLG_LogType* lg,
                 enum CLG_Level            level,
                 const char*               file_line,
                 const char*               fn,
                 const char*               message) _CLOG_ATTR_NONNULL(1,
                                                         3,
                                                         4,
                                                         5);
void CLG_logf(const struct CLG_LogType* lg,
              enum CLG_Level            level,
              const char*               file_line,
              const char*               fn,
              const char*               format,
              ...) _CLOG_ATTR_NONNULL(1,
                                      3,
                                      4,
                                      5) _CLOG_ATTR_PRINTF_FORMAT(5,
                                                                  6);
void CLG_log_raw(const struct CLG_LogType* lg,
                 const char*               message);

#define CLG_LOGREF_DECLARE_GLOBAL(var, id)                                                                             \
    static CLG_LogRef _static_##var = {id};                                                                            \
    CLG_LogRef*       var           = &_static_##var

#define CLOG_ENSURE(clg_ref) ((clg_ref)->type ? (clg_ref)->type : (CLG_logref_init(clg_ref), (clg_ref)->type))

#define CLOG_CHECK(clg_ref, verbose_level, ...) ((void)CLOG_ENSURE(clg_ref), ((clg_ref)->type->level >= verbose_level))

#define CLOG_AT_LEVEL(clg_ref, verbose_level, ...)                                                                     \
    {                                                                                                                  \
        const CLG_LogType* _lg_ty = CLOG_ENSURE(clg_ref);                                                              \
        if (_lg_ty->level >= verbose_level)                                                                            \
        {                                                                                                              \
            CLG_logf(_lg_ty, verbose_level, __FILE__ ":" STRINGIFY(__LINE__), __func__, __VA_ARGS__);                  \
        }                                                                                                              \
    }                                                                                                                  \
    ((void)0)

#define CLOG_AT_LEVEL_NOCHECK(clg_ref, verbose_level, ...)                                                             \
    {                                                                                                                  \
        const CLG_LogType* _lg_ty = CLOG_ENSURE(clg_ref);                                                              \
        if (!CLG_quiet_get() || _lg_ty->level >= verbose_level)                                                        \
        {                                                                                                              \
            CLG_logf(_lg_ty, verbose_level, __FILE__ ":" STRINGIFY(__LINE__), __func__, __VA_ARGS__);                  \
        }                                                                                                              \
    }                                                                                                                  \
    ((void)0)

#define CLOG_STR_AT_LEVEL(clg_ref, verbose_level, str)                                                                 \
    {                                                                                                                  \
        const CLG_LogType* _lg_ty = CLOG_ENSURE(clg_ref);                                                              \
        if (_lg_ty->level >= verbose_level)                                                                            \
        {                                                                                                              \
            CLG_log_str(_lg_ty, verbose_level, __FILE__ ":" STRINGIFY(__LINE__), __func__, str);                       \
        }                                                                                                              \
    }                                                                                                                  \
    ((void)0)

#define CLOG_STR_AT_LEVEL_NOCHECK(clg_ref, verbose_level, str)                                                         \
    {                                                                                                                  \
        const CLG_LogType* _lg_ty = CLOG_ENSURE(clg_ref);                                                              \
        if (!CLG_quiet_get() || _lg_ty->level >= verbose_level)                                                        \
        {                                                                                                              \
            CLG_log_str(_lg_ty, verbose_level, __FILE__ ":" STRINGIFY(__LINE__), __func__, str);                       \
        }                                                                                                              \
    }                                                                                                                  \
    ((void)0)

#define CLOG_FATAL(clg_ref, ...) CLOG_AT_LEVEL(clg_ref, CLG_LEVEL_FATAL, __VA_ARGS__)
#define CLOG_ERROR(clg_ref, ...) CLOG_AT_LEVEL(clg_ref, CLG_LEVEL_ERROR, __VA_ARGS__)
#define CLOG_WARN(clg_ref, ...) CLOG_AT_LEVEL(clg_ref, CLG_LEVEL_WARN, __VA_ARGS__)
#define CLOG_INFO(clg_ref, ...) CLOG_AT_LEVEL(clg_ref, CLG_LEVEL_INFO, __VA_ARGS__)
#define CLOG_DEBUG(clg_ref, ...) CLOG_AT_LEVEL(clg_ref, CLG_LEVEL_DEBUG, __VA_ARGS__)
#define CLOG_TRACE(clg_ref, ...) CLOG_AT_LEVEL(clg_ref, CLG_LEVEL_TRACE, __VA_ARGS__)
#define CLOG_ASSERT(clg_ref, cond, ...) CLOG_AT_LEVEL(clg_ref, CLG_LEVEL_FATAL, __VA_ARGS__)

#define CLOG_STR_FATAL(clg_ref, str) CLOG_STR_AT_LEVEL(clg_ref, CLG_LEVEL_FATAL, str)
#define CLOG_STR_ERROR(clg_ref, str) CLOG_STR_AT_LEVEL(clg_ref, CLG_LEVEL_ERROR, str)
#define CLOG_STR_WARN(clg_ref, str) CLOG_STR_AT_LEVEL(clg_ref, CLG_LEVEL_WARN, str)
#define CLOG_STR_INFO(clg_ref, str) CLOG_STR_AT_LEVEL(clg_ref, CLG_LEVEL_INFO, str)
#define CLOG_STR_DEBUG(clg_ref, str) CLOG_STR_AT_LEVEL(clg_ref, CLG_LEVEL_DEBUG, str)
#define CLOG_STR_TRACE(clg_ref, str) CLOG_STR_AT_LEVEL(clg_ref, CLG_LEVEL_TRACE, str)

#define CLOG_INFO_NOCHECK(clg_ref, format, ...) CLOG_AT_LEVEL_NOCHECK(clg_ref, CLG_LEVEL_INFO, format, __VA_ARGS__)
#define CLOG_STR_INFO_NOCHECK(clg_ref, str) CLOG_STR_AT_LEVEL_NOCHECK(clg_ref, CLG_LEVEL_INFO, str)
