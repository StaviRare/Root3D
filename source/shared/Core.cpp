#include "Core.h"
#include "Debug.h"
#include "Time.h"
#include "Camera.h"
#include "SceneManager.h"
#include "GraphicsWrapper.h"
#include "PlatformDetector.h"
#include "RenderCommandHandler.h"
#include "WindowWrapper.h"
#include "Input.h"

static Window* window;

void Core::Initialize()
{
    // LOAD CONFIG W/ JsonParser

    PlatformDetector::Initialize();
    Time::Initialize();
    Input::Initialize();
    WindowWrapper::Initialize(960, 540);            // Hard coded. will use EngineConfig in future.
    GraphicsWrapper::Initialize(APIType::OpenGL);   // Hard coded. will use EngineConfig in future.

    // Physics initialize

    SceneManager::loadScene(0);
}

void Core::Tick()
{
    // Physics Simulation

    // Input Handling
    Input::Tick();
    WindowWrapper::PollEvents();

    // Game Logic Processing
    SceneManager::runScene();

    // Scene Rendering
    RenderCommandHandler::Tick();
    GraphicsWrapper::ClearScreen();
    GraphicsWrapper::ExecuteRenderCommands();

    // UI Rendering

    // End of Frame Tasks
    WindowWrapper::SwapBuffers();

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
    WindowWrapper::UnInitialize();
    Input::UnInitialize();
}