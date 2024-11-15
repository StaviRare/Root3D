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

void Core::Initialize()
{
    PlatformDetector::Initialize();
    Timer::Initialize();
    Input::Initialize();
    Screen::Initialize();
    Graphics::Initialize();
    Physics::Initialize();
    SceneManager::LoadScene(0); // should not be here.
}

void Core::UnInitialize()
{
    // Shut everything down, in reverse order
    Physics::UnInitialize();
    Graphics::UnInitialize();
    Screen::UnInitialize();
    Input::UnInitialize();
}

void Core::Resume()
{
    Screen::Resume();
}

void Core::Pause()
{
    Screen::Pause();
}

void Core::Tick()
{
    // Calculate loop time
    Timer::CalculateLoopTime();

    // Handle fixed update
    while (Timer::accumulatedTime >= Timer::fixedTimeStep)
    {
        PhysicsHandler::SetData(); // Should not be here.
        Physics::Simulate();
        PhysicsHandler::GetData(); // Should not be here.
        Timer::UpdateFixedTime();
    }

    // Handle variable update
    Input::Tick();
    Screen::PollEvents();
    SceneManager::RunScene(); // Should not be here.
    RenderCommandHandler::Tick(); // Should not be here.
    Graphics::ClearScreen();
    Graphics::ExecuteRenderCommands();
    Graphics::SwapFrameBuffers();
}