#include "Graphics.h"
#include "OpenGL.h"
#include "DirectX11.h"
#include "Debug.h"
#include "Config.h"

APIType Graphics:: _currentType;
GraphicsAPI* Graphics::_currentAPI = nullptr;

string Graphics::TypeName()
{
    switch (_currentType)
    {
        case ( APIType::OpenGL ):
        return "OpenGL";

        case ( APIType::DirectX11 ):
        return "DirectX11";

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
        case ( APIType::OpenGL ):
        _currentAPI = new OpenGL();
        break;

        case ( APIType::DirectX11 ):
        _currentAPI = new DirectX11();
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
