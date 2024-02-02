#include "GraphicsWrapper.h"
#include "OpenGL.h"

GraphicsAPI* GraphicsWrapper::graphicsAPI = nullptr;


void GraphicsWrapper::Initialize(APIType api)
{
    graphicsAPI = new OpenGL();

    //if (api == APIType::OpenGL)
    //{
    //    graphicsAPI = new OpenGL();
    //}
    //else if (api == APIType::DirectX)
    //{
    //    graphicsAPI = new DirectX();
    //}

    graphicsAPI->Initialize();
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
