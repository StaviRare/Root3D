#pragma once

#include "GraphicsAPI.h"

enum class APIType
{
    OpenGL,
    DirectX11
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
    static void SwapFrameBuffers();
};
