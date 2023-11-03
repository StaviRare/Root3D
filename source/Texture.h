// texture.h
#pragma once

class Texture
{
public:
    unsigned int textureID = 0;
    unsigned char* rawData = nullptr;
    unsigned int width = 0;
    unsigned int height = 0;
    unsigned int nrChannels = 0;

    Texture() : textureID(0), rawData(nullptr), width(0), height(0), nrChannels(0)
    {

    }

    ~Texture()
    {
        delete[] rawData;
    }

    // Copy constructor
    Texture(const Texture& other) : textureID(other.textureID), width(other.width), height(other.height), nrChannels(other.nrChannels)
    {
        rawData = new unsigned char[width * height * nrChannels];
        std::copy(other.rawData, other.rawData + width * height * nrChannels, rawData);
    }

    Texture& operator=(const Texture& other)
    {
        if (this != &other)
        {
            Texture temp(other); // Utilize the copy constructor
            std::swap(textureID, temp.textureID);
            std::swap(rawData, temp.rawData);
            std::swap(width, temp.width);
            std::swap(height, temp.height);
            std::swap(nrChannels, temp.nrChannels);
        }
        return *this;
    }
};
