#pragma once
#include "GraphicsAPI.h"

// ToDo! - Handle this better
enum class APIType
{
    OpenGL,
    DirectX
};

class GraphicsWrapper // Change name to Graphics
{
public:
    static void Initialize(APIType api);
    static void ClearScreen();
    static void ExecuteRenderCommands();
    static void UnInitialize();
    

private:
    static GraphicsAPI* graphicsAPI;
};
