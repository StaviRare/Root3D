#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "D3DCompiler.lib")

#include <d3dcompiler.h>

#include "DirectX11.h"
#include "Screen.h"
#include "Timer.h"
#include "Calc.h"
#include "Log.h"


void DirectX11::Initialize()
{
    HRESULT hr = E_FAIL;

    // Setup Device and Swap Chain
    HWND hwnd = reinterpret_cast<HWND>( const_cast<void*>( Screen::GetNativeHandle() ) );
    CreateDeviceAndSwapChain(hwnd);

    // Initialize Render Target and Viewport
    CreateRenderTargetView();
    SetupViewport(Screen::GetWidth(), Screen::GetHeight());

    // Setup Model-View-Projection (MVP) buffer
    XMMATRIX initialMVP = DirectX::XMMatrixIdentity();
    CreateBuffer(&initialMVP, sizeof(VPBuffer), D3D11_BIND_CONSTANT_BUFFER, &vpBuffer);
    CreateBuffer(&initialMVP, sizeof(MBuffer), D3D11_BIND_CONSTANT_BUFFER, &mBuffer);

    // Setup Depth Stencil Texture
    D3D11_TEXTURE2D_DESC descDepth = {};
    descDepth.Width = Screen::GetWidth();
    descDepth.Height = Screen::GetHeight();
    descDepth.MipLevels = 1;
    descDepth.ArraySize = 1;
    descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    descDepth.SampleDesc.Count = 1;
    descDepth.Usage = D3D11_USAGE_DEFAULT;
    descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    ID3D11Texture2D* depthStencilTexture = nullptr;

    if (FAILED(device->CreateTexture2D(&descDepth, nullptr, &depthStencilTexture)))
    {
        ENGINE_ERROR("Failed to create Depth Stencil Texture");
        return;
    }

    // Setup Depth Stencil View
    D3D11_DEPTH_STENCIL_VIEW_DESC descDSV = {};
    descDSV.Format = descDepth.Format;
    descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    descDSV.Texture2D.MipSlice = 0;

    if (FAILED(device->CreateDepthStencilView(depthStencilTexture, &descDSV, &depthStencilView)))
    {
        ENGINE_ERROR("Failed to create Depth Stencil View");
        depthStencilTexture->Release();
        return;
    }

    depthStencilTexture->Release();

    context->OMSetRenderTargets(1, &backBufferRTV, depthStencilView);

    // Configure and Set Depth Stencil State
    D3D11_DEPTH_STENCIL_DESC dsDesc = {true, D3D11_DEPTH_WRITE_MASK_ALL, D3D11_COMPARISON_LESS};
    hr = device->CreateDepthStencilState(&dsDesc, &depthStencilState);
    if (FAILED(hr))
    {
        ENGINE_ERROR("Failed to configure Depth Stencil State");
        return;
    }

    context->OMSetDepthStencilState(depthStencilState, 1);

    // Setup and Apply Rasterizer State
    D3D11_RASTERIZER_DESC rasterizerDesc = {D3D11_FILL_SOLID, D3D11_CULL_BACK, TRUE, TRUE};
    hr = device->CreateRasterizerState(&rasterizerDesc, &rasterizerState);
    if (FAILED(hr))
    {
        ENGINE_ERROR("Failed to configure Rasterizer State");
        return;
    }

    context->RSSetState(rasterizerState);

    // Setup Lighting Buffer
    CreateBuffer(&lightBufferData, sizeof(LightBuffer), D3D11_BIND_CONSTANT_BUFFER, &lightBuffer);

    initialized = true;
}

void DirectX11::UnInitialize()
{
    if (rasterizerState) rasterizerState->Release();
    if (depthStencilState) depthStencilState->Release();
    if (vpBuffer) vpBuffer->Release();
    if (mBuffer) mBuffer->Release();
    if (lightBuffer) lightBuffer->Release();
    if (normalBuffer) normalBuffer->Release();
    if (texCoordBuffer) texCoordBuffer->Release();
    if (indexBuffer) indexBuffer->Release();
    if (vertexBuffer) vertexBuffer->Release();
    if (depthStencilView) depthStencilView->Release();
    if (backBufferRTV) backBufferRTV->Release();
    if (swapChain) swapChain->Release();
    if (context) context->Release();
    if (device) device->Release();

    initialized = false;
}

void DirectX11::BeginFrame(FrameUniform onceCmd)
{
    const float* bg = onceCmd.backgroundColor;
    const float* viewMatrix = onceCmd.viewMatrix;
    const float* projectionMatrix = onceCmd.projectionMatrix;

    context->ClearRenderTargetView(backBufferRTV, bg);
    context->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

    // VPBuffer (b0)
    XMMATRIX vpData[2] =
    {
        DirectX::XMMatrixTranspose(XMMATRIX(onceCmd.viewMatrix)),
        DirectX::XMMatrixTranspose(XMMATRIX(onceCmd.projectionMatrix)),
    };
    context->UpdateSubresource(vpBuffer, 0, nullptr, &vpData, 0, 0);
    context->VSSetConstantBuffers(0, 1, &vpBuffer);

    // LightBuffer (b2)
    lightBufferData.numLights = 20;

    for (int i = 0; i < 20; i++)
    {
        LightUniform lightCommands = onceCmd.lights[i];

        lightBufferData.lights[i].type = lightCommands.type;
        lightBufferData.lights[i].color = XMFLOAT3(
            lightCommands.color[0],
            lightCommands.color[1],
            lightCommands.color[2]);

        lightBufferData.lights[i].intensity = lightCommands.intensity;
        lightBufferData.lights[i].direction = XMFLOAT3(
            lightCommands.direction[0],
            lightCommands.direction[1],
            lightCommands.direction[2]);

        lightBufferData.lights[i].range = lightCommands.range;
        lightBufferData.lights[i].position = XMFLOAT3(
            lightCommands.position[0],
            lightCommands.position[1],
            lightCommands.position[2]);

        lightBufferData.lights[i].attenuation = XMFLOAT3(
            lightCommands.attenuation[0],
            lightCommands.attenuation[1],
            lightCommands.attenuation[2]);
    }

    context->UpdateSubresource(lightBuffer, 0, nullptr, &lightBufferData, 0, 0);
    context->PSSetConstantBuffers(2, 1, &lightBuffer);
}

void DirectX11::DrawObject(ObjectUniform command)
{
    GPUShader* shaderProgram = nullptr;
    auto shaderIt = m_shaderMap.find(command.shaderHandle);
    if (shaderIt != m_shaderMap.end())
    {
        shaderProgram = &shaderIt->second;
    }

    if (shaderProgram != nullptr)
    {
        context->IASetInputLayout(shaderProgram->inputLayout);
        context->VSSetShader(shaderProgram->vertexShader, nullptr, 0);
        context->PSSetShader(shaderProgram->pixelShader, nullptr, 0);

        XMMATRIX mData[1] = {DirectX::XMMatrixTranspose(XMMATRIX(command.modelMatrix))};
        context->UpdateSubresource(mBuffer, 0, nullptr, mData, 0, 0);
        context->VSSetConstantBuffers(1, 1, &mBuffer);

        GPUTexture* gpuTexture = nullptr;
        auto texIt = m_textureMap.find(command.textureHandle);
        if (texIt != m_textureMap.end())
        {
            gpuTexture = &texIt->second;
        }

        ID3D11ShaderResourceView* srv[1] = {gpuTexture ? gpuTexture->textureView : nullptr};
        context->PSSetShaderResources(0, 1, srv);

        CreateBuffer(const_cast<int*>( command.mesh.indices ), sizeof(int) * command.mesh.indicesSize, D3D11_BIND_INDEX_BUFFER, &indexBuffer);
        CreateBuffer(const_cast<float*>( command.mesh.vertices ), sizeof(float) * command.mesh.verticesSize, D3D11_BIND_VERTEX_BUFFER, &vertexBuffer);
        CreateBuffer(const_cast<float*>( command.mesh.texCoords ), sizeof(float) * command.mesh.texCoordsSize, D3D11_BIND_VERTEX_BUFFER, &texCoordBuffer);
        CreateBuffer(const_cast<float*>( command.mesh.normals ), sizeof(float) * command.mesh.normalsSize, D3D11_BIND_VERTEX_BUFFER, &normalBuffer);

        UINT strides[3] = {sizeof(float) * 3, sizeof(float) * 3, sizeof(float) * 2};
        UINT offsets[3] = {0, 0, 0};
        ID3D11Buffer* buffers[3] = {vertexBuffer, normalBuffer, texCoordBuffer};
        context->IASetVertexBuffers(0, 3, buffers, strides, offsets);
        context->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
        context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        context->DrawIndexed(command.mesh.indicesSize, 0, 0);

        if (indexBuffer) indexBuffer->Release();
        if (vertexBuffer) vertexBuffer->Release();
        if (normalBuffer) normalBuffer->Release();
        if (texCoordBuffer) texCoordBuffer->Release();
    }
}

void DirectX11::EndFrame()
{
    swapChain->Present(0, 0);
}

uniqueID DirectX11::CreateShader(const ShaderUpload data)
{
    ID3DBlob* vsBlob = nullptr;
    CompileShader(data.vertexCode, "VS", "vs_4_0", &vsBlob);
    if (!vsBlob)
    {
        return 0;
    }

    ID3D11VertexShader* vertexShader = nullptr;
    HRESULT hr = device->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, &vertexShader);
    if (FAILED(hr))
    {
        vsBlob->Release(); return 0;
    }

    D3D11_INPUT_ELEMENT_DESC layout[] =
    {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 1, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 2, 0, D3D11_INPUT_PER_VERTEX_DATA, 0}
    };

    ID3D11InputLayout* inputLayout = nullptr;
    hr = device->CreateInputLayout(layout, ARRAYSIZE(layout), vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &inputLayout);
    vsBlob->Release();
    if (FAILED(hr))
    {
        return 0;
    }

    ID3DBlob* psBlob = nullptr;
    CompileShader(data.fragmentCode, "PS", "ps_4_0", &psBlob);
    if (!psBlob)
    {
        return 0;
    }

    ID3D11PixelShader* pixelShader = nullptr;
    hr = device->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, &pixelShader);
    psBlob->Release();
    if (FAILED(hr))
    {
        return 0;
    }

    nextShaderID++;
    GPUShader gpuShader{inputLayout, pixelShader, vertexShader};
    m_shaderMap[nextShaderID] = gpuShader;

    return nextShaderID;
}

void DirectX11::DestroyShader(uniqueID id)
{
    auto it = m_shaderMap.find(id);
    if (it != m_shaderMap.end())
    {
        if (it->second.vertexShader) it->second.vertexShader->Release();
        if (it->second.pixelShader) it->second.pixelShader->Release();
        if (it->second.inputLayout) it->second.inputLayout->Release();
        m_shaderMap.erase(it);
    }
}

uniqueID DirectX11::CreateTexture(const TextureUpload data)
{
    D3D11_TEXTURE2D_DESC desc{};
    desc.Width = data.width;
    desc.Height = data.height;
    desc.MipLevels = 1;
    desc.ArraySize = 1;
    desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.SampleDesc.Count = 1;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

    D3D11_SUBRESOURCE_DATA initData{};
    initData.pSysMem = data.rawData;
    initData.SysMemPitch = data.width * 4;

    ID3D11Texture2D* d3dTexture = nullptr;
    if (FAILED(device->CreateTexture2D(&desc, &initData, &d3dTexture)))
    {
        return 0;
    }

    ID3D11ShaderResourceView* textureView = nullptr;
    if (FAILED(device->CreateShaderResourceView(d3dTexture, nullptr, &textureView)))
    {
        d3dTexture->Release(); 
        return 0;
    }

    nextTextureID++;
    GPUTexture gpuTex{d3dTexture, textureView};
    m_textureMap[nextTextureID] = gpuTex;

    return nextTextureID;
}

void DirectX11::DestroyTexture(uniqueID id)
{
    auto it = m_textureMap.find(id);
    if (it != m_textureMap.end())
    {
        if (it->second.d3dTexture) it->second.d3dTexture->Release();
        if (it->second.textureView) it->second.textureView->Release();
        m_textureMap.erase(it);
    }
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
        ENGINE_ERROR("Failed to create buffer. Error code: " + std::to_string(hr));
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
        ENGINE_ERROR("Failed to create device and swap chain. Error code: " + std::to_string(hr));
    }
}

void DirectX11::CreateRenderTargetView()
{
    ID3D11Texture2D* pBackBuffer = nullptr;
    HRESULT hr = swapChain->GetBuffer(0, __uuidof( ID3D11Texture2D ), (void**) &pBackBuffer);

    if (FAILED(hr))
    {
        ENGINE_ERROR("Failed to create render target view. Error code: " + std::to_string(hr));
        return;
    }

    hr = device->CreateRenderTargetView(pBackBuffer, nullptr, &backBufferRTV);

    pBackBuffer->Release();

    if (FAILED(hr))
    {
        ENGINE_ERROR("Failed to create render target view. Error code: " + std::to_string(hr));
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

void DirectX11::CompileShader(const string& source, const char* entryPoint, const char* shaderModel, ID3DBlob** blobOut)
{
    ID3DBlob* errorBlob = nullptr;
    const char* src = source.c_str();
    HRESULT hr = D3DCompile(src, source.size(), nullptr, nullptr, nullptr, entryPoint, shaderModel, 0, 0, blobOut, &errorBlob);

    if (FAILED(hr))
    {
        string errorMessage = "Shader compilation failed. ";
        
        if (errorBlob != nullptr)
        {
            errorMessage += "Error Details: " + string(static_cast<const char*>(errorBlob->GetBufferPointer()), errorBlob->GetBufferSize());
            errorBlob->Release();
        }
        else
        {
            errorMessage += "No error details available.";
        }

        // Ensure the blobOut is null to indicate failure
        *blobOut = nullptr;

        ENGINE_ERROR(errorMessage);
    }

    if (errorBlob != nullptr)
    {
        errorBlob->Release();
    }
}