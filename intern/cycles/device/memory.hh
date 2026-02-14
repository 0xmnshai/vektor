#pragma once

#include <stddef.h>
#include <cstdint>

enum MemoryType
{
    MEM_READ_ONLY,
    MEM_READ_WRITE,
    MEM_DEVICE_ONLY,
    MEM_GLOBAL,
    MEM_IMAGE_TEXTURE,
};

enum DataType
{
    TYPE_UNKNOWN,
    TYPE_UCHAR,
    TYPE_UINT16,
    TYPE_UINT,
    TYPE_INT,
    TYPE_FLOAT,
    TYPE_HALF,
    TYPE_UINT64,
};

static const size_t datatype_size(DataType datatype) noexcept
{
    switch (datatype)
    {
        case TYPE_UNKNOWN:
            return 1;
        case TYPE_UCHAR:
            return sizeof(char);
        case TYPE_FLOAT:
            return sizeof(float);
        case TYPE_UINT:
            return sizeof(uint32_t);
        case TYPE_UINT16:
            return sizeof(uint16_t);
        case TYPE_INT:
            return sizeof(int32_t);
        case TYPE_HALF:
            return sizeof(uint16_t);
        case TYPE_UINT64:
            return sizeof(uint64_t);
        default:
            return 0;
    }
}

template <typename T>
struct device_type_traits
{
    static const DataType data_type    = TYPE_UNKNOWN;
    static const size_t   num_elements = sizeof(T);
};

template <>
struct device_type_traits<char>
{
    static const DataType data_type    = TYPE_UCHAR;
    static const size_t   num_elements = sizeof(char);
};

template <>
struct device_type_traits<float>
{
    static const DataType data_type    = TYPE_FLOAT;
    static const size_t   num_elements = sizeof(float);
};

template <>
struct device_type_traits<int>
{
    static const DataType data_type    = TYPE_INT;
    static const size_t   num_elements = sizeof(int);
};

template <>
struct device_type_traits<uint32_t>
{
    static const DataType data_type    = TYPE_UINT;
    static const size_t   num_elements = sizeof(uint32_t);
};

template <>
struct device_type_traits<uint16_t>
{
    static const DataType data_type    = TYPE_UINT16;
    static const size_t   num_elements = sizeof(uint16_t);
};

template <>
struct device_type_traits<uint64_t>
{
    static const DataType data_type    = TYPE_UINT64;
    static const size_t   num_elements = sizeof(uint64_t);
};