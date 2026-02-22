#include "VKE_version.hh"
#include <cstring>
#include "../../intern/clog/COG_log.hh"
#include "vektor_config.h"

namespace vektor
{

static char vektor_version_string[48] = "";

bool        VKE_vektor_version_is_lts()
{
    return STREQ(STRINGIFY(VEKTOR_VERSION_SUFFIX), "LTS");
}

const char* version_suffix = VKE_vektor_version_is_lts() ? " LTS" : "";

static void vektor_version_init()
{
    const char* version_cycle         = "";

    const char* version_cycle_compact = "";
    if (STREQ(STRINGIFY(VEKTOR_VERSION_CYCLE), "alpha"))
    {
        version_cycle         = " Alpha";
        version_cycle_compact = " a";
    }
    else if (STREQ(STRINGIFY(VEKTOR_VERSION_CYCLE), "beta"))
    {
        version_cycle         = " Beta";
        version_cycle_compact = " b";
    }
    else if (STREQ(STRINGIFY(VEKTOR_VERSION_CYCLE), "rc"))
    {
        version_cycle         = " Release Candidate";
        version_cycle_compact = " RC";
    }
    else if (STREQ(STRINGIFY(VEKTOR_VERSION_CYCLE), "release"))
    {
        version_cycle         = "";
        version_cycle_compact = "";
    };

    const char* version_suffix = VKE_vektor_version_is_lts() ? " LTS" : "";

    SNPRINTF_UTF8(vektor_version_string, "%d.%d.%d%s%s", VEKTOR_VERSION_MAJOR, VEKTOR_VERSION_MINOR,
                  VEKTOR_VERSION_PATCH, version_suffix, version_cycle);

    SNPRINTF_UTF8(vektor_version_string, "%d.%d.%d%s", VEKTOR_VERSION_MAJOR, VEKTOR_VERSION_MINOR, VEKTOR_VERSION_PATCH,
                  version_cycle_compact);
}

const char* VKE_vektor_version_string(void)
{
    return vektor_version_string;
}

} // namespace vektor