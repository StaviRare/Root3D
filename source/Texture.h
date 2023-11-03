#pragma once

class Texture
{
public:
    unsigned int textureID = 0;
    unsigned char* rawData = nullptr;
    unsigned int width = 0;
    unsigned int height = 0;
    unsigned int nrChannels = 0;

    ~Texture()
    {
        delete[] rawData;
    }

    Texture& operator=(const Texture& other)
    {
        if (this != &other)
        {
            // Free the existing resources
            delete[] rawData;

            // Copy non-resource members
            width = other.width;
            height = other.height;
            nrChannels = other.nrChannels;
            textureID = other.textureID;

            // Allocate new resources and copy
            rawData = new unsigned char[width * height * nrChannels];
            std::copy(other.rawData, other.rawData + width * height * nrChannels, rawData);
        }

        return *this;
    }
};
