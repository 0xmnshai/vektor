#pragma once

#include <mutex>

#include "../dna/DNA_listBase.h"

namespace vektor
{

struct wmTimer;

enum eReportType : uint16_t
{
    RPT_DEBUG                 = (1 << 0),
    RPT_INFO                  = (1 << 1),
    RPT_OPERATOR              = (1 << 2),
    RPT_PROPERTY              = (1 << 3),
    RPT_WARNING               = (1 << 4),
    RPT_ERROR                 = (1 << 5),
    RPT_ERROR_INVALID_INPUT   = (1 << 6),
    RPT_ERROR_INVALID_CONTEXT = (1 << 7),
    RPT_ERROR_OUT_OF_MEMORY   = (1 << 8),
};

#define RPT_DEBUG_ALL (RPT_DEBUG)
#define RPT_INFO_ALL (RPT_INFO)
#define RPT_OPERATOR_ALL (RPT_OPERATOR)
#define RPT_PROPERTY_ALL (RPT_PROPERTY)
#define RPT_WARNING_ALL (RPT_WARNING)
#define RPT_ERROR_ALL (RPT_ERROR | RPT_ERROR_INVALID_INPUT | RPT_ERROR_INVALID_CONTEXT | RPT_ERROR_OUT_OF_MEMORY)

enum ReportListFlags
{
    RPT_PRINT                  = (1 << 0),
    RPT_STORE                  = (1 << 1),
    RPT_FREE                   = (1 << 2),
    RPT_OP_HOLD                = (1 << 3),
    RPT_PRINT_HANDLED_BY_OWNER = (1 << 4),
};

struct Report
{
    Report *    next, *prev;
    short       type;
    short       flag;
    int         len;
    const char* typestr;
    const char* message;
};

struct ReportList
{
    ListBaseT<Report> list;
    int               printlevel;
    int               storelevel;
    int               flag;
    char              _pad[4];
    wmTimer*          reporttimer;

    /** Mutex for thread-safety, runtime only. */
    std::mutex*       lock;
};
} // namespace vektor