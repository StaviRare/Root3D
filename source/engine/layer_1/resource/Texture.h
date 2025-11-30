#pragma once
#include "Types.h"
#include "TextureManager.h"
#include "Log.h"
#include "Object.h"

class Texture : public Object
{
    public:
    Texture()
    {
        m_name = "Texture";
    }

    // Create from raw data
    Texture(unsigned char* data, unsigned int width, unsigned int height, unsigned int channels)
    {
        m_name = "Texture";
        m_resourceID = TextureManager::CreateOrGet(data, width, height, channels);
    }

    ~Texture()
    {
        TextureManager::TryDestroy(m_resourceID);
    }

    uniqueID getID() const
    {
        return m_resourceID;
    }

    private:
    uniqueID m_resourceID = 0;
};
