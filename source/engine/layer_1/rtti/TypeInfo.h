#pragma once

#include <vector>
#include "Types.h"

struct FieldInfo;

struct TypeInfo
{
    const char* name;
    uint64 size;
    std::vector<FieldInfo> fields;
    const TypeInfo* baseType;
};