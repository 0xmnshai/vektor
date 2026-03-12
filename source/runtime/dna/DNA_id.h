#pragma once

namespace vektor::dna {

struct IDPropertyUIData {
  char *description = nullptr;
  int rna_subtype = 0;
  char _pad[4] = {};
};

struct IDPropertyUIDataEnumItem {
  /* Unique identifier, used for string lookup. */
  char *identifier = nullptr;
  /* UI name of the item. */
  char *name = nullptr;
  /*  description. */
  char *description = nullptr;
  /* Unique integer value, should never change. */
  int value = 0;
  /* Optional icon. */
  int icon = 0;
};

}  // namespace vektor::dna
