#pragma once

#include <string>
#include <unordered_map>
#include "stb_image.h"
#include "Texture.h"
#include "Debug.h"

class TextureResourceManager
{
private:
    static std::unordered_map<std::string, Texture> textures;

public:
    static Texture Load(const std::string& path)
    {
        // Check if texture is already loaded
        auto it = textures.find(path);
        
        if (it != textures.end())
        {
            // If it's already loaded, return the existing texture
            return it->second;
        }

        Texture texture;
        int width, height, nrChannels;
        texture.rawData = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);

        if (texture.rawData)
        {
            texture.width = static_cast<unsigned int>(width);
            texture.height = static_cast<unsigned int>(height);
            texture.nrChannels = static_cast<unsigned int>(nrChannels);

            // GET TEXTURE ID FROM GRAPHICS API
            // GET TEXTURE ID FROM GRAPHICS API
            // GET TEXTURE ID FROM GRAPHICS API

            // Store the texture in the map
            textures[path] = texture;
        }
        else
        {
            Debug::error("Failed to load texture : " + path);
        }

        return texture;
    }
    static void Unload(const Texture& texture)
    {
        // This function assumes that the textureID is unique and used as an identifier
        for (auto it = textures.begin(); it != textures.end(); ++it)
        {
            if (it->second.textureID == texture.textureID)
            {
                // Here you would typically delete the texture from your graphics API
                // DeleteTextureID(it->second.textureID);

                // Free the loaded texture data and remove it from the map
                stbi_image_free(it->second.rawData);
                textures.erase(it);
                return;
            }
        }

        // Handle the case where the texture to unload is not found
        // For example, log a warning or throw an exception
    }
};