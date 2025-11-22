#pragma once

#include "Types.h"
#include "GraphicsAPI.h"

enum class GraphicsType
{
    Null,
    OpenGL,
    OpenGLES1,
    DirectX11
};

class Graphics
{
    friend class Engine;

    public:
    static string TypeName();

    private:
    static GraphicsAPI* _currentAPI;
    static GraphicsType _currentType;

    private:
    static void Initialize();
    static void ClearScreen();
    static void ExecuteRenderCommands();
    static void UnInitialize();
    static void SwapFrameBuffers();
};
