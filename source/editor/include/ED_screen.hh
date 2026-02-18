#pragma once

// using wm types .h for scrarea will change to screen VK later
#include "../../runtime/vklib/VKE_string_ref.hh"
#include "../dna/DNA_windowmanager_types.h" // IWYU pragma: keep

using namespace vektor::vklib;

StringRefNull ED_area_name(const vektor::ScrArea* area);
