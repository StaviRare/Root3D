#include "Log.h"
#include "ShaderManager.h"
#include "Graphics.h"

uniqueID ShaderManager::m_nextID;
std::unordered_map<uniqueID, Resource> ShaderManager::m_shaders;

uniqueID ShaderManager::CreateOrGet(const string vs, const string fs)
{
    bool isNewShader = true;
    uniqueID returnValue = 0;

    // Get hash from shader:
    std::hash<std::string> hasher;
    hash hashVS = hasher(vs);
    hash hashFS = hasher(fs);
    hash combinedHash = hashVS ^ (hashFS << 1);

    for (const auto& it : m_shaders)
    {
        const Resource& r = it.second;

        if (r.hash == combinedHash)
        {
            returnValue = it.first;
            m_shaders[returnValue].useCount++;
            isNewShader = false;
        }
    }

    if (isNewShader)
    {
        ShaderUpload upload;
        upload.vertexCode = vs;
        upload.fragmentCode = fs;
        unsigned int shaderHandle = Graphics::CreateShader(upload);

        if (shaderHandle)
        {
            m_nextID++;

            Resource resource;
            resource.handle = shaderHandle;
            resource.useCount = 1;
            resource.hash = combinedHash;
            m_shaders[m_nextID] = resource;

            returnValue = m_nextID;
        }
        else
        {
            ENGINE_ERROR("Shader compile fail.");
        }
    }

    // Shader is valid
    if (returnValue > 0)
    {
        if (isNewShader)
        {
            ENGINE_INFO("Resource [Shader] Created | ID: " + std::to_string(returnValue));
        }
    }

    return returnValue;
}

uniqueID ShaderManager::TryDestroy(uniqueID id)
{
    uniqueID returnValue = 0;

    auto it = m_shaders.find(id);
    if(it != m_shaders.end())
    {
        if(it->second.useCount > 1)
        {
            it->second.useCount--;
            returnValue = id;
        }
        else
        {
            Graphics::DestroyShader(it->second.handle);
            m_shaders.erase(it);
            ENGINE_INFO("Resource [Shader] Destroyed  | ID: " + std::to_string(id));
        }
    }

    return returnValue;
}

unsigned int ShaderManager::GetHandle(uniqueID id)
{
    unsigned int returnValue = 0;

    auto it = m_shaders.find(id);

    if(it != m_shaders.end())
    {
        returnValue = it->second.handle;
    }

    return returnValue;
}
