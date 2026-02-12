#pragma once

#if defined(__GNUC__) || defined(__clang__)
#define ATTR_WARN_UNUSED_RESULT __attribute__((warn_unused_result))
#define ATTR_NONNULL(...) __attribute__((nonnull(__VA_ARGS__)))
#else
#define ATTR_WARN_UNUSED_RESULT
#define ATTR_NONNULL(...)
#endif