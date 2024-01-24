#pragma once

#include "Types.h"

class IPlatformInput
{
    public:
    virtual ~IPlatformInput() = default;

    virtual void Initialize() = 0;
    virtual void Tick() = 0;
    virtual void UnInitialize() = 0;
    virtual bool GetKey(const string& key) = 0;
    virtual bool GetKeyDown(const string& key) = 0;
    virtual bool GetKeyUp(const string& key) = 0;
};
