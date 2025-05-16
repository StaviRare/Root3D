#pragma once

#include "Types.h"
#include "UserDefinedTypes.h"

struct ISerializer;
struct IDeserializer;

struct FieldInfo
{
    const char* name;
    const char* typeName;
    uint64 offset;
    FuncDyn2<void*, ISerializer&> serializeFn;
    FuncDyn2<void*, IDeserializer&> deserializeFn;
};