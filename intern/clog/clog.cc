#include <time.h>
#include <unistd.h>
#include <cstdarg>
#include <cstdio>

#include "../file_system/fileops.hh"
#include "COG_log.hh"

#include "../libmv/threading.hh"

#include "../gaurdalloc/MEM_gaurdalloc.hh"

enum eCLogColor
{
    COLOR_DEFAULT,
    COLOR_RED,
    COLOR_GREEN,
    COLOR_YELLOW,
    COLOR_DIM,

    COLOR_RESET,
};
#define COLOR_LEN (COLOR_RESET + 1)
static bool                g_quiet                    = false;

static struct CLogContext* g_ctx                      = nullptr;

static const char*         clg_color_table[COLOR_LEN] = {nullptr};

static void                clg_color_table_init(bool use_color)
{
    for (int i = 0; i < COLOR_LEN; i++)
    {
        clg_color_table[i] = "";
    }
    if (use_color)
    {
        clg_color_table[COLOR_DEFAULT] = "\033[1;37m";
        clg_color_table[COLOR_RED]     = "\033[1;31m";
        clg_color_table[COLOR_GREEN]   = "\033[1;32m";
        clg_color_table[COLOR_YELLOW]  = "\033[1;33m";
        clg_color_table[COLOR_DIM]     = "\033[2;37m";
        clg_color_table[COLOR_RESET]   = "\033[0m";
    }
}

static void CLG_ctx_output_set(CLogContext* ctx,
                               void*        file_handle)
{
    ctx->output_file = static_cast<FILE*>(file_handle);
    ctx->output      = fileno(ctx->output_file);
    ctx->use_color   = isatty(ctx->output);
}

static CLG_LogRef** clg_all_refs_p()
{
    static CLG_LogRef* all_refs = nullptr;
    return &all_refs;
}

static void CLG_ctx_free(CLogContext* ctx)
{
    while (ctx->types != nullptr)
    {
        CLG_LogType* item = ctx->types;
        ctx->types        = item->next;
        MEM_delete(item);
    }

    for (CLG_LogRef* ref = *clg_all_refs_p(); ref; ref = ref->next)
    {
        ref->type = nullptr;
    }

    for (uint i = 0; i < 2; i++)
    {
        while (ctx->filters[i] != nullptr)
        {
            CLG_IDFilter* item = ctx->filters[i];
            ctx->filters[i]    = item->next;
            MEM_delete(item);
        }
    }
#ifdef WITH_CLOG_PTHREADS
    pthread_mutex_destroy(&ctx->types_lock);
#endif
    MEM_delete(ctx);
    if (ctx->log_file)
    {
        vektor::file_system::FS_fclose(ctx->log_file);
    }
}

void CLG_logref_register(CLG_LogRef* clg_ref)
{
    static libmv::mutex mutex;
    libmv::scoped_lock  lock(mutex);
    CLG_LogRef**        all_refs = clg_all_refs_p();
    clg_ref->next                = *all_refs;
    *all_refs                    = clg_ref;
}

static CLogContext* CLG_ctx_init()
{
    CLogContext* ctx = MEM_new_zeroed<CLogContext>(__func__);
#ifdef WITH_CLOG_PTHREADS
    pthread_mutex_init(&ctx->types_lock, nullptr);
#endif
    ctx->default_type.level = CLG_LEVEL_WARN;
    ctx->use_source         = true;
    CLG_ctx_output_set(ctx, stdout);

    return ctx;
}

void CLG_init()
{
    g_ctx = CLG_ctx_init();

    clg_color_table_init(g_ctx->use_color);

    using namespace vektor::file_system;
    if (FS_create_directory("vektor_logs"))
    {
        g_ctx->log_file = FS_fopen("vektor_logs/vektor.log", "a");
        if (g_ctx->log_file)
        {
            setvbuf(g_ctx->log_file, NULL, _IONBF, 0);
        }
    }
}

void CLG_exit()
{
    CLG_ctx_free(g_ctx);
}

void CLG_output_set(void* file_handle)
{
    CLG_ctx_output_set(g_ctx, file_handle);
}

static void CLG_ctx_output_use_source_set(CLogContext* ctx,
                                          int          value)
{
    ctx->use_source = (bool)value;
}

static void CLG_ctx_output_use_basename_set(CLogContext* ctx,
                                            int          value)
{
    ctx->use_basename = (bool)value;
}

static uint64_t clg_timestamp_ticks_get()
{
    uint64_t       tick;
    struct timeval tv;
    std::chrono::system_clock::now();
    tick = tv.tv_sec * 1000 + tv.tv_usec / 1000;
    return tick;
}

static void CLG_ctx_output_use_timestamp_set(CLogContext* ctx,
                                             int          value)
{
    ctx->use_timestamp = (bool)value;
    if (ctx->use_timestamp)
    {
        ctx->timestamp_tick_start = clg_timestamp_ticks_get();
    }
}

static void CLG_ctx_output_use_memory_set(CLogContext* ctx,
                                          int          value)
{
    ctx->use_memory = (bool)value;
}

static void CLT_ctx_error_fn_set(CLogContext* ctx,
                                 void (*error_fn)(void* file_handle))
{
    ctx->callbacks.error_fn = error_fn;
}

static void CLG_ctx_fatal_fn_set(CLogContext* ctx,
                                 void (*fatal_fn)(void* file_handle))
{
    ctx->callbacks.fatal_fn = fatal_fn;
}

static void CLG_ctx_backtrace_fn_set(CLogContext* ctx,
                                     void (*backtrace_fn)(void* file_handle))
{
    ctx->callbacks.backtrace_fn = backtrace_fn;
}

static void CLG_ctx_level_set(CLogContext* ctx,
                              CLG_Level    level)
{
    ctx->default_type.level = level;
    for (CLG_LogType* ty = ctx->types; ty; ty = ty->next)
    {
        ty->level = level;
    }
}

void CLG_output_use_source_set(int value)
{
    CLG_ctx_output_use_source_set(g_ctx, value);
}

void CLG_output_use_basename_set(int value)
{
    CLG_ctx_output_use_basename_set(g_ctx, value);
}

void CLG_output_use_timestamp_set(int value)
{
    CLG_ctx_output_use_timestamp_set(g_ctx, value);
}

void CLG_output_use_memory_set(int value)
{
    CLG_ctx_output_use_memory_set(g_ctx, value);
}

void CLG_error_fn_set(void (*error_fn)(void* file_handle))
{
    CLT_ctx_error_fn_set(g_ctx, error_fn);
}

void CLG_fatal_fn_set(void (*fatal_fn)(void* file_handle))
{
    CLG_ctx_fatal_fn_set(g_ctx, fatal_fn);
}

void CLG_backtrace_fn_set(void (*fatal_fn)(void* file_handle))
{
    CLG_ctx_backtrace_fn_set(g_ctx, fatal_fn);
}

void CLG_level_set(CLG_Level level)
{
    CLG_ctx_level_set(g_ctx, level);
}

void CLG_quiet_set(bool quiet)
{
    g_quiet = quiet;
}

bool CLG_quiet_get()
{
    return g_quiet;
}

void CLG_logref_init(CLG_LogRef* clg_ref)
{

    if (g_ctx == nullptr)
    {
        CLG_init();
    }

    CLG_LogType* new_type = MEM_new_zeroed<CLG_LogType>(__func__);
    strncpy(new_type->identifier, clg_ref->identifier, sizeof(new_type->identifier) - 1);
    new_type->ctx   = g_ctx;
    new_type->level = g_ctx->default_type.level;

#ifdef WITH_CLOG_PTHREADS
    pthread_mutex_lock(&g_ctx->types_lock);
#endif
    new_type->next = g_ctx->types;
    g_ctx->types   = new_type;
#ifdef WITH_CLOG_PTHREADS
    pthread_mutex_unlock(&g_ctx->types_lock);
#endif

    clg_ref->type = new_type;
}

void CLG_logf(const struct CLG_LogType* lg,
              enum CLG_Level            level,
              const char*               file_line,
              const char*               fn,
              const char*               format,
              ...)
{
    if (g_quiet && level > CLG_LEVEL_ERROR)
        return;

    va_list args;
    va_start(args, format);

    FILE* out = g_ctx ? g_ctx->output_file : stdout;
    if (!out)
        out = stdout;

    const char* level_str = "INFO";
    const char* color_str = "";
    const char* reset_str = "";

    if (g_ctx && g_ctx->use_color)
    {
        reset_str = "\033[0m";
    }

    switch (level)
    {
        case CLG_LEVEL_FATAL:
            level_str = "FATAL";
            if (g_ctx && g_ctx->use_color)
                color_str = "\033[1;31m";
            break;
        case CLG_LEVEL_ERROR:
            level_str = "ERROR";
            if (g_ctx && g_ctx->use_color)
                color_str = "\033[31m";
            break;
        case CLG_LEVEL_WARN:
            level_str = "WARN";
            if (g_ctx && g_ctx->use_color)
                color_str = "\033[33m";
            break;
        case CLG_LEVEL_INFO:
            level_str = "INFO";
            if (g_ctx && g_ctx->use_color)
                color_str = "\033[32m";
            break;
        case CLG_LEVEL_DEBUG:
            level_str = "DEBUG";
            break;
        case CLG_LEVEL_TRACE:
            level_str = "TRACE";
            break;
    }

    fprintf(out, "%s[%s] %s: ", color_str, level_str, lg ? lg->identifier : "Unknown");
    vfprintf(out, format, args);
    if (file_line && fn)
    {
        fprintf(out, " (%s %s)", file_line, fn);
    }
    fprintf(out, "%s\n", reset_str);

    if (g_ctx && g_ctx->log_file)
    {
        va_list args_copy;
        va_copy(args_copy, args);
        fprintf(g_ctx->log_file, "[%s] %s: ", level_str, lg ? lg->identifier : "Unknown");
        vfprintf(g_ctx->log_file, format, args_copy);
        if (file_line && fn)
        {
            fprintf(g_ctx->log_file, " (%s %s)", file_line, fn);
        }
        fprintf(g_ctx->log_file, "\n");
        fflush(g_ctx->log_file);
        fsync(fileno(g_ctx->log_file));
        va_end(args_copy);
    }

    va_end(args);

    if (level == CLG_LEVEL_FATAL)
    {
        if (g_ctx && g_ctx->callbacks.fatal_fn)
        {
            g_ctx->callbacks.fatal_fn(out);
        }
        abort();
    }
}

void CLG_log_str(const struct CLG_LogType* lg,
                 enum CLG_Level            level,
                 const char*               file_line,
                 const char*               fn,
                 const char*               message)
{
    CLG_logf(lg, level, file_line, fn, "%s", message);
}

void CLG_log_raw(const struct CLG_LogType* lg,
                 const char*               message)
{
    FILE* out = g_ctx ? g_ctx->output_file : stdout;
    if (!out)
        out = stdout;
    fprintf(out, "%s", message);
    if (g_ctx && g_ctx->log_file)
    {
        fprintf(g_ctx->log_file, "%s", message);
        fflush(g_ctx->log_file);
        fsync(fileno(g_ctx->log_file));
    }
}
