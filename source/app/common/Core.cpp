#include "Core.h"
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

void Core::Initialize()
{
    PlatformDetector::Initialize();
    Timer::Initialize();
    Input::Initialize();
    Screen::Initialize();
    Graphics::Initialize();
    Physics::Initialize();
    SceneManager::LoadScene(0);     // Should not be here. Will register to Tick loop.
}

void Core::UnInitialize()
{
    // Shut everything down, in reverse order
    SceneManager::UnloadScene();
    Physics::UnInitialize();
    Graphics::UnInitialize();
    Screen::UnInitialize();
    Input::UnInitialize();
}

void Core::Resume()
{
    Screen::Resume();
    Timer::Resume();
}

void Core::Pause()
{
    Timer::Pause();
    Screen::Pause();
}

void Core::Tick()
{
    // Calculate loop time
    Timer::CalculateLoopTime();

    // Handle fixed update
    while (Timer::accumulatedTime >= Timer::fixedTimeStep)
    {
        PhysicsHandler::SetData();  // Should not be here. Will register to Tick loop.
        Physics::Simulate();
        PhysicsHandler::GetData();  // Should not be here. Will register to Tick loop.
        Timer::UpdateFixedTime();
    }

    // Handle variable update
    Input::Tick();
    Screen::PollEvents();
    EntityPool::Tick();             // Should not be here. Will register to Tick loop.
    RenderCommandHandler::Tick();   // Should not be here. Will register to Tick loop.
    Graphics::ClearScreen();
    Graphics::ExecuteRenderCommands();
    Graphics::SwapFrameBuffers();
}