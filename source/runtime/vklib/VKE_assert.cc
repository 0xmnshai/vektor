#include "VKE_assert.h"
#include <cstdio>
#include <cstdlib>

void _VKE_assert_print_pos(const char *file, int line, const char *function,
                           const char *id) {
  fprintf(stderr, "%s: %s:%d: %s\n", id, file, line, function);
}

void _VKE_assert_print_extra(const char *str) {
  fprintf(stderr, "Extra info: %s\n", str);
}

void _VKE_assert_print_backtrace(void) {

  fprintf(stderr, "Backtrace not implemented.\n");
}

void _VKE_assert_abort(void) { abort(); }
