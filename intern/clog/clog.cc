
#include <time.h>
#include <unistd.h>

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

/** Action on error level. */
static void CLT_ctx_error_fn_set(CLogContext* ctx,
                                 void (*error_fn)(void* file_handle))
{
    ctx->callbacks.error_fn = error_fn;
}

/** Action on fatal level. */
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
