#pragma once

#include <list>
#include <d3d11.h>
#include <DirectXMath.h>
#include "GraphicsAPI.h"
#include "Texture.h"
#include "Types.h"

using DirectX::XMFLOAT3;
using DirectX::XMMATRIX;

struct ShaderProgram
{
    unsigned int ID = 0;
    ID3D11InputLayout* inputLayout = nullptr;
    ID3D11PixelShader* pixelShader = nullptr;
    ID3D11VertexShader* vertexShader = nullptr;
};

// Model-View-Projection
struct MVPBuffer
{
    XMMATRIX model;
    XMMATRIX view;
    XMMATRIX projection;
};

// Uses padding to meet DirectX 16-byte alignment requirements.
struct LightBuffer 
{
    XMFLOAT3 LightDirection;
    float padding1;
    XMFLOAT3 LightColor;
    float padding2;
};

class DirectX11 : public GraphicsAPI
{
    public:
    void Initialize();
    void ClearScreen();
    void ExecuteRenderCommands();
    void SwapFrameBuffers();
    void UnInitialize();

    private:
    void CreateDeviceAndSwapChain(HWND hwnd);
    void CreateRenderTargetView();
    void SetupViewport(UINT width, UINT height);
    void CreateBuffer(void* data, UINT size, D3D11_BIND_FLAG bindFlag, ID3D11Buffer** buffer);
    void BindTexture(Texture& texture);
    unsigned int CreateShaderProgram(const string& vertexSource, const string& fragmentSource);
    void CompileShader(const string& source, const char* entryPoint, const char* shaderModel, ID3DBlob** blobOut);

    ID3D11Device* device = nullptr;
    ID3D11DeviceContext* context = nullptr;
    IDXGISwapChain* swapChain = nullptr;
    ID3D11RenderTargetView* backBufferRTV = nullptr;
    ID3D11DepthStencilView* depthStencilView = nullptr;

    ID3D11Buffer* vertexBuffer = nullptr;
    ID3D11Buffer* indexBuffer = nullptr;
    ID3D11Buffer* texCoordBuffer = nullptr;
    ID3D11Buffer* normalBuffer = nullptr;
    ID3D11Buffer* mvpBuffer = nullptr;
    ID3D11Buffer* lightBuffer = nullptr;

    bool initialized = false;
    unsigned int nextShaderID = 0;
    std::list<ShaderProgram> shaderMap;
    MVPBuffer mvpBufferData;
    LightBuffer lightBufferData;
};
