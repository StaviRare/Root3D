#include "Graphics.h"
#include "OpenGL.h"
#include "Debug.h"

GraphicsAPI* Graphics::graphicsAPI = nullptr;

void Graphics::Initialize(APIType api)
{
    if (api == APIType::OpenGL)
    {
        graphicsAPI = new OpenGL();
    }
    else
    {
        Debug::LogError("Unsupported Graphics API selected!");
    }

    if (graphicsAPI != nullptr)
    {
        graphicsAPI->Initialize();
    }
}

void Graphics::ClearScreen()
{
    graphicsAPI->ClearScreen();
}

void Graphics::ExecuteRenderCommands()
{
    graphicsAPI->ExecuteRenderCommands();
}

void Graphics::UnInitialize()
{
    graphicsAPI->UnInitialize();
}
