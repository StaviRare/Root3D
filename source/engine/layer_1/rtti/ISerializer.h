#pragma once
#include "Types.h"

struct ISerializer
{
    virtual void BeginObject() = 0;
    virtual void EndObject() = 0;
    virtual void WriteField(const char* name) = 0;
    
    template <typename T>
    void WriteValue(const T& value);
};

template <typename T>
inline void ISerializer::WriteValue(const T& value)
{
    // For now...
}