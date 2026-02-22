#pragma once

#include <cstdint>

#include "../dna/DNA_listBase.h"
#include "../vklib/VKE_listbase_iterator.hh" // IWYU pragma: keep

namespace vektor
{

typedef double (*BLI_timer_func)(uintptr_t uuid,
                                 void*     user_data);
typedef void (*BLI_timer_data_free)(uintptr_t uuid,
                                    void*     user_data);

struct TimedFunction
{
    TimedFunction *next, *prev;
    void*          user_data;
    double         next_time;
    uintptr_t      uuid;
    bool           tag_removal;
    bool           persistent;
};

struct TimerContainer
{
    ListBaseT<TimedFunction> funcs;
};

static TimerContainer GlobalTimer = {{nullptr}};

static void           remove_non_persistent_functions()
{
    for (TimedFunction& timed_func : GlobalTimer.funcs)
    {
        if (!timed_func.persistent)
        {
            timed_func.tag_removal = true;
        }
    }
}

// void VKE_timer_on_file_load(void);

inline void VKE_timer_on_file_load()
{
    remove_non_persistent_functions();
}

} // namespace vektor