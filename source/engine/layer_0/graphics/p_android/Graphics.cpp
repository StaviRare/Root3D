#include "Graphics.h"
#include "OpenGLES1.h"
#include "Log.h"
#include "Config.h"

GraphicsAPI Graphics:: _currentType;
IGraphicsAPI* Graphics::_currentAPI = nullptr;

string Graphics::TypeName()
{
    switch (_currentType)
    {
        case ( GraphicsAPI::OpenGLES1 ):
            return "OpenGLES1";

        default:
            return "Null";
    }
}

void Graphics::Initialize()
{
    RuntimeSettings config = Config::Runtime();
    GraphicsAPI type = config.RenderingAPI;

    switch (type)
    {
        case ( GraphicsAPI::OpenGLES1 ):
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
        _currentType = GraphicsAPI::Null;
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