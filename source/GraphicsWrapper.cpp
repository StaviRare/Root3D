#include "GraphicsWrapper.h"
#include "OpenGLAPI.h"
#include "DirectXAPI.h"

GraphicsAPI* GraphicsWrapper::mAPI = nullptr;


void GraphicsWrapper::Initialize(APIType api)
{
    mAPI = new OpenGLAPI();

    //if (api == APIType::OpenGL)
    //{
    //    mAPI = new OpenGLAPI();
    //}
    //else if (api == APIType::DirectX)
    //{
    //    //mAPI = new DirectXAPI();
    //}

    mAPI->Initialize();
}

void GraphicsWrapper::ClearScreen()
{
    mAPI->ClearScreen();
}


void GraphicsWrapper::ExecuteRenderCommands()
{
    mAPI->ExecuteRenderCommands();
}
