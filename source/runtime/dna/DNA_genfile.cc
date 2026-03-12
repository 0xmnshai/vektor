#include "DNA_genfile.h"
#include <cstring>

namespace vektor::dna {

SDNA *DNA_sdna_from_data(const void *data, int32_t len, bool /*endian_swapped*/)
{
  if (!data || len < 8)
    return nullptr;

  const char *cp = (const char *)data;
  if (strncmp(cp, "SDNA", 4) != 0)
    return nullptr;
  cp += 4;

  if (strncmp(cp, "NAME", 4) != 0)
    return nullptr;
  cp += 4;

  SDNA *sdna = new SDNA();

  sdna->names.emplace_back("location");
  sdna->types.push_back({"float", 4});

  DNAStruct transform;
  transform.type_index = 0;             // index of "float" (obviously wrong, but for demo)
  transform.members.push_back({0, 0});  // type 0, name 0
  sdna->structs.push_back(transform);

  return sdna;
}

void DNA_sdna_free(SDNA *sdna)
{
  delete sdna;
}

void *DNA_struct_reconstruct(const SDNA * /*filesdna*/,
                             const SDNA * /*memsdna*/,
                             int32_t /*struct_index*/,
                             int32_t nr,
                             const void *data)
{
  // Simplified: binary copy if DNA matches.
  // In reality, this would perform field-by-field mapping.
  int size = 16;  // Dummy size
  void *new_data = malloc(size * nr);
  memcpy(new_data, data, size * nr);
  return new_data;
}

}  // namespace vektor::dna
