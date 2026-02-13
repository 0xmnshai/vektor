
#include <stddef.h>
namespace vektor
{
void* mem_lockfree_malloc_arrayN_aligned(const size_t len,
                                         const size_t size,
                                         const size_t alignment,
                                         const char*  str,
                                         size_t&      r_bytes_num);
}