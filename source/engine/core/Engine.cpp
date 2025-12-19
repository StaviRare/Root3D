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
#include "Config.h"

bool Engine::Initialize()
{
    RuntimeSettings config = Config::Runtime();

    Timer::Initialize();

    float timeSinceEpoch = Timer::TimeSinceEpoch();
    unsigned int seed = static_cast<unsigned int>(timeSinceEpoch);

    Random::InitState(seed);

    Input::Initialize();

    m_window = new Window();
    m_window->Initialize(config.ScreenWidth, config.ScreenHeight);
    auto windowHandle = m_window->GetNativeHandle();

    Graphics::Initialize(config.RenderingAPI, windowHandle);

    Physics::Initialize();
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