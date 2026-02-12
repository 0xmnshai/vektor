



#pragma once

#if defined(__linux__) || defined(__GNU__) || defined(__NetBSD__) ||           \
    defined(__OpenBSD__) || defined(__FreeBSD_kernel__) || defined(__HAIKU__)


#include <stdint.h>


#ifndef UINT64_MAX
#define UINT64_MAX 18446744073709551615
typedef uint8_t u_int8_t;
typedef uint16_t u_int16_t;
typedef uint32_t u_int32_t;
typedef uint64_t u_int64_t;
#endif

#elif defined(__APPLE__)

#include <inttypes.h>


#elif defined(_MSC_VER)
#include <stdint.h>


#else


#include <stdint.h>
#include <sys/types.h>

#endif

#include <stdbool.h>
#include <stddef.h>

#ifndef __cplusplus

#if defined(__APPLE__) || defined(__NetBSD__) || defined(__OpenBSD__)
typedef unsigned int char32_t;
#else
#include <uchar.h>
#endif
#endif

typedef unsigned int uint;
typedef unsigned short ushort;
typedef unsigned long ulong;
typedef unsigned char uchar;
