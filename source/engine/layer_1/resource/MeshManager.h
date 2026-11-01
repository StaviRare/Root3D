#pragma once

#include <vector>
#include <unordered_map>

#include "Types.h"
#include "Resource.h"
#include "GraphicsTypes.h"

class MeshManager
{
    public:
    static uniqueID GetHandle(uniqueID id);
    static uniqueID TryDestroy(uniqueID id);
    static uniqueID CreateOrGet(const string& path);
    static string GetVirtualPath();

    private:
    static uniqueID LoadAsset(const string& path);
    static uniqueID LoadPrimitive(const string& path);

    private:
    static uniqueID m_nextID;
    static std::unordered_map<uniqueID, Resource> m_meshes;
};