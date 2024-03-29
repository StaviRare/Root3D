#pragma once

#include "Types.h"
#include "GraphicsAPI.h"

enum class APIType
{
    Null,
    OpenGL,
    DirectX11
};

class Graphics
{
    friend class Core;

    public:
    static string TypeName();

    private:
    static APIType _currentType;
    static GraphicsAPI* _currentAPI;

    static void Initialize(APIType api);
    static void ClearScreen();
    static void ExecuteRenderCommands();
    static void UnInitialize();
    static void SwapFrameBuffers();
};
