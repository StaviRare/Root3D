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
        //_currentAPI = new OpenGL();
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

void Graphics::BeginFrame(FrameUniform cmd)
{
    if (_currentAPI != nullptr)
    {
        _currentAPI->BeginFrame(cmd);
    }
}

void Graphics::DrawObject(ObjectUniform cmd)
{
    if (_currentAPI != nullptr)
    {
        _currentAPI->DrawObject(cmd);
    }
}

void Graphics::EndFrame()
{
    if (_currentAPI != nullptr)
    {
        _currentAPI->EndFrame();
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

uniqueID Graphics::CreateShader(const ShaderUpload data)
{
    uniqueID returnValue = 0;

    if (_currentAPI != nullptr)
    {
        returnValue = _currentAPI->CreateShader(data);
    }

    return returnValue;
}

void Graphics::DestroyShader(uniqueID handle)
{
    if (_currentAPI != nullptr)
    {
        _currentAPI->DestroyShader(handle);
    }
}

uniqueID Graphics::CreateTexture(const TextureUpload data)
{
    uniqueID returnValue = 0;

    if (_currentAPI != nullptr)
    {
        returnValue = _currentAPI->CreateTexture(data);
    }

    return returnValue;
}

void Graphics::DestroyTexture(uniqueID handle)
{
    if (_currentAPI != nullptr)
    {
        _currentAPI->DestroyTexture(handle);
    }
}