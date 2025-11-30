#pragma once

#include <unordered_map>

#include "Types.h"
#include "Resource.h"

class ShaderManager
{
    public:
    static uniqueID TryDestroy(uniqueID id);
    static uniqueID GetHandle(uniqueID id);
    static uniqueID CreateOrGet(const string vs, const string fs);

    private:
    static uniqueID m_nextID;
    static std::unordered_map<uniqueID, Resource> m_shaders;
};