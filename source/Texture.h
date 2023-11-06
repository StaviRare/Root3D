#pragma once
#include <cstring>
#include <utility>

class Texture
{
public:
    unsigned int textureID = 0;
    unsigned char* rawData = nullptr;
    unsigned int width = 0;
    unsigned int height = 0;
    unsigned int nrChannels = 0;

    Texture() : textureID(0), rawData(nullptr), width(0), height(0), nrChannels(0) {}

    ~Texture()
    {
        delete[] rawData;
    }

    // Corrected copy constructor
    Texture(const Texture& other) 
        : textureID(other.textureID), width(other.width), height(other.height), nrChannels(other.nrChannels)
    {
        if (other.rawData) {
            rawData = new unsigned char[width * height * nrChannels];
            std::memcpy(rawData, other.rawData, width * height * nrChannels);
        }
    }

    // Corrected assignment operator using copy-and-swap idiom
    Texture& operator=(Texture other)
    {
        std::swap(textureID, other.textureID);
        std::swap(rawData, other.rawData);
        std::swap(width, other.width);
        std::swap(height, other.height);
        std::swap(nrChannels, other.nrChannels);
        return *this;
    }
};
