#pragma once

// using wm types .h for scrarea will change to screen VK later
#include "../../runtime/vklib/VKE_string_ref.hh"
#include "wm_types.h"

using namespace vektor::vklib;

namespace vektor
{
StringRefNull ED_area_name(const ScrArea* area);
}