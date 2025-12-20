#include "Engine.h"
#include "Debug.h"
#include "Timer.h"
#include "SceneManager.h"
#include "Graphics.h"
#include "RenderManager.h"
#include "Input.h"
#include "Physics.h"
#include "PhysicsHandler.h"
#include "Random.h"
#include "SystemPlatform.h"

bool Engine::Initialize()
{
    // Engine config
    Platform currentPlatform = SystemPlatform::Get();
    EngineConfig engineConfig = BuildEngineConfig(currentPlatform);
    WindowConfig windowConfig = engineConfig.window;
    GraphicsConfig graphicsConfig = engineConfig.graphics;
    PhysicsConfig physicsConfig = engineConfig.physics;

    Timer::Initialize();

    float timeSinceEpoch = Timer::TimeSinceEpoch();
    unsigned int seed = static_cast<unsigned int>(timeSinceEpoch);
    Random::InitState(seed);

    Input::Initialize();

    m_window = new Window();
    m_window->Initialize(windowConfig);
    auto windowHandle = m_window->GetNativeHandle();
    graphicsConfig.windowHandle = windowHandle;

    Graphics::Initialize(graphicsConfig);

    Physics::Initialize(physicsConfig);
    SceneManager::Initialze();

    // For now.
    m_isRunning = true;
    return m_isRunning;
}

void Engine::UnInitialize()
{
    // Shut everything down, in reverse order
    SceneManager::UnInitialize();
    Physics::UnInitialize();
    Graphics::UnInitialize();
    m_window->UnInitialize();
    Input::UnInitialize();
}

void Engine::Resume()
{
    m_window->Resume();
    Timer::Resume();
}

void Engine::Pause()
{
    Timer::Pause();
    m_window->Pause();
}

void Engine::Tick()
{
    // Calculate loop time
    Timer::CalculateLoopTime();

    // Fixed update:
    while (Timer::s_accumulated >= Timer::s_fixedStep)
    {
        PhysicsHandler::SetData(); // sync-in
        Physics::Simulate();
        PhysicsHandler::GetData(); // sync-out
        Timer::UpdateFixedTime();
    }

    // Input events:
    Input::Tick();
    m_window->PollEvents();

    // swithc events
    // foreach
    // if resize
    // update graphics


    // Update:
    SceneManager::Tick();

    // Late update:
    SceneManager::LateTick();

    // Scene pre render:
    //Graphics::ClearScreen();
    RenderManager::PreRender();

    // Scene render:
    //Graphics::ExecuteRenderCommands();
    //Graphics::SwapFrameBuffers();
    RenderManager::Render();

    // Scene post render:
    RenderManager::PostRender();

    // GuiRender:
    // ToDo!
}

bool Engine::IsRunning()
{
    return m_isRunning;
}

EngineConfig Engine::BuildEngineConfig(Platform platform)
{
    EngineConfig returnValue;

    // For all platforms
    returnValue.window.title = "Root3D";
    returnValue.physics.Gravity = Vector3(0, -9.81f, 0);
    returnValue.physics.PhysicsTypeAPI = PhysicsAPI::Jolt;

    // Per platform
    switch (platform)
    {
        case Platform::Windows:
        returnValue.window.width = 960;
        returnValue.window.height = 540;
        returnValue.window.fullscreen = false;
        returnValue.graphics.MaxLights = 20;
        returnValue.graphics.RenderingAPI = GraphicsAPI::DirectX11;
        break;

        case Platform::Android:
        returnValue.window.width = 960;
        returnValue.window.height = 540;
        returnValue.window.fullscreen = true;
        returnValue.graphics.MaxLights = 10;
        returnValue.graphics.RenderingAPI = GraphicsAPI::OpenGLES1;
        break;
    }

    return returnValue;
}