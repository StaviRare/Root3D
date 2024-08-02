#include "Log.h"
#include "Graphics.h"
#include "OpenGL.h"
#include "DirectX11.h"
#include "Config.h"

GraphicsType Graphics:: _currentType;
GraphicsAPI* Graphics::_currentAPI = nullptr;

string Graphics::TypeName()
{
    switch (_currentType)
    {
        case ( GraphicsType::OpenGL ):
        return "OpenGL";

        case ( GraphicsType::DirectX11 ):
        return "DirectX11";

        default:
        return "Null";
    }
}

void Graphics::Initialize()
{
    RuntimeSettings config = Config::Runtime();
    GraphicsType type = config.RenderingAPI;

    switch (type)
    {
        case ( GraphicsType::OpenGL ):
        _currentAPI = new OpenGL();
        break;

        case ( GraphicsType::DirectX11 ):
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
        ENGINE_ERROR("Unsupported Graphics API selected!");
    }
}

void Graphics::ClearScreen()
{
    if (_currentAPI != nullptr)
    {
        _currentAPI->ClearScreen();
    }
}

void Graphics::ExecuteRenderCommands()
{
    if (_currentAPI != nullptr)
    {
        _currentAPI->ExecuteRenderCommands();
    }
}

void Graphics::SwapFrameBuffers()
{
    if (_currentAPI != nullptr)
    {
        _currentAPI->SwapFrameBuffers();
    }
}

void Graphics::UnInitialize()
{
    if (_currentAPI != nullptr)
    {
        _currentAPI->UnInitialize();

        delete _currentAPI;
        _currentAPI = nullptr;
        _currentType = GraphicsType::Null;
    }
}
