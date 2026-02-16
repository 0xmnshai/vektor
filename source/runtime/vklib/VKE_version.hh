#pragma once

#include <cstddef>

namespace vektor
{

#define VEKTOR_VERSION_SUFFIX

const char* VKE_vektor_version_string(void);

bool        VKE_vektor_version_is_lts(void);

void        VKE_vektor_version_blendfile_string_from_values(char*        str_buff,
                                                            const size_t str_buff_maxncpy,
                                                            const short  file_version,
                                                            const short  file_subversion);
} // namespace vektor