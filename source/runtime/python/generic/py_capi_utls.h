#pragma once

namespace vektor
{
[[nodiscard]] bool PyC_IsInterpreterActive();

void               PyC_FileAndNum(const char** r_filename,
                                  int*         r_lineno);

void               PyC_FileAndNum_Safe(const char** r_filename,
                                       int*         r_lineno);
} // namespace vektor