#include "Core.h"
#include "Debug.h"
#include "Timer.h"
#include "SceneManager.h"
#include "Graphics.h"
#include "PlatformDetector.h"
#include "RenderCommandHandler.h"
#include "Screen.h"
#include "Input.h"

void Core::Initialize()
{
    // LOAD CONFIG W/ JsonParser

    PlatformDetector::Initialize();
    Timer::Initialize();
    Input::Initialize();
    Screen::Initialize(960, 540);            // Hard coded. will use EngineConfig in future.
    Graphics::Initialize(APIType::DirectX11);   // Hard coded. will use EngineConfig in future.

    // Physics initialize

    SceneManager::LoadScene(0);
}

void Core::Tick()
{
    // Physics Simulation

    // Input Handling
    Input::Tick();
    Screen::PollEvents();

    // Game Logic Processing
    SceneManager::RunScene();

    // Scene Rendering
    RenderCommandHandler::Tick();
    Graphics::ClearScreen();
    Graphics::ExecuteRenderCommands();

    // UI Rendering

    // End of Frame Tasks
    Graphics::SwapFrameBuffers();

    // Game Pausing

    // Decommissioning

    // Calculate loop time
    Timer::CalculateLoopTime();
}

void Core::UnInitialize()
{
    // Shut everything down, in reverse order
    // Physics.Uninitialize();

    Graphics::UnInitialize();
    Screen::UnInitialize();
    Input::UnInitialize();
}