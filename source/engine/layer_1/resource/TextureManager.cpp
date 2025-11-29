#include "Log.h"
#include "TextureManager.h"
#include "Graphics.h"

uniqueID TextureManager::m_nextID;
std::unordered_map<uniqueID, Resource> TextureManager::m_textures;

uniqueID TextureManager::CreateOrGet(unsigned char* data, unsigned int width, unsigned int height, unsigned int channels)
{
    bool isNewTexture = true;
    uniqueID returnValue = 0;

    // Get hash from texture:
    std::hash<unsigned char> hasher;
    size_t hashValue = width ^ (height << 1) ^ (channels << 2);
    for (unsigned int i = 0; i < width * height * channels; ++i)
    {
        hashValue ^= hasher(data[i]) + 0x9e3779b9 + (hashValue << 6) + (hashValue >> 2);
    }

    // Check if texture with same hash already exists
    for (const auto& it : m_textures)
    {
        const Resource& r = it.second;

        if (r.hash == hashValue)
        {
            returnValue = it.first;
            m_textures[returnValue].useCount++;
            isNewTexture = false;
        }
    }

    // Not found, create new
    if (isNewTexture)
    {
        TextureUpload upload;
        upload.rawData = data;
        upload.width = width;
        upload.height = height;
        upload.nrChannels = channels;
        unsigned int shaderHandle = Graphics::CreateTexture(upload);

        if (shaderHandle)
        {
            m_nextID++;

            Resource resource;
            resource.handle = shaderHandle;
            resource.useCount = 1;
            resource.hash = hashValue;
            m_textures[m_nextID] = resource;

            returnValue = m_nextID;
        }
        else
        {
            ENGINE_ERROR("Texture compile fail.");
        }
    }

    // Texture is valid
    if (returnValue > 0)
    {
        if (isNewTexture)
        {
            ENGINE_INFO("Resource [Texture] Created | ID: " + std::to_string(returnValue));
        }
    }

    return returnValue;
}

uniqueID TextureManager::TryDestroy(uniqueID id)
{
    uniqueID returnValue = 0;

    auto it = m_textures.find(id);
    if(it != m_textures.end())
    {
        if(it->second.useCount > 1)
        {
            it->second.useCount--;
            returnValue = id;
        }
        else
        {
            Graphics::DestroyTexture(it->second.handle);
            m_textures.erase(it);
            ENGINE_INFO("Resource [Texture] Destroyed  | ID: " + std::to_string(id));
        }
    }

    return returnValue;
}

unsigned int TextureManager::GetHandle(uniqueID id)
{
    unsigned int returnValue = 0;

    auto it = m_textures.find(id);

    if(it != m_textures.end())
    {
        returnValue = it->second.handle;
    }

    return returnValue;
}
