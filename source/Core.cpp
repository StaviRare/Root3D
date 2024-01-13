#include "Core.h"
#include "Debug.h"
#include "Time.h"
#include "Camera.h"
#include "SceneManager.h"
#include "GraphicsWrapper.h"
#include "PlatformDetector.h"
#include "RenderCommandHandler.h"

void Core::Initialize()
{
    PlatformDetector::Initialize();
    Time::Initialize();
    GraphicsWrapper::Initialize(APIType::OpenGL); // hard coded. will use config in future.
    SceneManager::loadScene(0);
}

void Core::Tick()
{
    // Physics Simulation

    // Input Handling

    // Game Logic Processing
    SceneManager::runScene();

    // Scene Rendering
    RenderCommandHandler::Tick();
    GraphicsWrapper::ClearScreen();
    GraphicsWrapper::ExecuteRenderCommands();

    // UI Rendering

    // End of Frame Tasks ()
    //GraphicsWrapper::SwapBuffers();

    // Game Pausing

    // Decommissioning

    // Calculate loop time
    Time::CalculateLoopTime();
}

void Core::UnInitialize()
{
    // Shut everything down, in reverse order
    // Physics.Uninitialize();
}