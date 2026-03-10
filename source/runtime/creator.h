#pragma once

#include "VPI_ISystem.h"

namespace vektor::runtime {
void main_args_parse(int argc, const char **argv);
void initialize(vpi::VPI_ISystem *sys, vpi::VPI_IWindow *window);
void tick();
}  // namespace vektor::runtime
