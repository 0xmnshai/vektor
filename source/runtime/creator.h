#pragma once

#include "VPI_ISystem.h"
namespace vektor::runtime {
void initialize(vpi::VPI_ISystem *sys, vpi::VPI_IWindow *window);
void tick();
}  // namespace vektor::runtime
