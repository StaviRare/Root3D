#pragma once

#include "GraphicsAPI.h"
#include <d3d11.h>
#include <DirectXMath.h>

// Forward declarations to reduce the need for including DirectX headers in other files.
struct ID3D11Device;
struct ID3D11DeviceContext;
struct IDXGISwapChain;
struct ID3D11RenderTargetView;
struct ID3D11VertexShader;
struct ID3D11PixelShader;
struct ID3D11InputLayout;
struct ID3D11Buffer;

class DirectX11 : public GraphicsAPI
{
    public:
    void Initialize();
    void ClearScreen();
    void ExecuteRenderCommands();
    void UnInitialize();

    private:
    void CreateDeviceAndSwapChain(HWND hwnd);
    void CreateRenderTargetView();
    void SetupViewport(UINT width, UINT height);
    void CreateShadersAndInputLayout();

    void CompileShaderFromSource(const char* source, const char* entryPoint, const char* shaderModel, ID3DBlob** blobOut);

    ID3D11Device* device = nullptr;
    ID3D11DeviceContext* context = nullptr;
    IDXGISwapChain* swapChain = nullptr;
    ID3D11RenderTargetView* backBufferRTV = nullptr;
    ID3D11VertexShader* vertexShader = nullptr;
    ID3D11PixelShader* pixelShader = nullptr;
    ID3D11InputLayout* inputLayout = nullptr;

    ID3D11Buffer* indexBuffer = nullptr;
    ID3D11Buffer* vertexBuffer = nullptr;
    ID3D11Buffer* constantBuffer = nullptr;

    bool initialized = false;
};
