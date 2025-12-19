#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "D3DCompiler.lib")

#include "DirectX11.h"
#include "Log.h"

void DirectX11::Initialize(void* windowHandle)
{
    HRESULT hr = E_FAIL;

    // Setup Device and Swap Chain
    HWND hwnd = reinterpret_cast<HWND>( const_cast<void*>( windowHandle ) );
    CreateDeviceAndSwapChain(hwnd);

    // Initialize Render Target and Viewport
    CreateRenderTargetView();


    RECT rect;
    if (GetClientRect(hwnd, &rect))
    {
        auto width = rect.right - rect.left;
        auto height = rect.bottom - rect.top;

        SetupViewport(width, height);


        // Create view/projection buffer
        VPBuffer initialVP{XMMatrixIdentity(), XMMatrixIdentity()};
        m_viewProjBuffer = CreateBuffer(&initialVP, sizeof(VPBuffer), D3D11_BIND_CONSTANT_BUFFER);

        // Create model buffer
        MBuffer initialM{XMMatrixIdentity()};
        m_modelBuffer = CreateBuffer(&initialM, sizeof(MBuffer), D3D11_BIND_CONSTANT_BUFFER);

        // Create light buffer
        LightBuffer initialLight{};
        m_lightBuffer = CreateBuffer(&initialLight, sizeof(LightBuffer), D3D11_BIND_CONSTANT_BUFFER);

        // Setup Depth Stencil Texture
        D3D11_TEXTURE2D_DESC descDepth = {};
        descDepth.Width = width;
        descDepth.Height = height;
        descDepth.MipLevels = 1;
        descDepth.ArraySize = 1;
        descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
        descDepth.SampleDesc.Count = 1;
        descDepth.Usage = D3D11_USAGE_DEFAULT;
        descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
        ID3D11Texture2D* depthStencilTexture = nullptr;

        if (FAILED(m_device->CreateTexture2D(&descDepth, nullptr, &depthStencilTexture)))
        {
            ENGINE_ERROR("Failed to create Depth Stencil Texture");
            return;
        }

        // Setup Depth Stencil View
        D3D11_DEPTH_STENCIL_VIEW_DESC descDSV = {};
        descDSV.Format = descDepth.Format;
        descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
        descDSV.Texture2D.MipSlice = 0;

        if (FAILED(m_device->CreateDepthStencilView(depthStencilTexture, &descDSV, &m_depthStencilView)))
        {
            ENGINE_ERROR("Failed to create Depth Stencil View");
            depthStencilTexture->Release();
            return;
        }

        depthStencilTexture->Release();

        m_context->OMSetRenderTargets(1, &m_backBufferRTV, m_depthStencilView);

        // Configure and Set Depth Stencil State
        D3D11_DEPTH_STENCIL_DESC dsDesc = {true, D3D11_DEPTH_WRITE_MASK_ALL, D3D11_COMPARISON_LESS};
        hr = m_device->CreateDepthStencilState(&dsDesc, &m_depthStencilState);
        if (FAILED(hr))
        {
            ENGINE_ERROR("Failed to configure Depth Stencil State");
            return;
        }

        m_context->OMSetDepthStencilState(m_depthStencilState, 1);

        // Setup and Apply Rasterizer State
        D3D11_RASTERIZER_DESC rasterizerDesc = {D3D11_FILL_SOLID, D3D11_CULL_BACK, TRUE, TRUE};
        hr = m_device->CreateRasterizerState(&rasterizerDesc, &m_rasterizerState);
        if (FAILED(hr))
        {
            ENGINE_ERROR("Failed to configure Rasterizer State");
            return;
        }

        m_context->RSSetState(m_rasterizerState);

        m_initialized = true;
    }
}

void DirectX11::UnInitialize()
{
    if (m_rasterizerState) m_rasterizerState->Release();
    if (m_depthStencilState) m_depthStencilState->Release();
    if (m_viewProjBuffer) m_viewProjBuffer->Release();
    if (m_modelBuffer) m_modelBuffer->Release();
    if (m_lightBuffer) m_lightBuffer->Release();
    if (m_normalBuffer) m_normalBuffer->Release();
    if (m_texCoordBuffer) m_texCoordBuffer->Release();
    if (m_indexBuffer) m_indexBuffer->Release();
    if (m_vertexBuffer) m_vertexBuffer->Release();
    if (m_depthStencilView) m_depthStencilView->Release();
    if (m_backBufferRTV) m_backBufferRTV->Release();
    if (m_swapChain) m_swapChain->Release();
    if (m_context) m_context->Release();
    if (m_device) m_device->Release();

    m_initialized = false;
}

void DirectX11::BeginFrame(FrameUniform cmd)
{
    const float* bg = cmd.backgroundColor;
    const float* viewMatrix = cmd.viewMatrix;
    const float* projectionMatrix = cmd.projectionMatrix;

    m_context->ClearRenderTargetView(m_backBufferRTV, bg);
    m_context->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

    // VPBuffer (b0)
    VPBuffer vpData;
    vpData.view = XMMatrixTranspose(XMMATRIX(cmd.viewMatrix));
    vpData.projection = XMMatrixTranspose(XMMATRIX(cmd.projectionMatrix));
    m_context->UpdateSubresource(m_viewProjBuffer, 0, nullptr, &vpData, 0, 0);
    m_context->VSSetConstantBuffers(0, 1, &m_viewProjBuffer);

    // LightBuffer (b2)
    LightBuffer lightData;
    size_t numLights = sizeof(cmd.lights) / sizeof(cmd.lights[0]);
    lightData.numLights = numLights;

    for (int i = 0; i < numLights; i++)
    {
        LightUniform lightCommands = cmd.lights[i];

        lightData.lights[i].type = lightCommands.type;
        lightData.lights[i].color = XMFLOAT3(
            lightCommands.color[0],
            lightCommands.color[1],
            lightCommands.color[2]);

        lightData.lights[i].intensity = lightCommands.intensity;
        lightData.lights[i].direction = XMFLOAT3(
            lightCommands.direction[0],
            lightCommands.direction[1],
            lightCommands.direction[2]);

        lightData.lights[i].range = lightCommands.range;
        lightData.lights[i].position = XMFLOAT3(
            lightCommands.position[0],
            lightCommands.position[1],
            lightCommands.position[2]);

        lightData.lights[i].attenuation = XMFLOAT3(
            lightCommands.attenuation[0],
            lightCommands.attenuation[1],
            lightCommands.attenuation[2]);
    }

    m_context->UpdateSubresource(m_lightBuffer, 0, nullptr, &lightData, 0, 0);
    m_context->PSSetConstantBuffers(2, 1, &m_lightBuffer);
}

void DirectX11::DrawObject(ObjectUniform cmd)
{
    DX11Shader* shaderProgram = nullptr;
    auto shaderIt = m_shaderMap.find(cmd.shaderHandle);
    if (shaderIt != m_shaderMap.end())
    {
        shaderProgram = &shaderIt->second;
    }

    if (shaderProgram != nullptr)
    {
        m_context->IASetInputLayout(shaderProgram->inputLayout);
        m_context->VSSetShader(shaderProgram->vertexShader, nullptr, 0);
        m_context->PSSetShader(shaderProgram->pixelShader, nullptr, 0);

        MBuffer mData;
        mData.model = XMMatrixTranspose(XMMATRIX(cmd.modelMatrix));
        m_context->UpdateSubresource(m_modelBuffer, 0, nullptr, &mData, 0, 0);
        m_context->VSSetConstantBuffers(1, 1, &m_modelBuffer);

        DX11Texture* gpuTexture = nullptr;
        auto texIt = m_textureMap.find(cmd.textureHandle);
        if (texIt != m_textureMap.end())
        {
            gpuTexture = &texIt->second;
        }

        ID3D11ShaderResourceView* srv[1] = {gpuTexture ? gpuTexture->textureView : nullptr};
        m_context->PSSetShaderResources(0, 1, srv);

        m_indexBuffer = CreateBuffer(const_cast<int*>( cmd.mesh.indices ), sizeof(int) * cmd.mesh.indicesSize, D3D11_BIND_INDEX_BUFFER);
        m_vertexBuffer = CreateBuffer(const_cast<float*>( cmd.mesh.vertices ), sizeof(float) * cmd.mesh.verticesSize, D3D11_BIND_VERTEX_BUFFER);
        m_texCoordBuffer = CreateBuffer(const_cast<float*>( cmd.mesh.texCoords ), sizeof(float) * cmd.mesh.texCoordsSize, D3D11_BIND_VERTEX_BUFFER);
        m_normalBuffer = CreateBuffer(const_cast<float*>( cmd.mesh.normals ), sizeof(float) * cmd.mesh.normalsSize, D3D11_BIND_VERTEX_BUFFER);

        UINT strides[3] = {sizeof(float) * 3, sizeof(float) * 3, sizeof(float) * 2};
        UINT offsets[3] = {0, 0, 0};
        ID3D11Buffer* buffers[3] = {m_vertexBuffer, m_normalBuffer, m_texCoordBuffer};
        m_context->IASetVertexBuffers(0, 3, buffers, strides, offsets);
        m_context->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);
        m_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        m_context->DrawIndexed(cmd.mesh.indicesSize, 0, 0);

        if (m_indexBuffer) m_indexBuffer->Release();
        if (m_vertexBuffer) m_vertexBuffer->Release();
        if (m_normalBuffer) m_normalBuffer->Release();
        if (m_texCoordBuffer) m_texCoordBuffer->Release();
    }
}

void DirectX11::EndFrame()
{
    m_swapChain->Present(0, 0);
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

uniqueID DirectX11::CreateShader(const ShaderUpload data)
{
    ID3DBlob* vsBlob = CompileShader(data.vertexCode, "VS", "vs_4_0");

    if (!vsBlob)
    {
        return 0;
    }

    ID3D11VertexShader* vertexShader = nullptr;
    HRESULT hr = m_device->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, &vertexShader);
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
    hr = m_device->CreateInputLayout(layout, ARRAYSIZE(layout), vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &inputLayout);
    vsBlob->Release();
    if (FAILED(hr))
    {
        return 0;
    }

    ID3DBlob* psBlob = CompileShader(data.fragmentCode, "PS", "ps_4_0");

    if (!psBlob)
    {
        return 0;
    }

    ID3D11PixelShader* pixelShader = nullptr;
    hr = m_device->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, &pixelShader);
    psBlob->Release();
    if (FAILED(hr))
    {
        return 0;
    }

    m_nextShaderID++;
    DX11Shader gpuShader{inputLayout, pixelShader, vertexShader};
    m_shaderMap[m_nextShaderID] = gpuShader;

    return m_nextShaderID;
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
    if (FAILED(m_device->CreateTexture2D(&desc, &initData, &d3dTexture)))
    {
        return 0;
    }

    ID3D11ShaderResourceView* textureView = nullptr;
    if (FAILED(m_device->CreateShaderResourceView(d3dTexture, nullptr, &textureView)))
    {
        d3dTexture->Release(); 
        return 0;
    }

    // Store it
    m_nextTextureID++;
    DX11Texture gpuTex{d3dTexture, textureView};
    m_textureMap[m_nextTextureID] = gpuTex;

    return m_nextTextureID;
}

ID3D11Buffer* DirectX11::CreateBuffer(void* data, UINT size, D3D11_BIND_FLAG bindFlag)
{
    ID3D11Buffer* returnValue;
    D3D11_BUFFER_DESC desc = {};
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.ByteWidth = size;
    desc.BindFlags = bindFlag;
    desc.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA initData = {};
    initData.pSysMem = data;

    HRESULT hr = m_device->CreateBuffer(&desc, &initData, &returnValue);

    if (FAILED(hr))
    {
        ENGINE_ERROR("Failed to create buffer. Error code: " + std::to_string(hr));
    }

    return returnValue;
}

void DirectX11::CreateDeviceAndSwapChain(HWND hwnd)
{
    RECT rect;
    if (GetClientRect(hwnd, &rect))
    {
        auto width = rect.right - rect.left;
        auto height = rect.bottom - rect.top;

        DXGI_SWAP_CHAIN_DESC sd;
        ZeroMemory(&sd, sizeof(sd));
        sd.BufferCount = 1;
        sd.BufferDesc.Width = width;
        sd.BufferDesc.Height = height;
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
            &m_swapChain, &m_device, &featureLevel, &m_context);

        if (FAILED(hr))
        {
            ENGINE_ERROR("Failed to create device and swap chain. Error code: " + std::to_string(hr));
        }
    }
}

void DirectX11::CreateRenderTargetView()
{
    ID3D11Texture2D* pBackBuffer = nullptr;
    HRESULT hr = m_swapChain->GetBuffer(0, __uuidof( ID3D11Texture2D ), (void**) &pBackBuffer);

    if (FAILED(hr))
    {
        ENGINE_ERROR("Failed to create render target view. Error code: " + std::to_string(hr));
        return;
    }

    hr = m_device->CreateRenderTargetView(pBackBuffer, nullptr, &m_backBufferRTV);

    pBackBuffer->Release();

    if (FAILED(hr))
    {
        ENGINE_ERROR("Failed to create render target view. Error code: " + std::to_string(hr));
        return;
    }

    m_context->OMSetRenderTargets(1, &m_backBufferRTV, nullptr);
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
    m_context->RSSetViewports(1, &vp);
}

ID3DBlob* DirectX11::CompileShader(const string& source, const char* entryPoint, const char* shaderModel)
{
    ID3DBlob* errorBlob = nullptr;
    ID3DBlob* returnValue = nullptr;
    const char* src = source.c_str();
    HRESULT hr = D3DCompile(src, source.size(), nullptr, nullptr, nullptr, entryPoint, shaderModel, 0, 0, &returnValue, &errorBlob);

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

        returnValue = nullptr;
        ENGINE_ERROR(errorMessage);
    }

    if (errorBlob != nullptr)
    {
        errorBlob->Release();
    }

    return returnValue;
}