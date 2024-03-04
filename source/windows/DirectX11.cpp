#include <d3dcompiler.h>
#include "DirectX11.h"
#include "Screen.h"
#include "Debug.h"
#include "Timer.h"
#include "RenderQueue.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "D3DCompiler.lib")

static float debugRotation;


struct Vertex
{
    DirectX::XMFLOAT3 position;
    DirectX::XMFLOAT4 color;
};

struct ConstantBuffer
{
    DirectX::XMMATRIX worldViewProj;
};

// Simple vertex shader source code
const char* vertexShaderSource = R"(
cbuffer ConstantBuffer : register(b0) {
    matrix worldViewProj;
};
struct VS_INPUT {
    float3 Pos : POSITION;
    float4 Col : COLOR;
};
struct PS_INPUT {
    float4 Pos : SV_POSITION;
    float4 Col : COLOR;
};
PS_INPUT VS(VS_INPUT input) {
    PS_INPUT output;
    output.Pos = mul(float4(input.Pos, 1.0), worldViewProj);
    output.Col = input.Col;
    return output;
}
)";

// Simple pixel shader source code
const char* pixelShaderSource = R"(
struct PS_INPUT {
    float4 Pos : SV_POSITION;
    float4 Col : COLOR;
};
float4 PS(PS_INPUT input) : SV_Target {
    return input.Col;
}
)";


void DirectX11::Initialize()
{
    void* nativeHandle = Screen::GetNativeHandle();
    HWND hwnd = reinterpret_cast<HWND>(const_cast<void*>(nativeHandle));

    CreateDeviceAndSwapChain(hwnd);
    CreateRenderTargetView();
    SetupViewport(Screen::GetWidth(), Screen::GetHeight());
    CreateShadersAndInputLayout();

    // Create the constant buffer for the worldViewProj matrix
    D3D11_BUFFER_DESC bd = {};
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(ConstantBuffer);
    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bd.CPUAccessFlags = 0;
    
    HRESULT hr = device->CreateBuffer(&bd, nullptr, &constantBuffer);
    
    if (FAILED(hr))
    {
        // Handle error
    }


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
    //if (initialized)
    //{
    //    const float clearColor[4] = {0.0f, 0.2f, 0.4f, 1.0f};
    //    context->ClearRenderTargetView(backBufferRTV, clearColor);
    //}
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

                // Define vertices
                Vertex vertices[] =
                {
                    {DirectX::XMFLOAT3(-0.5f, -0.5f, 0.0f), DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f)}, // Top left
                    {DirectX::XMFLOAT3(0.5f, -0.5f, 0.0f), DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f)},   // Top right
                    {DirectX::XMFLOAT3(0.5f, 0.5f, 0.0f), DirectX::XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f)}, // Bottom left
                    {DirectX::XMFLOAT3(-0.5f, 0.5f, 0.0f), DirectX::XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f)}    // Bottom right
                };

                // Define indices for triangle strip
                std::vector<unsigned short> indices = { 0, 1, 2, 2, 3, 0 };

                // Create a buffer for vertices
                D3D11_BUFFER_DESC vertexBufferDesc = {};
                vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
                vertexBufferDesc.ByteWidth = sizeof(vertices);
                vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

                D3D11_SUBRESOURCE_DATA vertexData = {};
                vertexData.pSysMem = vertices;

                ID3D11Buffer* frameVertexBuffer = nullptr;
                HRESULT hr = device->CreateBuffer(&vertexBufferDesc, &vertexData, &frameVertexBuffer);
                if (FAILED(hr))
                {
                    // Handle error
                    return;
                }

                // Set the vertex buffer
                UINT stride = sizeof(Vertex);
                UINT offset = 0;
                context->IASetVertexBuffers(0, 1, &frameVertexBuffer, &stride, &offset);

                // Create a buffer for indices
                D3D11_BUFFER_DESC indexBufferDesc = {};
                indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
                indexBufferDesc.ByteWidth = sizeof(unsigned short) * indices.size();
                indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

                D3D11_SUBRESOURCE_DATA indexData = {};
                indexData.pSysMem = indices.data();

                ID3D11Buffer* indexBuffer = nullptr;
                hr = device->CreateBuffer(&indexBufferDesc, &indexData, &indexBuffer);
                if (FAILED(hr))
                {
                    // Handle error
                    return;
                }

                // Set the index buffer
                context->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R16_UINT, 0);

                // Set primitive topology
                context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

                // Draw the indexed vertices
                context->DrawIndexed(indices.size(), 0, 0);

                // Release resources
                if (frameVertexBuffer) frameVertexBuffer->Release();
                if (indexBuffer) indexBuffer->Release();
            }
        }

        swapChain->Present(0, 0);
    }
}

void DirectX11::UnInitialize()
{
    if (constantBuffer) constantBuffer->Release();
    if (vertexBuffer) vertexBuffer->Release();
    if (inputLayout) inputLayout->Release();
    if (pixelShader) pixelShader->Release();
    if (vertexShader) vertexShader->Release();
    if (backBufferRTV) backBufferRTV->Release();
    if (swapChain) swapChain->Release();
    if (context) context->Release();
    if (device) device->Release();
    initialized = false;
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
        // Handle error
    }
}

void DirectX11::CreateRenderTargetView()
{
    ID3D11Texture2D* pBackBuffer = nullptr;
    HRESULT hr = swapChain->GetBuffer(0, __uuidof( ID3D11Texture2D ), (void**) &pBackBuffer);
    
    if (FAILED(hr))
    {
        // Handle error
        return;
    }
    
    hr = device->CreateRenderTargetView(pBackBuffer, nullptr, &backBufferRTV);
    
    pBackBuffer->Release();
    
    if (FAILED(hr))
    {
        // Handle error
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
    ID3DBlob* vsBlob = nullptr;
    CompileShaderFromSource(vertexShaderSource, "VS", "vs_4_0", &vsBlob);
    device->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, &vertexShader);

    D3D11_INPUT_ELEMENT_DESC layout[] =
    {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
    };

    UINT numElements = ARRAYSIZE(layout);
    device->CreateInputLayout(layout, numElements, vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &inputLayout);
    vsBlob->Release(); // Release the vertex shader blob after creating the input layout

    ID3DBlob* psBlob = nullptr;
    CompileShaderFromSource(pixelShaderSource, "PS", "ps_4_0", &psBlob);
    device->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, &pixelShader);
    psBlob->Release();
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
        // Handle or log the error more extensively here if needed
    }

    if (errorBlob) errorBlob->Release();
}