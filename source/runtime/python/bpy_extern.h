#pragma once

#include "../vklib/VKE_context.h"

namespace vektor
{
[[nodiscard]] bool                       BPY_context_member_get(vkContext*          C,
                                                                const char*         member,
                                                                bContextDataResult* result);

[[nodiscard]] std::optional<std::string> BPY_python_current_file_and_line(void);

} // namespace vektor