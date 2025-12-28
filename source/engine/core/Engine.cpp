#include <vector>

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

    // Window events:
    std::vector<WindowEvent> events = m_window->PollEvents();

    for (auto& e : events)
    {
        switch (e.type)
        {
            case WindowEventType::Resize:
            {
                Graphics::Resize(e.width, e.height);
                break;
            }
            case WindowEventType::Close:
            {
                // debug win:
                m_isRunning = false;
                // debug win:
                // debug win:



                Debug::Log("WindowClose");
                // Pass it to SceneManager
                break;
            }
            case WindowEventType::FocusGained:
            {
                Debug::Log("WindowFocusGained");
                // Pass it to SceneManager
                break;
            }
            case WindowEventType::FocusLost:
            {
                Debug::Log("WindowFocusLost");
                // Pass it to SceneManager
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
}

bool Engine::IsRunning()
{
    return m_isRunning;
}

EngineConfig Engine::BuildEngineConfig(Platform platform)
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
        case Platform::Windows:
        {
            config.window.width = 960;
            config.window.height = 540;
            config.window.fullscreen = false;
            config.graphics.maxLights = 20;
            config.graphics.graphicsAPI = GraphicsAPI::DirectX11;
            break;
        }
        case Platform::Android:
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