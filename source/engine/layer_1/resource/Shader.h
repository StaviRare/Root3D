#pragma once

#include "Types.h"
#include "Object.h"

class Shader : public Object
{
    public:
    Shader(const string& name);
    ~Shader();
    uniqueID getResourceID() const;

    private:
    uniqueID m_resourceID = 0;
};