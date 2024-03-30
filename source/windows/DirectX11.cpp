#include <d3dcompiler.h>
#include "DirectX11.h"
#include "Screen.h"
#include "Debug.h"
#include "Timer.h"
#include "RenderQueue.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "D3DCompiler.lib")

void DirectX11::Initialize()
{
    HRESULT hr = E_FAIL; // for safety.

    void* nativeHandle = Screen::GetNativeHandle();
    HWND hwnd = reinterpret_cast<HWND>(const_cast<void*>(nativeHandle));

    CreateDeviceAndSwapChain(hwnd);
    CreateRenderTargetView();
    SetupViewport(Screen::GetWidth(), Screen::GetHeight());

    DirectX::XMMATRIX initialData = DirectX::XMMatrixIdentity();
    CreateBuffer(&initialData, sizeof(MVPBuffer), D3D11_BIND_CONSTANT_BUFFER, &mvpBuffer);

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
            
            auto lightCommands = RenderQueue::GetLightRenderCommands();
            auto objectCommands = RenderQueue::GetObjectRenderCommands();

            for (const auto& command : objectCommands)
            {
                // Shader. Move to shaderManager //

                if (command.shader->ID == 0)
                {
                    command.shader->ID = CreateShaderProgram(command.shader->vertexCode, command.shader->fragmentCode);
                }

                ShaderProgram* shaderProgram = nullptr;

                for (auto& sp : shaderMap)
                {
                    if (sp.ID == command.shader->ID)
                    {
                        shaderProgram = &sp;
                        break;
                    }
                }

                // Shader. Move to shaderManager //

                if (shaderProgram != nullptr)
                {
                    context->IASetInputLayout(shaderProgram->inputLayout);
                    context->VSSetShader(shaderProgram->vertexShader, nullptr, 0);
                    context->PSSetShader(shaderProgram->pixelShader, nullptr, 0);

                    
                    mvpBufferData.model = DirectX::XMMatrixTranspose(DirectX::XMMATRIX(command.modelMatrix));
                    mvpBufferData.view = DirectX::XMMatrixTranspose(DirectX::XMMATRIX(vm));
                    mvpBufferData.projection = DirectX::XMMatrixTranspose(DirectX::XMMATRIX(pm));

                    context->UpdateSubresource(mvpBuffer, 0, nullptr, &mvpBufferData, 0, 0);
                    context->VSSetConstantBuffers(0, 1, &mvpBuffer);

                    if (command.texture)
                    {
                        BindTexture(*( command.texture ));
                    }

                    CreateBuffer(const_cast<int*>( command.indices ), sizeof(int) * command.indicesSize, D3D11_BIND_INDEX_BUFFER, &indexBuffer);
                    CreateBuffer(const_cast<float*>( command.vertices ), sizeof(float) * 3 * command.verticesSize, D3D11_BIND_VERTEX_BUFFER, &vertexBuffer);
                    CreateBuffer(const_cast<float*>( command.texCoords ), sizeof(float) * 2 * command.texCoordsSize, D3D11_BIND_VERTEX_BUFFER, &texCoordBuffer);
                    CreateBuffer(const_cast<float*>( command.normals ), sizeof(float) * 3 * command.normalsSize , D3D11_BIND_VERTEX_BUFFER, &normalBuffer);

                    UINT strides[3] = {
                        sizeof(float) * 3, // Position
                        sizeof(float) * 3, // Normal
                        sizeof(float) * 2  // Texture Coordinate
                    };

                    UINT offsets[3] = {0, 0, 0};
                    ID3D11Buffer* buffers[3] = {vertexBuffer, normalBuffer, texCoordBuffer};
                    context->IASetVertexBuffers(0, 3, buffers, strides, offsets);

                    context->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
                    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
                    context->DrawIndexed(command.indicesSize, 0, 0);

                    if (indexBuffer) indexBuffer->Release();
                    if (vertexBuffer) vertexBuffer->Release();
                    if (normalBuffer) texCoordBuffer->Release();
                    if (texCoordBuffer) texCoordBuffer->Release();
                }
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
    if (mvpBuffer) mvpBuffer->Release();
    if (backBufferRTV) backBufferRTV->Release();
    if (swapChain) swapChain->Release();
    if (context) context->Release();
    if (device) device->Release();
    if (depthStencilView) depthStencilView->Release();

    for (auto program : shaderMap)
    {
        if (program.inputLayout) program.inputLayout->Release();
        if (program.pixelShader) program.pixelShader->Release();
        if (program.vertexShader) program.vertexShader->Release();
    }

    shaderMap.clear();

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

void DirectX11::CompileShader(const string& source, const char* entryPoint, const char* shaderModel, ID3DBlob** blobOut)
{
    ID3DBlob* errorBlob = nullptr;
    const char* src = source.c_str();
    HRESULT hr = D3DCompile(src, strlen(src), nullptr, nullptr, nullptr, entryPoint, shaderModel, 0, 0, blobOut, &errorBlob);

    if (FAILED(hr))
    {
        if (errorBlob)
        {
            OutputDebugStringA(reinterpret_cast<const char*>( errorBlob->GetBufferPointer() ));
            errorBlob->Release();
        }

        *blobOut = nullptr; // return null blob on failure
    }

    if (errorBlob)
    {
        errorBlob->Release();
    }
}

unsigned int DirectX11::CreateShaderProgram(const string& vertexSource, const string& fragmentSource)
{
    // Compile vertex shader
    ShaderProgram shaderProgram;
    ID3DBlob* vsBlob = nullptr;
    CompileShader(vertexSource, "VS", "vs_4_0", &vsBlob);

    if (!vsBlob)
    {
        Debug::LogError("Failed to compile vertex shader.");
        return 0;
    }

    // Create vertex shader
    HRESULT hr = device->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, &shaderProgram.vertexShader);

    if (FAILED(hr))
    {
        Debug::LogError("Failed to create vertex shader.");
        vsBlob->Release();
        return 0;
    }

    // Define input layout for vertex data
    D3D11_INPUT_ELEMENT_DESC layout[] = 
    {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 1, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 2, 0, D3D11_INPUT_PER_VERTEX_DATA, 0}
    };

    // Create input layout
    hr = device->CreateInputLayout(layout, ARRAYSIZE(layout), vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &shaderProgram.inputLayout);
    vsBlob->Release(); // Release the vertex shader blob after creating the input layout

    if (FAILED(hr))
    {
        Debug::LogError("Failed to create input layout.");
        return 0;
    }

    // Compile pixel shader
    ID3DBlob* psBlob = nullptr;
    CompileShader(fragmentSource, "PS", "ps_4_0", &psBlob);

    if (!psBlob)
    {
        Debug::LogError("Failed to compile pixel shader.");
        return 0;
    }

    // Create pixel shader
    hr = device->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, &shaderProgram.pixelShader);
    psBlob->Release();

    if (FAILED(hr))
    {
        Debug::LogError("Failed to create pixel shader.");
        return 0;
    }

    nextShaderID++;
    shaderProgram.ID = nextShaderID;
    shaderMap.push_back(shaderProgram);

    return nextShaderID;
}