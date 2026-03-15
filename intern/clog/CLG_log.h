#pragma once

#include <cstdint>
#include <cstdio>
#include <mutex>
#include <pthread.h>
#include <sys/types.h>

#define CLOG_BUF_LEN_INIT 512

#define STREQ(a, b) (strcmp(a, b) == 0)
#define STREQLEN(a, b, n) (strncmp(a, b, n) == 0)

#ifdef _WIN32
#  define PATHSEP_CHAR '\\'
#else
#  define PATHSEP_CHAR '/'
#endif

#define _CLOG_ATTR_NONNULL(args...) __attribute__((nonnull(args)))

#define _CLOG_ATTR_PRINTF_FORMAT(format_param, dots_param) \
  __attribute__((format(printf, format_param, dots_param)))

namespace clog {

enum CLG_Level {
  /* Similar to assert. This logs the message, then a stack trace and abort. */
  CLG_LEVEL_FATAL = 0,
  /* An error we can recover from, should not happen. */
  CLG_LEVEL_ERROR,
  /* General warnings (which aren't necessary to show to users). */
  CLG_LEVEL_WARN,
  /* Information about devices, files, configuration, user operations. */
  CLG_LEVEL_INFO,
  /* Debugging information for developers. */
  CLG_LEVEL_DEBUG,
  /* Very verbose code execution tracing. */
  CLG_LEVEL_TRACE,
};

struct CLG_LogType {
  struct CLG_LogType *next;
  char identifier[64];
  /** FILE output. */
  struct CLogContext *clg_ctx;
  /** Control behavior. */
  CLG_Level level;
};

struct CLG_LogRef {
  explicit CLG_LogRef(const char *identifier);
  const char *identifier;
  CLG_LogType *type;
  struct CLG_LogRef *next;
};

static std::mutex LOG_MUTEX;

struct CLG_IDFilter {
  struct CLG_IDFilter *next;
  /** Over alloc. */
  char match[0];
};

struct CLogStringBuf {
  char *data;
  uint len;
  uint len_alloc;
  bool is_alloc;
};

enum eCLogColor {
  COLOR_DEFAULT,
  COLOR_RED,
  COLOR_GREEN,
  COLOR_YELLOW,
  COLOR_DIM,

  COLOR_RESET,
};
#define COLOR_LEN (COLOR_RESET + 1)
static const char *clg_color_table[COLOR_LEN] = {nullptr};

struct CLogContext {
  /** Single linked list of types. */
  CLG_LogType *types;

  pthread_mutex_t types_lock;

  /* exclude, include filters. */
  CLG_IDFilter *filters[2];
  bool use_color;
  bool use_source;
  bool use_basename;
  bool use_timestamp;
  bool use_memory;

  /** Borrowed, not owned. */
  int output;
  FILE *output_file;

  /** For dual output support (e.g. console + file). */
  int output_extra;
  FILE *output_file_extra;

  /** For timer (use_timestamp). */
  uint64_t timestamp_tick_start;

  /** For new types. */
  struct {
    CLG_Level level;
  } default_type;

  struct {
    void (*error_fn)(void *file_handle);
    void (*fatal_fn)(void *file_handle);
    void (*backtrace_fn)(void *file_handle);
  } callbacks;
};

static struct CLogContext *g_ctx = nullptr;
static bool g_quiet = false;

static void clg_str_init(CLogStringBuf *cstr, char *buf_stack, uint buf_stack_len);

void CLG_log_str(const CLG_LogType *type,
                 enum CLG_Level level,
                 const char *file_line,
                 const char *fn,
                 const char *message) _CLOG_ATTR_NONNULL(1, 3, 4, 5);

void CLG_logf(const CLG_LogType *type,
              enum CLG_Level level,
              const char *file_line,
              const char *fn,
              const char *format,
              ...) _CLOG_ATTR_NONNULL(1, 3, 4, 5) _CLOG_ATTR_PRINTF_FORMAT(5, 6);

void CLG_log_raw(const CLG_LogType *lg, const char *message);

void CLG_init();
void CLG_exit();

void CLG_output_set(void *file_handle);
void CLG_output_extra_set(void *file_handle);
void CLG_output_use_source_set(int value);
void CLG_output_use_basename_set(int value);
void CLG_output_use_timestamp_set(int value);
void CLG_output_use_memory_set(int value);
void CLG_error_fn_set(void (*error_fn)(void *file_handle));
void CLG_fatal_fn_set(void (*fatal_fn)(void *file_handle));
void CLG_backtrace_fn_set(void (*fatal_fn)(void *file_handle));
void CLG_type_filter_exclude(const char *type_match, int type_match_len);
void CLG_type_filter_include(const char *type_match, int type_match_len);
void CLG_level_set(CLG_Level level);

void CLG_quiet_set(bool quiet);
bool CLG_quiet_get();

void CLG_logref_init(CLG_LogRef *clg_ref);
void CLG_logref_register(CLG_LogRef *clg_ref);
void CLG_logref_list_all(void (*callback)(const char *identifier, void *user_data),
                         void *user_data);

static void clg_color_table_init(bool use_color);

static CLG_LogRef **clg_all_refs_p();
static void clg_str_free(CLogStringBuf *cstr);

static const char *clg_level_as_text(enum CLG_Level level);
static enum eCLogColor clg_level_to_color(enum CLG_Level level);
int CLG_color_support_get(CLG_LogRef *clg_ref);

static CLG_LogType *clg_ctx_type_find_by_name(CLogContext *ctx, const char *identifier);
static CLG_LogType *clg_ctx_type_register(CLogContext *ctx, const char *identifier);

static void clg_ctx_error_action(CLogContext *clg_ctx);
static void clg_ctx_fatal_action(CLogContext *clg_ctx);
static void clg_ctx_backtrace(CLogContext *clg_ctx);
static uint64_t clg_timestamp_ticks_get();

static void clg_str_reserve(CLogStringBuf *cstr, uint len);
static void clg_str_append_with_len(CLogStringBuf *cstr, const char *str, uint len);
static void write_timestamp(CLogStringBuf *cstr, uint64_t timestamp_tick_start);

static void clg_str_append(CLogStringBuf *cstr, const char *str);

static void clg_str_append_char(CLogStringBuf *cstr, char c, uint len);
static void write_memory(CLogStringBuf *cstr);

static void write_level(CLogStringBuf *cstr, enum CLG_Level level, bool use_color);
static void write_type(CLogStringBuf *cstr, const CLG_LogType *lg);

static void write_file_line_fn(
    CLogStringBuf *cstr, const char *file_line, const char *fn, bool use_basename, bool use_color);

static void clg_str_indent_multiline(CLogStringBuf *cstr, uint indent_len);

static void clg_str_vappendf(CLogStringBuf *cstr, const char *format, va_list args);

static void CLG_ctx_output_set(CLogContext *ctx, void *file_handle);

static void CLG_ctx_output_use_source_set(CLogContext *ctx, int value);
static void CLG_ctx_output_use_basename_set(CLogContext *ctx, int value);
static void CLG_ctx_output_use_timestamp_set(CLogContext *ctx, int value);
static void CLG_ctx_output_use_memory_set(CLogContext *ctx, int value);
static void CLT_ctx_error_fn_set(CLogContext *ctx, void (*error_fn)(void *file_handle));
static void CLG_ctx_fatal_fn_set(CLogContext *ctx, void (*fatal_fn)(void *file_handle));
static void CLG_ctx_backtrace_fn_set(CLogContext *ctx, void (*backtrace_fn)(void *file_handle));

static CLogContext *CLG_ctx_init();

inline CLG_LogRef::CLG_LogRef(const char *identifier)
    : identifier(identifier), type(nullptr), next(nullptr)
{
  clog::CLG_logref_register(this);
}

}  // namespace clog

#define CLO_STRINGIFY_ARG(x) #x
#define CLO_STRINGIFY(x) CLO_STRINGIFY_ARG(x)

#define CLG_LOGREF_DECLARE_GLOBAL(var, id) \
  static clog::CLG_LogRef _static_##var(id); \
  clog::CLG_LogRef *(var) = &_static_##var

#define CLG_LOGREF_DECLARE_EXTERN(var) extern clog::CLG_LogRef(*(var))

#define CLOG_ENSURE(clg_ref) \
  (((clg_ref))->type ? ((clg_ref))->type : (clog::CLG_logref_init(clg_ref), ((clg_ref))->type))

#define CLOG_CHECK(clg_ref, verbose_level, ...) \
  ((void)CLOG_ENSURE(clg_ref), (((clg_ref))->type->level >= (verbose_level)))

#define CLOG_AT_LEVEL(clg_ref, verbose_level, ...) \
  do { \
    const clog::CLG_LogType *_lg_ty = CLOG_ENSURE(clg_ref); \
    if (_lg_ty->level >= (verbose_level)) { \
      clog::CLG_logf( \
          _lg_ty, (verbose_level), __FILE__ ":" CLO_STRINGIFY(__LINE__), __func__, __VA_ARGS__); \
    } \
  } while (0)

#define CLOG_STR_AT_LEVEL(clg_ref, verbose_level, str) \
  do { \
    const clog::CLG_LogType *_lg_ty = CLOG_ENSURE(clg_ref); \
    if (_lg_ty->level >= (verbose_level)) { \
      clog::CLG_log_str( \
          _lg_ty, (verbose_level), __FILE__ ":" CLO_STRINGIFY(__LINE__), __func__, (str)); \
    } \
  } while (0)

#define CLOG_FATAL(clg_ref, ...) CLOG_AT_LEVEL(clg_ref, clog::CLG_LEVEL_FATAL, __VA_ARGS__)
#define CLOG_ERROR(clg_ref, ...) CLOG_AT_LEVEL(clg_ref, clog::CLG_LEVEL_ERROR, __VA_ARGS__)
#define CLOG_WARN(clg_ref, ...) CLOG_AT_LEVEL(clg_ref, clog::CLG_LEVEL_WARN, __VA_ARGS__)
#define CLOG_INFO(clg_ref, ...) CLOG_AT_LEVEL(clg_ref, clog::CLG_LEVEL_INFO, __VA_ARGS__)
#define CLOG_DEBUG(clg_ref, ...) CLOG_AT_LEVEL(clg_ref, clog::CLG_LEVEL_DEBUG, __VA_ARGS__)
#define CLOG_TRACE(clg_ref, ...) CLOG_AT_LEVEL(clg_ref, clog::CLG_LEVEL_TRACE, __VA_ARGS__)

/* Log single string. */
#define CLOG_STR_FATAL(clg_ref, str) CLOG_STR_AT_LEVEL(clg_ref, clog::CLG_LEVEL_FATAL, str)
#define CLOG_STR_ERROR(clg_ref, str) CLOG_STR_AT_LEVEL(clg_ref, clog::CLG_LEVEL_ERROR, str)
#define CLOG_STR_WARN(clg_ref, str) CLOG_STR_AT_LEVEL(clg_ref, clog::CLG_LEVEL_WARN, str)
#define CLOG_STR_INFO(clg_ref, str) CLOG_STR_AT_LEVEL(clg_ref, clog::CLG_LEVEL_INFO, str)
#define CLOG_STR_DEBUG(clg_ref, str) CLOG_STR_AT_LEVEL(clg_ref, clog::CLG_LEVEL_DEBUG, str)
#define CLOG_STR_TRACE(clg_ref, str) CLOG_STR_AT_LEVEL(clg_ref, clog::CLG_LEVEL_TRACE, str)
