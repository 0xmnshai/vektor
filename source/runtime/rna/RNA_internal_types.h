#pragma once

#include <map>
#include <string>
#include <vector>

#include "../vklib/VKE_string_ref.hh" // IWYU pragma: keep
#include "../vklib/VKE_vector_set.hh" // IWYU pragma: keep
#include "RNA_types.h"

using namespace vektor::vklib;

#define RNA_MAX_ARRAY_DIMENSION 8

namespace vektor
{

struct PropertyRNA
{
    PropertyRNA *     next, *prev;

    int               magic;

    const std::string identifier;

    int               flag;

    int               flag_override;

    short             flag_parameter;

    short             flag_internal;

    short             tags;

    const std::string name;

    const char*       description;

    int               icon;

    const std::string translation_context;

    PropertyType      type;

    unsigned int      arraydimension;

    unsigned int      arraylength[RNA_MAX_ARRAY_DIMENSION];
    unsigned int      totarraylength;

    int               noteflag;

    void*             py_data;
};

struct PropertyRNAIdentifierGetter
{
    StringRef operator()(const PropertyRNA* prop) const;
};

struct ContainerRNA
{
    CustomIDVectorSet<PropertyRNA*, PropertyRNAIdentifierGetter>* prop_lookup_set;
    ListBaseT<PropertyRNA>                                        properties;
};

struct FunctionRNA
{
    ContainerRNA cont        = {};
    const char*  identifier  = nullptr;

    int          flag        = 0;

    const char*  description = nullptr;

    CallFunc     call        = {};

    PropertyRNA* c_ret       = nullptr;
};

struct StructRNA
{
    ContainerRNA                              cont                = {};

    const std::string                         identifier          = nullptr;

    void*                                     py_type             = nullptr;

    void*                                     vektor_type         = nullptr;

    int                                       flag                = 0;

    const EnumPropertyItem*                   prop_tag_defines    = nullptr;

    const std::string                         name                = nullptr;

    const std::string                         description         = nullptr;

    const std::string                         translation_context = nullptr;

    int                                       icon                = 0;

    PropertyRNA*                              nameproperty        = nullptr;

    PropertyRNA*                              iteratorproperty    = nullptr;

    StructRNA*                                base                = nullptr;

    StructRNA*                                nested              = nullptr;

    std::vector<std::unique_ptr<FunctionRNA>> functions;

};

struct vektorRNA
{
    std::vector<std::unique_ptr<StructRNA>> structs;

    std::map<StringRef, StructRNA*>         structs_map;
};

#define CONTAINER_RNA_ID(cont) (*(const char**)(((ContainerRNA*)(cont)) + 1))

} // namespace vektor
