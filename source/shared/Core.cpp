#include "Core.h"
#include "Debug.h"
#include "Time.h"
#include "Camera.h"
#include "SceneManager.h"
#include "GraphicsWrapper.h"
#include "PlatformDetector.h"
#include "RenderCommandHandler.h"
#include "Screen.h"
#include "Input.h"

void Core::Initialize()
{
    // LOAD CONFIG W/ JsonParser

    PlatformDetector::Initialize();
    Time::Initialize();
    Input::Initialize();
    Screen::Initialize(960, 540);            // Hard coded. will use EngineConfig in future.
    GraphicsWrapper::Initialize(APIType::OpenGL);   // Hard coded. will use EngineConfig in future.

    // Physics initialize

    SceneManager::loadScene(0);
}

void Core::Tick()
{
    // Physics Simulation

    // Input Handling
    Input::Tick();
    Screen::PollEvents();

    // Game Logic Processing
    SceneManager::runScene();

    // Scene Rendering
    RenderCommandHandler::Tick();
    GraphicsWrapper::ClearScreen();
    GraphicsWrapper::ExecuteRenderCommands();

    // UI Rendering

    // End of Frame Tasks
    Screen::SwapFrameBuffers();

    // Game Pausing

    // Decommissioning

    // Calculate loop time
    Time::CalculateLoopTime();
}

void Core::UnInitialize()
{
    // Shut everything down, in reverse order
    // Physics.Uninitialize();

    GraphicsWrapper::UnInitialize();
    Screen::UnInitialize();
    Input::UnInitialize();
}