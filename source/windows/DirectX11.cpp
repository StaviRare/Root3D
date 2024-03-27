#include <d3dcompiler.h>
#include "DirectX11.h"
#include "Screen.h"
#include "Debug.h"
#include "Timer.h"
#include "RenderQueue.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "D3DCompiler.lib")


const char* vertexShaderSource = R"(
cbuffer ConstantBuffer : register(b0) {
    matrix worldViewProj;
};

struct VS_INPUT {
    float3 Pos : POSITION;
    float4 Col : COLOR;
    float2 Tex : TEXCOORD0;
};

struct PS_INPUT {
    float4 Pos : SV_POSITION;
    float4 Col : COLOR;
    float2 Tex : TEXCOORD0;
};

PS_INPUT VS(VS_INPUT input) {
    PS_INPUT output;
    output.Pos = mul(float4(input.Pos, 1.0), worldViewProj);
    output.Col = input.Col;
    output.Tex = input.Tex;
    return output;
}
)";

// Simple pixel shader source code
const char* pixelShaderSource = R"(
Texture2D ObjTexture : register(t0);
SamplerState ObjSampler : register(s0);

struct PS_INPUT {
    float4 Pos : SV_POSITION;
    float2 Tex : TEXCOORD0;
};

float4 PS(PS_INPUT input) : SV_Target {
    return ObjTexture.Sample(ObjSampler, input.Tex);
}
)";

void DirectX11::Initialize()
{
    HRESULT hr = E_FAIL; // for safety.

    void* nativeHandle = Screen::GetNativeHandle();
    HWND hwnd = reinterpret_cast<HWND>(const_cast<void*>(nativeHandle));

    CreateDeviceAndSwapChain(hwnd);
    CreateRenderTargetView();
    SetupViewport(Screen::GetWidth(), Screen::GetHeight());
    CreateShadersAndInputLayout();

    DirectX::XMMATRIX initialData = DirectX::XMMatrixIdentity();
    CreateBuffer(&initialData, sizeof(DirectX::XMMATRIX), D3D11_BIND_CONSTANT_BUFFER, &constantBuffer);

    // Create a Depth Stencil Texture
    ID3D11Texture2D* depthStencilTexture = nullptr;
    D3D11_TEXTURE2D_DESC descDepth = {};
    descDepth.Width = Screen::GetWidth();
    descDepth.Height = Screen::GetHeight();
    descDepth.MipLevels = 1;
    descDepth.ArraySize = 1;
    descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    descDepth.SampleDesc.Count = 1;
    descDepth.SampleDesc.Quality = 0;
    descDepth.Usage = D3D11_USAGE_DEFAULT;
    descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    hr = device->CreateTexture2D(&descDepth, nullptr, &depthStencilTexture);
    
    if (FAILED(hr)) 
    {         
        Debug::LogError("Create a Depth Stencil Texture ERROR");
    }

    // Create a Depth Stencil View
    D3D11_DEPTH_STENCIL_VIEW_DESC descDSV = {};
    descDSV.Format = descDepth.Format;
    descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    descDSV.Texture2D.MipSlice = 0;
    hr = device->CreateDepthStencilView(depthStencilTexture, &descDSV, &depthStencilView);
    
    if (FAILED(hr)) 
    {        
        Debug::LogError("Create a Depth Stencil View ERROR");
    }

    depthStencilTexture->Release(); // no longer needed

    // Bind the Depth Stencil View
    context->OMSetRenderTargets(1, &backBufferRTV, depthStencilView);

    // Configure Depth Stencil State
    D3D11_DEPTH_STENCIL_DESC dsDesc = {};
    dsDesc.DepthEnable = true;
    dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    dsDesc.DepthFunc = D3D11_COMPARISON_LESS;

    ID3D11DepthStencilState* depthStencilState = nullptr;
    hr = device->CreateDepthStencilState(&dsDesc, &depthStencilState);
    
    if (FAILED(hr))
    {
        Debug::LogError("Configure Depth Stencil State ERROR");
    }

    context->OMSetDepthStencilState(depthStencilState, 1);

    D3D11_RASTERIZER_DESC rasterizerDesc = {};
    rasterizerDesc.FillMode = D3D11_FILL_SOLID;
    rasterizerDesc.CullMode = D3D11_CULL_BACK;
    rasterizerDesc.FrontCounterClockwise = TRUE;
    rasterizerDesc.DepthClipEnable = TRUE;
    ID3D11RasterizerState* rasterizerState = nullptr;
    device->CreateRasterizerState(&rasterizerDesc, &rasterizerState);
    context->RSSetState(rasterizerState);
    rasterizerState->Release();

    initialized = true;
}

void DirectX11::ClearScreen()
{
    context->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
}

void DirectX11::ExecuteRenderCommands()
{
    if (initialized)
    {
        const GlobalRenderCommand* onceCmd = RenderQueue::GetGlobalRenderCommand();

        if (onceCmd)
        {
            const float* bg = onceCmd->backgroundColor;
            const float* vm = onceCmd->viewMatrix;
            const float* pm = onceCmd->projectionMatrix;

            context->ClearRenderTargetView(backBufferRTV, bg);
            
            DirectX::XMMATRIX viewMatrix = DirectX::XMMATRIX(vm);
            DirectX::XMMATRIX projectionMatrix = DirectX::XMMATRIX(pm);

            auto objectCommands = RenderQueue::GetObjectRenderCommands();

            for (const auto& command : objectCommands)
            {
                DirectX::XMMATRIX modelMatrix = DirectX::XMMATRIX(command.modelMatrix);
                DirectX::XMMATRIX worldViewProj = modelMatrix * viewMatrix * projectionMatrix;
                worldViewProj = DirectX::XMMatrixTranspose(worldViewProj);

                context->UpdateSubresource(constantBuffer, 0, nullptr, &worldViewProj, 0, 0);
                context->VSSetConstantBuffers(0, 1, &constantBuffer);

                // Set pipeline state
                context->IASetInputLayout(inputLayout);
                context->VSSetShader(vertexShader, nullptr, 0);
                context->PSSetShader(pixelShader, nullptr, 0);

                if (command.texture)
                {
                    BindTexture(*( command.texture ));
                }

                CreateBuffer(const_cast<int*>(command.indices), sizeof(int) * command.indicesSize, D3D11_BIND_INDEX_BUFFER, &indexBuffer);
                CreateBuffer(const_cast<float*>(command.vertices), sizeof(float) * 3 * command.verticesSize, D3D11_BIND_VERTEX_BUFFER, &vertexBuffer);
                CreateBuffer(const_cast<float*>(command.texCoords), sizeof(float) * 2 * command.texCoordsSize, D3D11_BIND_VERTEX_BUFFER, &texCoordBuffer);

                UINT strides[2] = { sizeof(float) * 3, sizeof(float) * 2 }; // Update stride for texture coordinates
                UINT offsets[2] = { 0, 0 };
                ID3D11Buffer* buffers[2] = { vertexBuffer, texCoordBuffer };

                context->IASetVertexBuffers(0, 2, buffers, strides, offsets);
                context->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
                context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
                context->DrawIndexed(command.indicesSize, 0, 0);

                if (indexBuffer) indexBuffer->Release();
                if (vertexBuffer) vertexBuffer->Release();
                if (texCoordBuffer) texCoordBuffer->Release();
            }
        }

        RenderQueue::Clear();
    }
}

void DirectX11::SwapFrameBuffers()
{
    swapChain->Present(0, 0);
}

void DirectX11::UnInitialize()
{
    if (constantBuffer) constantBuffer->Release();
    if (inputLayout) inputLayout->Release();
    if (pixelShader) pixelShader->Release();
    if (vertexShader) vertexShader->Release();
    if (backBufferRTV) backBufferRTV->Release();
    if (swapChain) swapChain->Release();
    if (context) context->Release();
    if (device) device->Release();
    if (depthStencilView) depthStencilView->Release();

    initialized = false;
}

void DirectX11::BindTexture(Texture& texture)
{
    // Create texture description
    D3D11_TEXTURE2D_DESC desc;
    ZeroMemory(&desc, sizeof(desc));
    desc.Width = texture.width;
    desc.Height = texture.height;
    desc.MipLevels = 1;
    desc.ArraySize = 1;
    desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.SampleDesc.Count = 1;
    desc.SampleDesc.Quality = 0;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    desc.CPUAccessFlags = 0;
    desc.MiscFlags = 0;

    // Create texture data
    D3D11_SUBRESOURCE_DATA initData;
    ZeroMemory(&initData, sizeof(initData));
    initData.pSysMem = texture.rawData;
    initData.SysMemPitch = texture.width * 4; // Assuming 4 bytes per pixel for RGBA

    // Create texture
    ID3D11Texture2D* d3dTexture = nullptr;
    HRESULT hr = device->CreateTexture2D(&desc, &initData, &d3dTexture);
    if (FAILED(hr))
    {
        // Handle error
        Debug::LogError("Failed to create texture.");
        return;
    }

    // Create shader resource view
    ID3D11ShaderResourceView* textureView = nullptr;
    hr = device->CreateShaderResourceView(d3dTexture, nullptr, &textureView);
    if (FAILED(hr))
    {
        // Handle error
        Debug::LogError("Failed to create shader resource view.");
        d3dTexture->Release();
        return;
    }

    // Bind the texture view to slot 0 of the pixel shader
    context->PSSetShaderResources(0, 1, &textureView);
    //Debug::Log("Texture bound successfully.");

    // Release the resources
    textureView->Release();
    d3dTexture->Release();
}


void DirectX11::CreateBuffer(void* data, UINT size, D3D11_BIND_FLAG bindFlag, ID3D11Buffer** buffer)
{
    D3D11_BUFFER_DESC desc = {};
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.ByteWidth = size;
    desc.BindFlags = bindFlag;
    desc.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA initData = {};
    initData.pSysMem = data;

    HRESULT hr = device->CreateBuffer(&desc, &initData, buffer);

    if (FAILED(hr))
    {
        Debug::LogError("Failed to create buffer.");
    }
}

void DirectX11::CreateDeviceAndSwapChain(HWND hwnd)
{
    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferCount = 1;
    sd.BufferDesc.Width = Screen::GetWidth();
    sd.BufferDesc.Height = Screen::GetHeight();
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hwnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;

    D3D_FEATURE_LEVEL featureLevels[] = {D3D_FEATURE_LEVEL_11_0};
    D3D_FEATURE_LEVEL featureLevel;

    HRESULT hr = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0,
        featureLevels, ARRAYSIZE(featureLevels), D3D11_SDK_VERSION, &sd,
        &swapChain, &device, &featureLevel, &context);

    if (FAILED(hr))
    {

    }
}

void DirectX11::CreateRenderTargetView()
{
    ID3D11Texture2D* pBackBuffer = nullptr;
    HRESULT hr = swapChain->GetBuffer(0, __uuidof( ID3D11Texture2D ), (void**) &pBackBuffer);

    if (FAILED(hr))
    {

        return;
    }

    hr = device->CreateRenderTargetView(pBackBuffer, nullptr, &backBufferRTV);

    pBackBuffer->Release();

    if (FAILED(hr))
    {

        return;
    }

    context->OMSetRenderTargets(1, &backBufferRTV, nullptr);
}

void DirectX11::SetupViewport(UINT width, UINT height)
{
    D3D11_VIEWPORT vp;
    ZeroMemory(&vp, sizeof(D3D11_VIEWPORT));
    vp.Width = (FLOAT) width;
    vp.Height = (FLOAT) height;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    context->RSSetViewports(1, &vp);
}

void DirectX11::CreateShadersAndInputLayout()
{
    // Compile vertex shader
    ID3DBlob* vsBlob = nullptr;
    CompileShaderFromSource(vertexShaderSource, "VS", "vs_4_0", &vsBlob);
    
    if (!vsBlob)
    {
        Debug::LogError("Failed to compile vertex shader.");
        return;
    }

    // Create vertex shader
    HRESULT hr = device->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, &vertexShader);
    
    if (FAILED(hr))
    {
        Debug::LogError("Failed to create vertex shader.");
        vsBlob->Release();
        return;
    }

    // Define input layout for vertex data
    D3D11_INPUT_ELEMENT_DESC layout[] = 
    {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 2, 0, D3D11_INPUT_PER_VERTEX_DATA, 0}
    };

    // Create input layout
    hr = device->CreateInputLayout(layout, ARRAYSIZE(layout), vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &inputLayout);
    vsBlob->Release(); // Release the vertex shader blob after creating the input layout

    if (FAILED(hr))
    {
        Debug::LogError("Failed to create input layout.");
        return;
    }

    // Compile pixel shader
    ID3DBlob* psBlob = nullptr;
    CompileShaderFromSource(pixelShaderSource, "PS", "ps_4_0", &psBlob);
    
    if (!psBlob)
    {
        Debug::LogError("Failed to compile pixel shader.");
        return;
    }

    // Create pixel shader
    hr = device->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, &pixelShader);
    psBlob->Release();
    
    if (FAILED(hr))
    {
        Debug::LogError("Failed to create pixel shader.");
        return;
    }
}

void DirectX11::CompileShaderFromSource(const char* source, const char* entryPoint, const char* shaderModel, ID3DBlob** blobOut)
{
    ID3DBlob* errorBlob = nullptr;
    HRESULT hr = D3DCompile(source, strlen(source), nullptr, nullptr, nullptr, entryPoint, shaderModel, 0, 0, blobOut, &errorBlob);

    if (FAILED(hr))
    {
        if (errorBlob)
        {
            OutputDebugStringA(reinterpret_cast<const char*>( errorBlob->GetBufferPointer() ));
            errorBlob->Release();
        }

        *blobOut = nullptr; // return null blob on failure
    }

    if (errorBlob) errorBlob->Release();
}