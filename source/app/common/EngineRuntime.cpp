#include "EngineRuntime.h"
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
#include "EntityPool.h"

void EngineRuntime::Initialize()
{
    PlatformDetector::Initialize();
    Timer::Initialize();
    Input::Initialize();
    Screen::Initialize();
    Graphics::Initialize();
    Physics::Initialize();
    SceneManager::LoadScene(0);
}

void EngineRuntime::UnInitialize()
{
    // Shut everything down, in reverse order
    SceneManager::UnloadScene();
    Physics::UnInitialize();
    Graphics::UnInitialize();
    Screen::UnInitialize();
    Input::UnInitialize();
}

void EngineRuntime::Resume()
{
    Screen::Resume();
    Timer::Resume();
}

void EngineRuntime::Pause()
{
    Timer::Pause();
    Screen::Pause();
}

void EngineRuntime::Tick()
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
    EntityPool::Tick();

    // LateUpdate:
    // ToDo!

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