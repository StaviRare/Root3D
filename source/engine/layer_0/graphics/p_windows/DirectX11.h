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
struct Light
{
    int type;
    XMFLOAT3 padding;
    XMFLOAT3 color;
    float padding2;
    float intensity;
    XMFLOAT3 padding3;
    XMFLOAT3 position;
    float padding4;
    XMFLOAT3 direction;
    float padding5;
    XMFLOAT3 attenuation;
    float padding6;
    float range;
    XMFLOAT3 padding7;
};

struct LightBuffer 
{
    Light lights[20];
    int numLights;
    XMFLOAT3 padding6;
};

class DirectX11 : public GraphicsAPI
{
    public:
    void Initialize();
    void UnInitialize();
    void BeginFrame(FrameUniform cmd);
    void DrawObject(ObjectUniform cmd);
    void EndFrame();

    GPUHandle CreateTexture(const TextureUpload& data);
    void DestroyTexture(GPUHandle handle);

    GPUHandle CreateShader(const ShaderUpload& data);
    void DestroyShader(GPUHandle handle);

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
    ID3D11DepthStencilState* depthStencilState = nullptr;
    ID3D11RasterizerState* rasterizerState = nullptr;

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

    const int MAX_LIGHTS = 20;
};
