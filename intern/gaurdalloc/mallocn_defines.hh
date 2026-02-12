#pragma once

#define ATTR_MALLOC __attribute__((malloc))
#define ATTR_WARN_UNUSED_RESULT __attribute__((warn_unused_result))
#define ATTR_ALLOC_SIZE(...) __attribute__((alloc_size(__VA_ARGS__)))
#define ATTR_NONNULL(...) __attribute__((nonnull(__VA_ARGS__)))
#define ATTR_MALLOC_LIKE __attribute__((malloc))
#define ATTR_MALLOC_SIZE(...) __attribute__((alloc_size(__VA_ARGS__)))
#define ATTR_NONNULL(...) __attribute__((nonnull(__VA_ARGS__)))
#define ATTR_WARN_UNUSED_RESULT __attribute__((warn_unused_result))
