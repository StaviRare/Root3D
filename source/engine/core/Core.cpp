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

void Core::Initialize()
{
    PlatformDetector::Initialize();
    Timer::Initialize();
    Input::Initialize();
    Screen::Initialize();
    Graphics::Initialize();
    Physics::Initialize();
    SceneManager::LoadScene(0);
}

void Core::Tick()
{
    // Calculate loop time
    Timer::CalculateLoopTime();

    // Handle fixed update
    while (Timer::accumulatedTime >= Timer::fixedTimeStep)
    {
        Physics::Simulate();
        Timer::UpdateFixedTime();
    }

    // Handle variable update
    Input::Tick();
    Screen::PollEvents();
    SceneManager::RunScene();
    RenderCommandHandler::Tick();
    Graphics::ClearScreen();
    Graphics::ExecuteRenderCommands();
    Graphics::SwapFrameBuffers();
}

void Core::UnInitialize()
{
    // Shut everything down, in reverse order
    Physics::UnInitialize();
    Graphics::UnInitialize();
    Screen::UnInitialize();
    Input::UnInitialize();
}