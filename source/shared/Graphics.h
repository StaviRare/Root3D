#pragma once

#include "GraphicsAPI.h"

enum class APIType
{
    OpenGL,
    DirectX
};

class Graphics
{
    friend class Core;

    private:
    static GraphicsAPI* graphicsAPI;

    static void Initialize(APIType api);
    static void ClearScreen();
    static void ExecuteRenderCommands();
    static void UnInitialize();
};
