#include "Texture.h"
#include "TextureManager.h"
#include "Log.h"

Texture::Texture(const string& name)
{
    m_name = "Texture";
    m_resourceID = TextureManager::CreateOrGet(name);
}

Texture::~Texture()
{
    TextureManager::TryDestroy(m_resourceID);
}

uniqueID Texture::getResourceID() const
{
    return m_resourceID;
}