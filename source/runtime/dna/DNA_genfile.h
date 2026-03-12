#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace vektor::dna {

struct DNAType {
  std::string name;
  uint16_t size;
};

struct DNAMember {
  uint16_t type_index;
  uint16_t name_index;
};

struct DNAStruct {
  uint16_t type_index;
  std::vector<DNAMember> members;
};

struct SDNA {
  std::vector<std::string> names;
  std::vector<DNAType> types;
  std::vector<DNAStruct> structs;

  /** Map from struct type index to struct index in 'structs' vector. */
  std::vector<int32_t> struct_map;
};

SDNA *DNA_sdna_from_data(const void *data, int32_t len, bool endian_swapped);

void DNA_sdna_free(SDNA *sdna);

void *DNA_struct_reconstruct(
    const SDNA *filesdna, const SDNA *memsdna, int32_t struct_index, int32_t nr, const void *data);

}  // namespace vektor::dna
