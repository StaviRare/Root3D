#pragma once

#include "Types.h"

struct IDeserializer
{
    virtual bool BeginObject() = 0;
    virtual void EndObject() = 0;
    virtual bool ReadField(const char* name) = 0;

    template <typename T>
    void ReadValue(T& value);
};

template <typename T>
inline void IDeserializer::ReadValue(T& value)
{
    // For now...
}