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
        _currentType = APIType::Null;
    }
}
