#include "GraphicsWrapper.h"
#include "OpenGL.h"
#include "Debug.h"

GraphicsAPI* GraphicsWrapper::graphicsAPI = nullptr;

void GraphicsWrapper::Initialize(APIType api)
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

void GraphicsWrapper::ClearScreen()
{
    graphicsAPI->ClearScreen();
}

void GraphicsWrapper::ExecuteRenderCommands()
{
    graphicsAPI->ExecuteRenderCommands();
}

void GraphicsWrapper::UnInitialize()
{
    graphicsAPI->UnInitialize();
}
