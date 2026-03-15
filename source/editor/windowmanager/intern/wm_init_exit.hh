#pragma once

#include "../../../runtime/lib/intern/context.hh"

namespace vektor::editor {
void WM_init(lib::vkContext *vkC, int argc, const char **argv);
void WM_exit();
}  // namespace vektor::editor
