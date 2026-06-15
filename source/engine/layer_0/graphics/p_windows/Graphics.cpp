#include "Log.h"
#include "Graphics.h"
#include "OpenGL.h"
#include "DirectX11.h"

GraphicsAPI Graphics:: _currentType;
IGraphicsAPI* Graphics::_currentAPI = nullptr;

string Graphics::TypeName()
{
    switch (_currentType)
    {
        case ( GraphicsAPI::OpenGL ):
        {
            return "OpenGL";
        }
        case ( GraphicsAPI::DirectX11 ):
        {
            return "DirectX11";
        }
        default:
        {
            return "Null";
        }
    }
}

void Graphics::Initialize(GraphicsDesc desc)
{
    switch (desc.graphicsAPI)
    {
        case ( GraphicsAPI::OpenGL ):
        {
            _currentAPI = new OpenGL();
            break;
        }
        case ( GraphicsAPI::DirectX11 ):
        {
            _currentAPI = new DirectX11();
            break;
        }
    }

    if (_currentAPI != nullptr)
    {
        _currentType = desc.graphicsAPI;
        _currentAPI->Initialize(desc.windowHandle);
    }
    else
    {
        ENGINE_ERROR("Unsupported Graphics API selected!");
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

void Graphics::Resize(uint32_t width, uint32_t height)
{
    if (_currentAPI != nullptr)
    {
        _currentAPI->Resize(width, height);
    }
}

void Graphics::OnSurfaceLost()
{
    if (_currentAPI != nullptr)
    {
        _currentAPI->OnSurfaceLost();
    }
}

void Graphics::OnSurfaceRecreated(void* windowHandle)
{
    if (_currentAPI != nullptr)
    {
        _currentAPI->OnSurfaceRecreated(windowHandle);
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