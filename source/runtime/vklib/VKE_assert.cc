#include "VKE_assert.h"
#include <cstdio>
#include <cstdlib>

#include "../../../intern/clog/COG_log.hh"

CLG_LOGREF_DECLARE_GLOBAL(LOG_ASSERT,
                          "ASSERT");

void _VKE_assert_print_pos(const char* file,
                           int         line,
                           const char* function,
                           const char* id)
{
    char file_line[1024];
    snprintf(file_line, sizeof(file_line), "%s:%d", file, line);

    const struct CLG_LogType* lg = CLOG_ENSURE(LOG_ASSERT);
    if (lg->level >= CLG_LEVEL_ERROR)
    {
        CLG_logf(lg, CLG_LEVEL_ERROR, file_line, function, "%s", id);
    }
}

void _VKE_assert_print_extra(const char* str)
{
    CLOG_ERROR(LOG_ASSERT, "Extra info: %s", str);
}

void _VKE_assert_print_backtrace(void)
{
    CLOG_ERROR(LOG_ASSERT, "Backtrace not implemented.");
}

void _VKE_assert_abort(void)
{
    CLOG_FATAL(LOG_ASSERT, "Aborting");
}
