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
#include "Platform.h"
#include "PlatformEventQueue.h"

bool Engine::Initialize()
{
    // Engine config
    PlatformType currentPlatform = Platform::GetType();
    EngineConfig engineConfig = BuildEngineConfig(currentPlatform);

    TimeDesc timeConfig = engineConfig.time;
    Timer::Initialize(timeConfig);

    float timeSinceEpoch = Timer::TimeSinceEpoch();
    unsigned int seed = static_cast<unsigned int>(timeSinceEpoch);
    Random::InitState(seed);

    Input::Initialize();

    WindowDesc windowConfig = engineConfig.window;
    m_window = new Window();
    m_window->Initialize(windowConfig);
    
    
    GraphicsDesc graphicsConfig = engineConfig.graphics;
    graphicsConfig.windowHandle = m_window->GetNativeHandle();
    Graphics::Initialize(graphicsConfig);

    PhysicsDesc physicsConfig = engineConfig.physics;
    Physics::Initialize(physicsConfig);


    SceneManager::Initialize();

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

    PlatformEvent e;
    while (PlatformEventQueue::Poll(e)) 
    {
        switch (e.type)
        {
            //// Debug::Log("AppPause");
            //// Debug::Log("AppResume");
            //// Debug::Log("WindowFocusLost");
            //// Debug::Log("WindowFocusGained");
            // Pass it to SceneManager

            case EventType::Close:
            {
                m_isRunning = false;
                break;
            }
            case EventType::Resize:
            {
                Graphics::Resize(e.width, e.height);
                break;
            }
            case EventType::SurfaceCreated:
            {
                void* windowHandle = m_window->GetNativeHandle();
                Graphics::OnSurfaceRecreated(windowHandle);
                break;
            }
            case EventType::SurfaceDestroyed:
            {
                Graphics::OnSurfaceLost();
                break;
            }
        }
    }

    // Update:
    SceneManager::Tick();

    // Late update:
    SceneManager::LateTick();

    // Scene pre render:
    RenderManager::PreRender();

    // Scene render:
    RenderManager::Render();

    // Scene post render:
    RenderManager::PostRender();

    // GuiRender:
    // ToDo!

    SceneManager::EndFrame();
}

bool Engine::IsRunning()
{
    return m_isRunning;
}

EngineConfig Engine::BuildEngineConfig(PlatformType platform)
{
    EngineConfig config;

    // For all platforms
    config.window.title = "Root3D";
    config.time.timeScale = 1.00f;
    config.time.maxDeltaTime = 0.10f;
    config.time.fixedTimeStep = 0.02f;
    config.physics.gravity = Vector3(0, -9.81f, 0);
    config.physics.physicsAPI = PhysicsAPI::Jolt;

    // Per platform
    switch (platform)
    {
        case PlatformType::Windows:
        {
            config.window.width = 960;
            config.window.height = 540;
            config.window.fullscreen = false;
            config.graphics.maxLights = 20;
            config.graphics.graphicsAPI = GraphicsAPI::DirectX11;
            break;
        }
        case PlatformType::Android:
        {
            config.window.width = 960;
            config.window.height = 540;
            config.window.fullscreen = true;
            config.graphics.maxLights = 10;
            config.graphics.graphicsAPI = GraphicsAPI::OpenGLES1;
            break;
        }
    }

    return config;
}