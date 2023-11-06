#pragma once
#include "GraphicsAPI.h"

enum class APIType
{
    OpenGL,
    DirectX
};

class GraphicsWrapper
{
public:
    static void Initialize(APIType api);
    static void ClearScreen();
    static void ExecuteRenderCommands();
    

private:
    static GraphicsAPI* mAPI;
    static void BindTexture(Texture& texture);
};
