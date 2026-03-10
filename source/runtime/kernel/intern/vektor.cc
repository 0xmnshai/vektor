#include "vektor.hh"
#include "vektor_version.h"
#include <cstdio>

static char vektor_version_string[64] = "";

const char *VKE_vektor_version_string()
{
  snprintf(vektor_version_string,
           sizeof(vektor_version_string),
           "%s %s (%d.%d.%d)",
           VEKTOR_VERSION_CYCLE,
           VEKTOR_VERSION,
           VEKTOR_VERSION_MAJOR,
           VEKTOR_VERSION_MINOR,
           VEKTOR_VERSION_PATCH);

  return vektor_version_string;
}
