#pragma once

#include <vector>
#include "Types.h"
#include "FieldInfo.h"

struct TypeInfo
{
    const char* name;
    uint64 size;
    std::vector<FieldInfo> fields;
    const TypeInfo* baseType;
};