#pragma once

#include <unordered_map>
#include <d3d11.h>
#include <DirectXMath.h>
#include "IGraphicsAPI.h"
#include "Texture.h"
#include "Types.h"

using DirectX::XMFLOAT3;
using DirectX::XMMATRIX;
using DirectX::XMMatrixIdentity;
using DirectX::XMMatrixTranspose;

struct GPUShader
{
    ID3D11InputLayout* inputLayout = nullptr;
    ID3D11PixelShader* pixelShader = nullptr;
    ID3D11VertexShader* vertexShader = nullptr;
};

struct GPUTexture
{
    ID3D11Texture2D* d3dTexture = nullptr;
    ID3D11ShaderResourceView* textureView = nullptr;
};

struct VPBuffer
{
    XMMATRIX view;
    XMMATRIX projection;
};

struct MBuffer
{
    XMMATRIX model;
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

class DirectX11 : public IGraphicsAPI
{
    public:
    void Initialize() override;
    void UnInitialize() override;
    void BeginFrame(FrameUniform cmd) override;
    void DrawObject(ObjectUniform cmd) override;
    void EndFrame();
    void DestroyShader(uniqueID handle);
    void DestroyTexture(uniqueID handle);
    uniqueID CreateShader(const ShaderUpload data);
    uniqueID CreateTexture(const TextureUpload data);

    private:
    void CreateDeviceAndSwapChain(HWND hwnd);
    void CreateRenderTargetView();
    void SetupViewport(UINT width, UINT height);
    void CreateBuffer(void* data, UINT size, D3D11_BIND_FLAG bindFlag, ID3D11Buffer** buffer);
    void CompileShader(const string& source, const char* entryPoint, const char* shaderModel, ID3DBlob** blobOut);

    private:
    ID3D11Buffer* vertexBuffer = nullptr;
    ID3D11Buffer* indexBuffer = nullptr;
    ID3D11Buffer* texCoordBuffer = nullptr;
    ID3D11Buffer* normalBuffer = nullptr;
    ID3D11Buffer* vpBuffer = nullptr;
    ID3D11Buffer* mBuffer = nullptr;
    ID3D11Buffer* lightBuffer = nullptr;
    ID3D11Device* device = nullptr;
    ID3D11DeviceContext* context = nullptr;
    IDXGISwapChain* swapChain = nullptr;
    ID3D11RenderTargetView* backBufferRTV = nullptr;
    ID3D11DepthStencilView* depthStencilView = nullptr;
    ID3D11DepthStencilState* depthStencilState = nullptr;
    ID3D11RasterizerState* rasterizerState = nullptr;

    bool initialized = false;
    unsigned int nextShaderID = 0;
    unsigned int nextTextureID = 0;
    std::unordered_map<uniqueID, GPUShader> m_shaderMap;
    std::unordered_map<uniqueID, GPUTexture> m_textureMap;
};
