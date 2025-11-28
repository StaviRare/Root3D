#include <fstream>
#include <sstream>
#include <algorithm>
#include <ttf2mesh.h>

#include "Log.h"
#include "Directory.h"
#include "AssetLoader.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

Texture AssetLoader::LoadTexture(const string& path)
{
    Texture returnValue;
    auto data = LoadResource(path);

    if (data.empty() == false)
    {
        int width, height, nrChannels;
        returnValue.rawData = stbi_load_from_memory(
            reinterpret_cast<const stbi_uc*>(data.data()), 
            data.size(), 
            &width, 
            &height, 
            &nrChannels, 
            0
        );

        if (returnValue.rawData)
        {
            returnValue.width = static_cast<unsigned int>(width);
            returnValue.height = static_cast<unsigned int>(height);
            returnValue.nrChannels = static_cast<unsigned int>(nrChannels);
        }
    }

    return returnValue;
}

Shader* AssetLoader::LoadShader(const string& path)
{
    Shader* returnValue = nullptr;
    auto data = LoadResource(path);

    if (data.empty() == false)
    {
        enum class ShaderType
        {
            NONE = -1,
            VERTEX = 0,
            FRAGMENT = 1
        };

        ShaderType type = ShaderType::NONE;

        std::string line;
        std::stringstream ss[2];
        std::string shaderString(data.begin(), data.end());
        std::istringstream fileStream(shaderString);

        while (std::getline(fileStream, line))
        {
            if (line.find("==SEPARATOR==") != std::string::npos)
            {
                type = ShaderType::FRAGMENT; // Switch to fragment shader
                continue; // Skip the separator line
            }

            if (type != ShaderType::NONE)
            {
                ss[static_cast<int>( type )] << line << '\n';
            }
            else
            {
                // Default to vertex shader if no type has been set yet
                type = ShaderType::VERTEX;
                ss[static_cast<int>( type )] << line << '\n';
            }
        }

        returnValue = new Shader(ss[0].str(),ss[1].str()) ;
    }

    return returnValue;
}

Font AssetLoader::LoadFont(const string& path)
{
    Font returnValue;
    auto data = LoadResource(path);

    if (data.empty() == false)
    {
        ttf_t* font = nullptr;
        int size = static_cast<int>(data.size());
        const uint8_t* rawData = reinterpret_cast<const uint8_t*>(data.data());
        bool fontLoadError = ttf_load_from_mem(rawData, size, &font, false) != TTF_DONE;

        if (fontLoadError)
        {
            ENGINE_ERROR("Failed to parse font: \"" + path + "\"");
        }
        else
        {
            // English only. Make sure the font includes English characters.
            for (char32_t ch = 32; ch <= 'z'; ++ch)
            {
                int index = ttf_find_glyph(font, static_cast<uint32_t>(ch));
                bool indexValid = index >= 0;

                if (indexValid)
                {
                    ttf_mesh_t* mesh = nullptr;
                    ttf_glyph_t* glyph = &font->glyphs[index];
                    bool meshValid = ttf_glyph2mesh(glyph, &mesh, TTF_QUALITY_HIGH, TTF_FEATURES_DFLT) == TTF_DONE;

                    if (meshValid)
                    {
                        GlyphEntry entry;
                        entry.codepoint = ch;
                        entry.metrics.advance = glyph->advance;

                        for (int j = 0; j < mesh->nvert; ++j)
                        {
                            entry.data.positions.emplace_back(Vector2{mesh->vert[j].x, mesh->vert[j].y});
                        }

                        for (int j = 0; j < mesh->nfaces; ++j)
                        {
                            entry.data.indices.push_back(mesh->faces[j].v1);
                            entry.data.indices.push_back(mesh->faces[j].v2);
                            entry.data.indices.push_back(mesh->faces[j].v3);
                        }

                        returnValue.glyphs.emplace_back(std::move(entry));
                        ttf_free_mesh(mesh);
                    }
                }
            }

            ttf_free(font);
        }
    }

    return returnValue;
}

std::vector<char> AssetLoader::LoadResource(const string& resourcePath)
{
    std::vector<char> data;
    const string resourceFile = Directory::GetResourcePath();
    std::ifstream file(resourceFile, std::ios::binary);
    bool isFileOpen = file.is_open();
    bool isResourceFound = false;

    while (isFileOpen && !isResourceFound)
    {
        unsigned int pathLength;
        file.read(reinterpret_cast<char*>( &pathLength ), sizeof(pathLength));
        isFileOpen = file.good();

        if (isFileOpen)
        {
            string path(pathLength, '\0');
            file.read(&path[0], pathLength);
            std::replace(path.begin(), path.end(), '\\', '/');

            unsigned int dataLength;
            file.read(reinterpret_cast<char*>( &dataLength ), sizeof(dataLength));
            isFileOpen = file.good();

            if (isFileOpen && path == resourcePath)
            {
                data.resize(dataLength);
                file.read(data.data(), dataLength);
                isResourceFound = file.good();
            }
            else if (isFileOpen)
            {
                file.seekg(dataLength, std::ios::cur);
            }
        }
    }

    if(resourceFile.empty() == false)
    {
        if (isResourceFound == false)
        {
            ENGINE_ERROR("Resource not found: " + resourcePath);
        }
        else
        {
            ENGINE_INFO("Resource loaded: " + resourcePath);
        }
    }
    else
    {
        ENGINE_ERROR("Resource file not found!");
    }

    return data;
}