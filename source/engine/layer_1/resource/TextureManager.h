#pragma once

#include <vector>
#include <unordered_map>

#include "Types.h"
#include "Resource.h"
#include "GraphicsTypes.h"

class TextureManager
{
    public:
    static uniqueID GetHandle(uniqueID id);
    static uniqueID TryDestroy(uniqueID id);
    static uniqueID CreateOrGet(const string& name);

    private:
    static string BuildTextureAssetPath(const string& name);
    static TextureUpload ParseTextureAsset(const std::vector<char>& data);

    private:
    static uniqueID m_nextID;
    static std::unordered_map<uniqueID, Resource> m_textures;
};