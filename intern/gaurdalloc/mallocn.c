#include "mallocn_intern.hh"

void (*MEM_set_error_callback)(void (*func)(const char *)) =
    MEM_lockfree_set_error_callback;
