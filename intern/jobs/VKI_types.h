#pragma once

#include <cstdint>

namespace vektor
{
struct VKI_ITimerTask;

using VKI_TimerProcPtr = void (*)(VKI_ITimerTask* task,
                                    uint64_t          time);

using VKI_TUserDataPtr = void*;

} // namespace vektor