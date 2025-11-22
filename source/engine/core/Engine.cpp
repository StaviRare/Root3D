#include "Engine.h"
#include "Debug.h"
#include "Timer.h"
#include "SceneManager.h"
#include "Graphics.h"
#include "PlatformDetector.h"
#include "RenderCommandHandler.h"
#include "Screen.h"
#include "Input.h"
#include "Physics.h"
#include "PhysicsHandler.h"
#include "Random.h"

bool Engine::Initialize()
{
    PlatformDetector::Initialize();
    Timer::Initialize();

    float timeSinceEpoch = Timer::TimeSinceEpoch();
    unsigned int seed = static_cast<unsigned int>(timeSinceEpoch);
    Random::InitState(seed);

    Input::Initialize();
    Screen::Initialize();
    Graphics::Initialize();
    Physics::Initialize();
    SceneManager::Initialze();

    // For now.
    isRunning = true;
    return isRunning;
}

void Engine::UnInitialize()
{
    // Shut everything down, in reverse order
    SceneManager::UnInitialize();
    Physics::UnInitialize();
    Graphics::UnInitialize();
    Screen::UnInitialize();
    Input::UnInitialize();
}

void Engine::Resume()
{
    Screen::Resume();
    Timer::Resume();
}

void Engine::Pause()
{
    Timer::Pause();
    Screen::Pause();
}

void Engine::Tick()
{
    // Calculate loop time
    Timer::CalculateLoopTime();

    // FixedUpdate:
    while (Timer::accumulatedTime >= Timer::fixedTimeStep)
    {
        PhysicsHandler::SetData(); // sync-in
        Physics::Simulate();
        PhysicsHandler::GetData(); // sync-out
        Timer::UpdateFixedTime();
    }

    // InputEvents:
    Input::Tick();
    Screen::PollEvents();

    // Update:
    SceneManager::Tick();

    // LateUpdate:
    SceneManager::LateTick();

    // ScenePreRender:
    Graphics::ClearScreen();
    RenderCommandHandler::Tick();

    // SceneRender:
    Graphics::ExecuteRenderCommands();
    Graphics::SwapFrameBuffers();

    // ScenePostRender:
    // ToDo!

    // GuiRender:
    // ToDo!
}

bool Engine::IsRunning()
{
    return isRunning;
}