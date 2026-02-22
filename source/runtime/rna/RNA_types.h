#pragma once

#include <span>
#include <vector>
#include "../../runtime/vklib/VKE_enum_flags.hh"
#include "../dna/DNA_id.h"
#include "../kernel/intern/VK_report.hh"

namespace vektor
{
struct PropertyRNA;
struct FunctionRNA;
struct StructRNA;
struct vkContext;

template <typename T,
          size_t N>
constexpr size_t ARRAY_SIZE(const T (&)[N])
{
    return N;
}

constexpr int64_t ANCESTOR_POINTERRNA_DEFAULT_SIZE = 2;

struct StructRNA;

enum PropertyType
{
    PROP_BOOLEAN    = 0,
    PROP_INT        = 1,
    PROP_FLOAT      = 2,
    PROP_STRING     = 3,
    PROP_ENUM       = 4,
    PROP_POINTER    = 5,
    PROP_COLLECTION = 6,
};

struct EnumPropertyItem
{
    int         value;
    const char* identifier;
    int         icon;
    const char* name;
    const char* description;
};

enum PropertyUnit
{
    PROP_UNIT_NONE              = (0 << 16),
    PROP_UNIT_LENGTH            = (1 << 16),  /* m */
    PROP_UNIT_AREA              = (2 << 16),  /* m^2 */
    PROP_UNIT_VOLUME            = (3 << 16),  /* m^3 */
    PROP_UNIT_MASS              = (4 << 16),  /* kg */
    PROP_UNIT_ROTATION          = (5 << 16),  /* radians */
    PROP_UNIT_TIME              = (6 << 16),  /* frame */
    PROP_UNIT_TIME_ABSOLUTE     = (7 << 16),  /* time in seconds (independent of scene) */
    PROP_UNIT_VELOCITY          = (8 << 16),  /* m/s */
    PROP_UNIT_ACCELERATION      = (9 << 16),  /* m/(s^2) */
    PROP_UNIT_CAMERA            = (10 << 16), /* mm */
    PROP_UNIT_POWER             = (11 << 16), /* W */
    PROP_UNIT_TEMPERATURE       = (12 << 16), /* C */
    PROP_UNIT_WAVELENGTH        = (13 << 16), /* `nm` (independent of scene). */
    PROP_UNIT_COLOR_TEMPERATURE = (14 << 16), /* K */
    PROP_UNIT_FREQUENCY         = (15 << 16), /* Hz */
};
ENUM_OPERATORS(PropertyUnit)

struct AncestorPointerRNA
{
    StructRNA* type;
    void*      data;
};

struct PrimitiveStringRNA
{
    const char* value;
};

struct PointerRNA
{
    ID*                             owner_id       = nullptr;
    StructRNA*                      type           = nullptr;
    void*                           data           = nullptr;

    std::vector<AncestorPointerRNA> ancestors      = {};

    PointerRNA()                                   = default;
    PointerRNA(const PointerRNA&)                  = default;
    PointerRNA(PointerRNA&&)                       = default;
    PointerRNA& operator=(const PointerRNA& other) = default;
    PointerRNA& operator=(PointerRNA&& other)      = default;

    PointerRNA(ID*        owner_id,
               StructRNA* type,
               void*      data)
        : owner_id(owner_id)
        , type(type)
        , data(data)
        , ancestors{}
    {
    }
    PointerRNA(ID*               owner_id,
               StructRNA*        type,
               void*             data,
               const PointerRNA& parent)
        : owner_id(owner_id)
        , type(type)
        , data(data)
        , ancestors(parent.ancestors)
    {
        this->ancestors.push_back({parent.type, parent.data});
    }
    PointerRNA(ID*                           owner_id,
               StructRNA*                    type,
               void*                         data,
               std::span<AncestorPointerRNA> parents)
        : owner_id(owner_id)
        , type(type)
        , data(data)
        , ancestors(parents.begin(),
                    parents.end())
    {
    }

    void reset() { *this = {}; }

    void invalidate() { this->reset(); }

    template <typename T>
    T* data_as() const
    {
        return static_cast<T*>(this->data);
    }

    PointerRNA parent() const
    {
        if (ancestors.empty())
        {
            return PointerRNA();
        }

        return PointerRNA(owner_id, ancestors.back().type, ancestors.back().data);
    }
};

static void rna_PrimitiveString_value_get(PointerRNA* ptr,
                                          char*       result)
{
    const PrimitiveStringRNA* data = static_cast<const PrimitiveStringRNA*>(ptr->data);
    strcpy(result, data->value ? data->value : "");
}

struct ParameterList
{
    void*        data;

    FunctionRNA* func;

    int          alloc_size;

    int          arg_count, ret_count;
};

using CallFunc = void (*)(vkContext*     vkC,
                          ReportList*    reports,
                          PointerRNA*    ptr,
                          ParameterList* parms);

extern const PointerRNA PointerRNA_NULL;
extern StructRNA*       RNA_PrimitiveString;

PointerRNA              RNA_pointer_create_discrete(ID*        owner_id,
                                                    StructRNA* type,
                                                    void*      data);

struct PropertyPointerRNA
{
    PointerRNA   ptr  = {};
    PropertyRNA* prop = nullptr;
};

struct PrimitiveIntRNA
{
    int value;
};

struct PrimitiveFloatRNA
{
    float value;
};

struct PrimitiveBooleanRNA
{
    bool value;
};

} // namespace vektor