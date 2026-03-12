#include <cstdlib>
#include <cstring>

#include "../DNA_defs.h"

namespace vektor::dna {

void DNA_internal_memcpy(void *dst, const void *src, const size_t size)
{
  memcpy(dst, src, size);
}

void DNA_internal_memzero(void *dst, const size_t size)
{
  memset(dst, 0, size);
}

void DNA_internal_swap(void *a, void *b, const size_t size)
{
  void *tmp = alloca(size);
  memcpy(tmp, a, size);
  memcpy(a, b, size);
  memcpy(b, tmp, size);
}
}  // namespace vektor::dna
