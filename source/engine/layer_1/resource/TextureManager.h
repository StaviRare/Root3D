#pragma once

#include <unordered_map>

#include "Types.h"
#include "Resource.h"

class TextureManager
{
    public:
    static uniqueID TryDestroy(uniqueID id);
    static uniqueID GetHandle(uniqueID id);
    static uniqueID CreateOrGet(unsigned char* data, unsigned int width, unsigned int height, unsigned int channels);

    private:
    static uniqueID m_nextID;
    static std::unordered_map<uniqueID, Resource> m_textures;
};