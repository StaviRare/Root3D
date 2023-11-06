#pragma once
#include "Texture.h"

class GraphicsAPI
{
public:
    virtual void Initialize() = 0;
    virtual void ClearScreen() = 0;
    virtual void ExecuteRenderCommands() = 0;
    virtual void BindTexture(Texture& texture) = 0;
};

