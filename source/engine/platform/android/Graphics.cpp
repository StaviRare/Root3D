#include "Graphics.h"
#include "OpenGLES1.h"
#include "Debug.h"
#include "Config.h"

APIType Graphics:: _currentType;
GraphicsAPI* Graphics::_currentAPI = nullptr;

string Graphics::TypeName()
{
    switch (_currentType)
    {
        case ( APIType::OpenGLES1 ):
            return "OpenGLES1";

        default:
            return "Null";
    }
}

void Graphics::Initialize()
{
    RuntimeSettings config = Config::Runtime();
    APIType type = config.RenderingAPI;

    switch (type)
    {
        case ( APIType::OpenGLES1 ):
            _currentAPI = new OpenGLES1();
            break;
    }

    if (_currentAPI != nullptr)
    {
        _currentType = type;
        _currentAPI->Initialize();
    }
    else
    {
        Debug::LogError("Unsupported Graphics API selected!");
    }
}

void Graphics::ClearScreen()
{
    _currentAPI->ClearScreen();
}

void Graphics::ExecuteRenderCommands()
{
    _currentAPI->ExecuteRenderCommands();
}

void Graphics::SwapFrameBuffers()
{
    _currentAPI->SwapFrameBuffers();
}

void Graphics::UnInitialize()
{
    _currentAPI->UnInitialize();
}
