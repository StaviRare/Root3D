#pragma once

#include "Types.h"
#include "Object.h"

class Texture : public Object
{
    public:
    Texture(const string& name);
    ~Texture();
    uniqueID getResourceID() const;

    private:
    uniqueID m_resourceID = 0;
};
