#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "Log.h"
#include "TextureManager.h"
#include "Graphics.h"
#include "AssetLoader.h"

uniqueID TextureManager::m_nextID = 0;
std::unordered_map<uniqueID, Resource> TextureManager::m_textures;

uniqueID TextureManager::GetHandle(uniqueID id)
{
    uniqueID returnValue = 0;

    auto it = m_textures.find(id);

    if(it != m_textures.end())
    {
        returnValue = it->second.handle;
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
            ENGINE_DEBUG("Texture destroyed | ID: " + std::to_string(id));
        }
    }

    return returnValue;
}

uniqueID TextureManager::CreateOrGet(const string& name)
{
    uniqueID returnValue = -1;
    string assetPath = BuildTextureAssetPath(name);
    bool assetPathValid = !assetPath.empty();

    if(assetPathValid){
        bool isNewTexture = true;
        hash hashValue = AssetLoader::GetAssetHash(assetPath);

        // Check if texture with same hash already exists
        for (const auto& it : m_textures)
        {
            const Resource& r = it.second;

            if (r.id == hashValue)
            {
                returnValue = it.first;
                m_textures[returnValue].useCount++;
                isNewTexture = false;
            }
        }

        if (isNewTexture)
        {
            auto assetData = AssetLoader::LoadResource(assetPath);
            TextureUpload upload = ParseTextureAsset(assetData);
            uniqueID textureHandle = Graphics::CreateTexture(upload);

            // Not the biggest fan of doing it here, but it'll do for now.
            stbi_image_free(upload.rawData);

            if (textureHandle)
            {
                m_nextID++;
                Resource resource;
                resource.handle = textureHandle;
                resource.useCount = 1;
                resource.id = hashValue;
                m_textures[m_nextID] = resource;
                returnValue = m_nextID;
            }
        }

        bool textureIsValid = returnValue > 0;

        if (textureIsValid)
        {
            if (isNewTexture)
            {
                ENGINE_DEBUG("Texture created | ID: " + std::to_string(returnValue));
            }
        }
    }

    return returnValue;
}

string TextureManager::BuildTextureAssetPath(const string& name)
{
    return name; // Maybe later we'll handle the extension automatically.
}

TextureUpload TextureManager::ParseTextureAsset(const std::vector<char>& data)
{
    TextureUpload returnValue;
    bool hasData = !data.empty();

    if (hasData)
    {
        int width = 0;
        int height = 0;
        int nrChannels = 0;

        unsigned char* rawData = stbi_load_from_memory(
            reinterpret_cast<const stbi_uc*>(data.data()),
            static_cast<int>(data.size()),
            &width,
            &height,
            &nrChannels,
            4 // force RGBA
            );

        if (rawData)
        {
            returnValue.rawData = rawData;
            returnValue.width = width;
            returnValue.height = height;
            returnValue.nrChannels = 4;
        }
    }

    return returnValue;
}