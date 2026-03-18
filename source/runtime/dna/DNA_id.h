#pragma once

#include <cstdint>

namespace vektor::dna {

struct ID {
  char name[/*MAX_ID_NAME*/ 258] = "";
  uint64_t session_id;  ///< Run-time unique ID for synchronization
  int type;            ///< IDType (Mesh, Object, Material, etc.)
};

struct IDPropertyUIData {
  const char *description = nullptr;
  int rna_subtype = 0;
  const char _pad[4] = {};
};

struct IDPropertyUIDataEnumItem {
  /* Unique identifier, used for string lookup. */
  const char *identifier = nullptr;
  /* UI name of the item. */
  const char *name = nullptr;
  /*  description. */
  const char *description = nullptr;
  /* Unique integer value, should never change. */
  int value = 0;
  /* Optional icon. */
  int icon = 0;
};

}  // namespace vektor::dna
