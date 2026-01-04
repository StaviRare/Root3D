#pragma once

#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <unordered_map>
#include "IGraphicsAPI.h"
#include "Types.h"

using DirectX::XMFLOAT3;
using DirectX::XMMATRIX;
using DirectX::XMMatrixIdentity;
using DirectX::XMMatrixTranspose;

struct DX11Shader
{
    ID3D11InputLayout* inputLayout = nullptr;
    ID3D11PixelShader* pixelShader = nullptr;
    ID3D11VertexShader* vertexShader = nullptr;
};

struct DX11Texture
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
    void Initialize(void* windowHandle) override;
    void UnInitialize() override;
    void Resize(uint32_t width, uint32_t height) override;
    void OnSurfaceLost() override;
    void OnSurfaceRecreated(void* windowHandle) override;
    void BeginFrame(FrameUniform cmd) override;
    void DrawObject(ObjectUniform cmd) override;
    void EndFrame() override;
    void DestroyShader(uniqueID id) override;
    void DestroyTexture(uniqueID id) override;
    uniqueID CreateShader(const ShaderUpload data) override;
    uniqueID CreateTexture(const TextureUpload data) override;

    private:
    void CreateDeviceAndSwapChain(HWND hwnd);
    void CreateRenderTargetView();
    void SetupViewport(UINT width, UINT height);
    ID3D11Buffer* CreateBuffer(void* data, UINT size, D3D11_BIND_FLAG bindFlag);
    ID3DBlob* CompileShader(const string& source, const char* entryPoint, const char* shaderModel);

    private:
    ID3D11Buffer* m_vertexBuffer = nullptr;
    ID3D11Buffer* m_indexBuffer = nullptr;
    ID3D11Buffer* m_texCoordBuffer = nullptr;
    ID3D11Buffer* m_normalBuffer = nullptr;
    ID3D11Buffer* m_viewProjBuffer = nullptr;
    ID3D11Buffer* m_modelBuffer = nullptr;
    ID3D11Buffer* m_lightBuffer = nullptr;
    ID3D11Device* m_device = nullptr;
    ID3D11DeviceContext* m_context = nullptr;
    IDXGISwapChain* m_swapChain = nullptr;
    ID3D11RenderTargetView* m_backBufferRTV = nullptr;
    ID3D11DepthStencilView* m_depthStencilView = nullptr;
    ID3D11RasterizerState* m_rasterizerState = nullptr;
    ID3D11DepthStencilState* m_depthStencilState = nullptr;
    uniqueID m_nextShaderID = 0;
    uniqueID m_nextTextureID = 0;
    std::unordered_map<uniqueID, DX11Shader> m_shaderMap;
    std::unordered_map<uniqueID, DX11Texture> m_textureMap;
};
