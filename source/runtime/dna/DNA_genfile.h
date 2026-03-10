/* SPDX-FileCopyrightText: 2026 Vektor Engine. All rights reserved.
 *
 * SPDX-License-Identifier: GPL-2.0-or-later */

/** \file
 * \ingroup dna
 * DNA system for Vektor, providing struct metadata and versioning support.
 */

#pragma once

#include <cstdint>
#include <vector>
#include <string>

namespace vektor::dna {

/**
 * DNA data-type.
 */
struct DNAType {
  std::string name;
  uint16_t size;
};

/**
 * DNA struct member.
 */
struct DNAMember {
  uint16_t type_index;
  uint16_t name_index;
};

/**
 * DNA struct.
 */
struct DNAStruct {
  uint16_t type_index;
  std::vector<DNAMember> members;
};

/**
 * The full DNA structure of a file.
 */
struct SDNA {
  std::vector<std::string> names;
  std::vector<DNAType> types;
  std::vector<DNAStruct> structs;
  
  /** Map from struct type index to struct index in 'structs' vector. */
  std::vector<int32_t> struct_map;
};

/**
 * Decode SDNA from raw bytes (Blender's DNA1 block).
 */
SDNA *DNA_sdna_from_data(const void *data, int32_t len, bool endian_swapped);

/**
 * Free SDNA.
 */
void DNA_sdna_free(SDNA *sdna);

/**
 * Reconstruct a struct from the file's DNA to the memory's DNA.
 * (Simplified version of Blender's DNA_struct_reconstruct)
 */
void *DNA_struct_reconstruct(const SDNA *filesdna, 
                             const SDNA *memsdna, 
                             int32_t struct_index, 
                             int32_t nr, 
                             const void *data);

}  // namespace vektor::dna
