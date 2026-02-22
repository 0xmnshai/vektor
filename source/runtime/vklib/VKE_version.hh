#pragma once

#include <cstddef>

namespace vektor
{

#define VEKTOR_VERSION_SUFFIX

#define VEKTOR_VERSION 1
#define VEKTOR_VERSION_PATCH 0
#define VEKTOR_VERSION_CYCLE alpha
#define VEKTOR_VERSION_SUFFIX

#define VEKTOR_FILE_VERSION VEKTOR_VERSION
#define VEKTOR_FILE_SUBVERSION VEKTOR_VERSION_PATCH

const char* VKE_vektor_version_string(void);

bool        VKE_vektor_version_is_lts(void);

void        VKE_vektor_version_blendfile_string_from_values(char*        str_buff,
                                                            const size_t str_buff_maxncpy,
                                                            const short  file_version,
                                                            const short  file_subversion);
} // namespace vektor