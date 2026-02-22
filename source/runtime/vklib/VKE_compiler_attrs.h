#pragma once

#if defined(__GNUC__) || defined(__clang__)
#define ATTR_WARN_UNUSED_RESULT __attribute__((warn_unused_result))
#define ATTR_NONNULL(...) __attribute__((nonnull(__VA_ARGS__)))
#define ATTR_PRINTF_FORMAT(format_param, dots_param) __attribute__((format(printf, format_param, dots_param)))
#else
#define ATTR_WARN_UNUSED_RESULT
#define ATTR_NONNULL(...)
#endif