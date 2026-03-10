#pragma once

#define ATTR_MALLOC __attribute__((malloc))
#define ATTR_WARN_UNUSED_RESULT __attribute__((warn_unused_result))
#ifndef ATTR_ALLOC_SIZE
#  define ATTR_ALLOC_SIZE(...) __attribute__((alloc_size(__VA_ARGS__)))
#endif
#define ATTR_NONNULL(a) __attribute__((nonnull(a)))