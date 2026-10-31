#pragma once

#include <vector>
#include <unordered_map>

#include "Types.h"
#include "Resource.h"
#include "GraphicsTypes.h"

class ShaderManager
{
    public:
    static uniqueID GetHandle(uniqueID id);
    static uniqueID TryDestroy(uniqueID id);
    static uniqueID CreateOrGet(const string& name);

    private:
    static string BuildShaderAssetPath(const string& name);
    static ShaderUpload ParseShaderAsset(const std::vector<char>& data);

    private:
    static uniqueID m_nextID;
    static std::unordered_map<uniqueID, Resource> m_shaders;
};