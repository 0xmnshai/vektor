#pragma once

#include "../dna/DNA_windowmanager_types.h"
#include "wm_event_types.hh"
#include "wm_types.h"

namespace vektor
{

struct vkContext
{
    void* data;
};

enum wmTimerFlags
{
    WM_TIMER_NO_FREE_CUSTOM_DATA = 1 << 0,
    WM_TIMER_TAGGED_FOR_REMOVAL  = 1 << 16,
};

struct wmTimer
{
    wmTimer *    next, *prev;

    /** Window this timer is attached to (optional). */
    wmWindow*    win;

    /** Set by timer user. */
    double       time_step;

    
    /** Set by timer user, goes to event system. */
    wmEventType  event_type;

    /** Various flags controlling timer options, see below. */
    wmTimerFlags flags;

    /** Set by timer user, to allow custom values. */
    void*        customdata;

    /** Total running time in seconds. */
    double       time_duration;
    /** Time since previous step in seconds. */
    double       time_delta;

    /** Internal, last time timer was activated. */
    double       time_last;
    /** Internal, next time we want to activate the timer. */
    double       time_next;
    /** Internal, when the timer started. */
    double       time_start;
    /** Internal, put timers to sleep when needed. */
    bool         sleep;
};

extern wmWindowManager* G_WM;

void                    init_wm();
} // namespace vektor